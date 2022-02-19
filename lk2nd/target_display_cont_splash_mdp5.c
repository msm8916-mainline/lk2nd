#include <arch/arm/mmu.h>
#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <string.h>

#include <dev/fbcon.h>
#include <kernel/event.h>
#include <kernel/thread.h>
#include <mdp5.h>
#include <platform.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <target.h>
#include <target/display.h>

/* Assume ctl 0 is used */
#define MDP_CTL_BASE			MDP_CTL_0_BASE

#define MDP_CTL_TOP_MODE_SEL_CMD	BIT(17)

static struct fbcon_config fb = {
	.bpp    = 24,
	.format = FB_FORMAT_RGB888,
};

struct pipe {
	unsigned int base;
	uint32_t type;
};

/*
 * VIG/DMA pipes are weird, but some devices use that...
 * Might need more changes to make them work properly.
 */
static const struct pipe pipes[] = {
	{
		.base = MDP_VP_0_RGB_0_BASE,
		.type = MDSS_MDP_PIPE_TYPE_RGB,
	},
	{
		.base = MDP_VP_0_VIG_0_BASE,
		.type = MDSS_MDP_PIPE_TYPE_VIG,
	},
	{
		.base = MDP_VP_0_DMA_0_BASE,
		.type = MDSS_MDP_PIPE_TYPE_DMA,
	},
};

extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
extern int check_ddr_addr_range_bound(uintptr_t start, uint32_t size);

static event_t refresh_event;

static int mdp5_cmd_refresh_loop(void *data)
{
	while (true) {
		event_wait(&refresh_event);
		event_unsignal(&refresh_event);

		writel(1, MDP_CTL_BASE + CTL_START);
		/* Limit to 50 Hz to prevent overlapping display updates */
		thread_sleep(20);
	}

	return 0;
}

static void mdp5_cmd_signal_refresh(void)
{
	event_signal(&refresh_event, false);
}

static void mdp5_cmd_start_refresh(struct fbcon_config *fb)
{
	thread_t *thr;

	event_init(&refresh_event, false, 0);

	thr = thread_create("display-refresh", &mdp5_cmd_refresh_loop,
			    NULL, HIGH_PRIORITY, DEFAULT_STACK_SIZE);
	if (!thr) {
		dprintf(CRITICAL, "Failed to create display-refresh thread\n");
		return;
	}

	thread_resume(thr);
	fb->update_start = mdp5_cmd_signal_refresh;
}

static void mmu_map_fb(addr_t addr, uint32_t size)
{
	addr_t end = addr + size;

	dprintf(INFO, "Mapping framebuffer region at %lx, size %u\n", addr, size);

	for (; addr < end; addr += MB)
		arm_mmu_map_section(addr, addr, MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH |
						MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN);
	arm_mmu_flush();
}

static int mdp5_read_config(struct fbcon_config *fb)
{
	const struct pipe *pipe = pipes, *pipe_end = pipe + ARRAY_SIZE(pipes);
	uint32_t stride, src_size, out_size, src_xy, out_xy, size;
	bool cmd_mode;

	for (; pipe < pipe_end; pipe++) {
		fb->base = (void*) readl(pipe->base + PIPE_SSPP_SRC0_ADDR);
		if (fb->base)
			break;
	}
	if (pipe == pipe_end) {
		dprintf(CRITICAL, "No continuous splash: cannot find active pipe\n");
		return -1;
	}

	stride = readl(pipe->base + PIPE_SSPP_SRC_YSTRIDE);
	src_size = readl(pipe->base + PIPE_SSPP_SRC_IMG_SIZE);
	out_size = readl(pipe->base + PIPE_SSPP_SRC_OUT_SIZE);
	src_xy = readl(pipe->base + PIPE_SSPP_SRC_XY);
	out_xy = readl(pipe->base + PIPE_SSPP_OUT_XY);
	cmd_mode = !!(readl(MDP_CTL_BASE + CTL_TOP) & MDP_CTL_TOP_MODE_SEL_CMD);

	dprintf(INFO, "Continuous splash detected: pipe: %d, base: %p, stride: %d, "
		"source: %dx%d (%d,%d), output: %dx%d (%d,%d) (cmd mode: %d)\n",
		pipe->type, fb->base, stride,
		src_size & 0xffff, src_size >> 16, src_xy & 0xffff, src_xy >> 16,
		out_size & 0xffff, out_size >> 16, out_xy & 0xffff, out_xy >> 16,
		cmd_mode
	);

	fb->stride = stride / (fb->bpp/8);
	fb->width = fb->stride;
	fb->height = out_size >> 16;

	if (cmd_mode)
		mdp5_cmd_start_refresh(fb);

	// Validate parameters
	if (fb->stride == 0 || fb->width == 0 || fb->height == 0) {
		dprintf(CRITICAL, "Invalid parameters for continuous splash\n");
		return -1;
	}

	// Validate memory region
	size = stride * fb->height;
	if (check_aboot_addr_range_overlap((uintptr_t) fb->base, size)
			|| check_ddr_addr_range_bound((uintptr_t) fb->base, size)) {
		dprintf(CRITICAL, "Invalid memory region for continuous splash"
			" (overlap or out of bounds)\n");
		return -1;
	}

	if ((addr_t)fb->base != MIPI_FB_ADDR)
		mmu_map_fb((addr_t)fb->base, size);

	return 0;
}

void target_display_init(const char *panel_name)
{
	/*
	 * Reading MDP registers will fail if necessary clocks/power domains
	 * are not enabled. Check if the MDP power domain (GDSC) is enabled
	 * to try to avoid crashing if the clocks are disabled.
	 */
	uint32_t val = readl(MDP_GDSCR);

	if (!(val & GDSC_POWER_ON_BIT)) {
		dprintf(CRITICAL, "No continuous splash: MDP GDSC is not enabled\n");
		return;
	}

	if (mdp5_read_config(&fb))
		return;

	// Setup framebuffer
	fbcon_setup(&fb);
}
