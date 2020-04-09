#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <string.h>
#include <mdp5.h>
#include <target.h>
#include <dev/fbcon.h>
#include <platform.h>
#include <platform/iomap.h>
#include <platform/timer.h>

/* Assume ctl/pipe 0 is used */
#define MDP_CTL_BASE			MDP_CTL_0_BASE
#define MDP_PIPE_BASE			MDP_VP_0_RGB_0_BASE

#define MDP_CTL_TOP_MODE_SEL_CMD	BIT(17)

static struct fbcon_config fb = {
	.bpp    = 24,
	.format = FB_FORMAT_RGB888,
};

static struct msm_panel_info dummy_panel = {
	.pipe_type = MDSS_MDP_PIPE_TYPE_RGB,
};

extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
extern int check_ddr_addr_range_bound(uintptr_t start, uint32_t size);

static void mdp5_cmd_mode_flush(void)
{
	mdp_dma_on(&dummy_panel);
	dsb();
	mdelay(20);
}

static int mdp5_read_config(struct fbcon_config *fb)
{
	uint32_t stride, src_size, out_size, src_xy, out_xy, size;
	bool cmd_mode;

	fb->base = (void*) readl(MDP_PIPE_BASE + PIPE_SSPP_SRC0_ADDR);
	if (!fb->base) {
		dprintf(CRITICAL, "Continuous splash does not appear to be enabled\n");
		return -1;
	}

	stride = readl(MDP_PIPE_BASE + PIPE_SSPP_SRC_YSTRIDE);
	src_size = readl(MDP_PIPE_BASE + PIPE_SSPP_SRC_IMG_SIZE);
	out_size = readl(MDP_PIPE_BASE + PIPE_SSPP_SRC_OUT_SIZE);
	src_xy = readl(MDP_PIPE_BASE + PIPE_SSPP_SRC_XY);
	out_xy = readl(MDP_PIPE_BASE + PIPE_SSPP_OUT_XY);
	cmd_mode = !!(readl(MDP_CTL_BASE + CTL_TOP) & MDP_CTL_TOP_MODE_SEL_CMD);

	dprintf(SPEW, "Continuous splash detected: base: %p, stride: %d, "
		"source: %dx%d (%d,%d), output: %dx%d (%d,%d) (cmd mode: %d)\n",
		fb->base, stride,
		src_size & 0xffff, src_size >> 16, src_xy & 0xffff, src_xy >> 16,
		out_size & 0xffff, out_size >> 16, out_xy & 0xffff, out_xy >> 16,
		cmd_mode
	);

	fb->stride = stride / (fb->bpp/8);
	fb->width = fb->stride;
	fb->height = out_size >> 16;

	if (cmd_mode)
		fb->update_start = mdp5_cmd_mode_flush;

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

	// Add MMU mappings if necessary
	fb->base = (void*) platform_map_fb((addr_t) fb->base, size);
	if (!fb->base) {
		dprintf(CRITICAL, "Failed to map continuous splash memory region\n");
		return -1;
	}

	return 0;
}

void target_display_init(const char *panel_name)
{
	dprintf(SPEW, "Panel: %s\n", panel_name);

	if (mdp5_read_config(&fb))
		return;

	// Setup framebuffer
	fbcon_setup(&fb);
}
