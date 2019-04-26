// SPDX-License-Identifier: GPL-2.0-only
/*
 * Continuous splash display implementation for LK with support for
 * Qualcomm's MDP3, MDP4 and MDP5. Allows reusing the display initialized
 * by a previous bootloader by reading the display configuration from the
 * hardware registers.
 *
 * Copyright (C) 2019-2022  Stephan Gerhold <stephan@gerhold.net>
 */

#include <arch/arm/mmu.h>
#include <bits.h>
#include <debug.h>
#include <reg.h>

#include <kernel/event.h>
#include <kernel/thread.h>

#if MDP3
#include <mdp3.h>
#elif MDP4
#include <dev/lcdc.h>
#include <mdp4.h>
#elif MDP5
#include <mdp5.h>
#else
#error Unknown display controller :(
#endif

#include <dev/fbcon.h>
#include <platform.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <target.h>

#define MDP_X(val)	BITS_SHIFT(val, 16, 0)
#define MDP_Y(val)	BITS_SHIFT(val, 32, 16)

#if !defined(MDP_GDSCR) && defined(MDSS_GDSCR)
#define MDP_GDSCR	MDSS_GDSCR
#endif

extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
extern int check_ddr_addr_range_bound(uintptr_t start, uint32_t size);

static void mdp_cmd_start_refresh(struct fbcon_config *fb);

#if MDP3 || MDP4

static void mdp_refresh(struct fbcon_config *fb)
{
	writel(1, MDP_DMA_P_START);
}

static bool mdp_read_config(struct fbcon_config *fb)
{
	uint32_t config, size, stride, out_xy, format, intf_sel;
	bool cmd_mode, auto_refresh = false;

	fb->base = (void*)readl(MDP_DMA_P_BUF_ADDR);
	if (!fb->base) {
		dprintf(CRITICAL, "No continuous splash: DMA_P not active\n");
		return false;
	}

	config = readl(MDP_DMA_P_CONFIG);
	size = readl(MDP_DMA_P_SIZE);
	stride = readl(MDP_DMA_P_BUF_Y_STRIDE);
	out_xy = readl(MDP_DMA_P_OUT_XY);
	format = BITS_SHIFT(config, 26, 25);
#if MDP3
	intf_sel = BITS_SHIFT(config, 20, 19); /* OUT_SEL */
	cmd_mode = intf_sel == 0x1; /* == DSI_CMD? */
#elif MDP4
	intf_sel = BITS_SHIFT(readl(MDP_DISP_INTF_SEL), 1, 0); /* PRIM_INTF_SEL */
	cmd_mode = intf_sel == 0x2; /* == DSI Command Mode? */
#endif
#ifdef MDP_AUTOREFRESH_CONFIG_P
	/* Crashes on MDP4/msm8960, mdp vsync clock is not on? */
	auto_refresh = !!(readl(MDP_AUTOREFRESH_CONFIG_P) & BIT(28));
#endif

	dprintf(INFO, "MDP3/4 continuous splash detected: base: %p, stride: %d, "
		"size: %dx%d, out: (%d,%d), config: %#x (format: %#x), intf: %d "
		"(cmd mode: %d, auto refresh: %d)\n",
		fb->base, stride,
		MDP_X(size), MDP_Y(size), MDP_X(out_xy), MDP_Y(out_xy),
		config, format, intf_sel, cmd_mode, auto_refresh
	);

	fb->width = MDP_X(size);
	fb->height = MDP_Y(size);

	switch (format) {
	case 0x0: /* RGB888 */
		fb->stride = stride / 3;
		fb->bpp = 3 * 8;
		fb->format = FB_FORMAT_RGB888;
		break;
	case 0x1: /* RGB565 */
		fb->stride = stride / 2;
		fb->bpp = 2 * 8;
		fb->format = FB_FORMAT_RGB565;
		break;
	case 0x2: /* xRGB888 */
		fb->stride = stride / 4;
		fb->bpp = 4 * 8;
		fb->format = FB_FORMAT_RGB888;
		break;
	default:
		dprintf(CRITICAL, "Unsupported MDP3/4 format: %#x\n", format);
		return false;
	}

	if (cmd_mode && !auto_refresh)
		mdp_cmd_start_refresh(fb);

	return true;
}

/* MDP3 || MDP4 */
#elif MDP5

struct mdp5_pipe {
	const char *name;
	uint32_t base;
	uint32_t flush;
};

static const struct mdp5_pipe mdp5_pipes[] = {
	{
		.name = "VIG_0",
		.base = MDP_VP_0_VIG_0_BASE,
		.flush = BIT(0),
	},
	{
		.name = "RGB_0",
		.base = MDP_VP_0_RGB_0_BASE,
		.flush = BIT(3),
	},
	{
		.name = "DMA_0",
		.base = MDP_VP_0_DMA_0_BASE,
		.flush = BIT(11),
	},
};

static void mdp_refresh(struct fbcon_config *fb)
{
	writel(1, MDP_CTL_0_BASE + CTL_START);
}

static const struct mdp5_pipe *mdp5_find_pipe(struct fbcon_config *fb)
{
	const struct mdp5_pipe *pipe = mdp5_pipes;
	const struct mdp5_pipe *pipe_end = pipe + ARRAY_SIZE(mdp5_pipes);

	for (; pipe < pipe_end; pipe++) {
		fb->base = (void*)readl(pipe->base + PIPE_SSPP_SRC0_ADDR);
		if (fb->base)
			return pipe;
	}
	return NULL;
}

static bool mdp_read_config(struct fbcon_config *fb)
{
	const struct mdp5_pipe *pipe = mdp5_find_pipe(fb);
	uint32_t src_size, img_size, src_xy, out_size, out_xy, stride, format, bpp;
	bool cmd_mode, auto_refresh;

	if (!pipe) {
		dprintf(CRITICAL, "No continuous splash: cannot find active pipe\n");
		return false;
	}

	src_size = readl(pipe->base + PIPE_SSPP_SRC_SIZE);
	img_size = readl(pipe->base + PIPE_SSPP_SRC_IMG_SIZE);
	src_xy = readl(pipe->base + PIPE_SSPP_SRC_XY);
	out_size = readl(pipe->base + PIPE_SSPP_SRC_OUT_SIZE);
	out_xy = readl(pipe->base + PIPE_SSPP_OUT_XY);
	stride = readl(pipe->base + PIPE_SSPP_SRC_YSTRIDE);
	format = readl(pipe->base + PIPE_SSPP_SRC_FORMAT);
	bpp = BITS_SHIFT(format, 10, 9) + 1; /* SRC_BPP */
	cmd_mode = !!(readl(MDP_CTL_0_BASE + CTL_TOP) & BIT(17)); /* MODE_SEL */
	auto_refresh = !!(readl(MDP_PP_0_BASE + MDSS_MDP_REG_PP_AUTOREFRESH_CONFIG) & BIT(31));

	dprintf(INFO, "MDP5 continuous splash detected: pipe: %s, base: %p, stride: %d, "
		"src: %dx%d (%d,%d), img: %dx%d, out: %dx%d (%d,%d), format: %#x (bpp: %d) "
		"(cmd mode: %d, auto refresh: %d)\n",
		pipe->name, fb->base, stride,
		MDP_X(src_size), MDP_Y(src_size), MDP_X(src_xy), MDP_Y(src_xy),
		MDP_X(img_size), MDP_Y(img_size),
		MDP_X(out_size), MDP_Y(out_size), MDP_X(out_xy), MDP_Y(out_xy),
		format, bpp, cmd_mode, auto_refresh
	);

	fb->width = MDP_X(img_size);
	fb->height = MDP_Y(img_size);
	fb->stride = stride / bpp;
	fb->bpp = bpp * 8;

	if (bpp == 2)
		fb->format = FB_FORMAT_RGB565;
	else
		fb->format = FB_FORMAT_RGB888;

	if (cmd_mode && !auto_refresh)
		mdp_cmd_start_refresh(fb);

	return true;
}
#endif /* MDP5 */

static event_t refresh_event;

static int mdp_cmd_refresh_loop(void *data)
{
	struct fbcon_config *fb = data;

	while (true) {
		/* Limit refresh to 50 Hz to avoid overlapping display updates */
		event_wait(&refresh_event);
		mdp_refresh(fb);
		thread_sleep(20);
	}
	return 0;
}

static void mdp_cmd_signal_refresh(void)
{
	event_signal(&refresh_event, false);
}

static void mdp_cmd_start_refresh(struct fbcon_config *fb)
{
	thread_t *thr;

	event_init(&refresh_event, false, EVENT_FLAG_AUTOUNSIGNAL);

	thr = thread_create("display-refresh", &mdp_cmd_refresh_loop,
			    fb, HIGH_PRIORITY, DEFAULT_STACK_SIZE);
	if (!thr) {
		dprintf(CRITICAL, "Failed to create display-refresh thread\n");
		return;
	}

	thread_resume(thr);
	fb->update_start = mdp_cmd_signal_refresh;
}

static bool mdp_check_config(struct fbcon_config *fb)
{
	uint32_t size;

	if (fb->stride == 0 || fb->width == 0 || fb->height == 0)
		return false;

	size = fb->stride * fb->bpp/8 * fb->height;
	if (check_aboot_addr_range_overlap((uintptr_t)fb->base, size) ||
	    check_ddr_addr_range_bound((uintptr_t)fb->base, size)) {
		dprintf(CRITICAL, "Invalid memory region for continuous splash"
			" (overlap or out of bounds)\n");
		return false;
	}

	/* Make sure FB memory region is mapped */
	arm_mmu_map_region((addr_t)fb->base, (addr_t)fb->base, size,
			   MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH |
			   MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN);
	arm_mmu_flush();
	return true;
}

void target_display_init(const char *panel_name)
{
	static struct fbcon_config fb;

#ifdef MDP_GDSCR
	/*
	 * Reading MDP registers will fail if necessary clocks/power domains
	 * are not enabled. Check if the MDP power domain (GDSC) is enabled
	 * to try to avoid crashing if the clocks are disabled.
	 */
	if (!(readl(MDP_GDSCR) & GDSC_POWER_ON_BIT)) {
		dprintf(CRITICAL, "No continuous splash: MDP GDSC is not enabled\n");
		return;
	}
#endif

	if (!mdp_read_config(&fb) || !mdp_check_config(&fb))
		return;

	fbcon_setup(&fb);
}
