#include <debug.h>
#include <reg.h>
#include <string.h>
#include <mdp5.h>
#include <platform.h>
#include <dev/fbcon.h>
#include <target/display.h>

#define PIPE_BASE  MDP_VP_0_RGB_0_BASE

static struct fbcon_config fb = {
	.bpp    = 24,
	.format = FB_FORMAT_RGB888,
};

extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
extern int check_ddr_addr_range_bound(uintptr_t start, uint32_t size);

static int mdss_read_config(struct fbcon_config *fb)
{
	uint32_t stride, src_size, out_size, src_xy, out_xy, size;

	fb->base = (void*) readl(PIPE_BASE + PIPE_SSPP_SRC0_ADDR);
	if (!fb->base) {
		dprintf(CRITICAL, "Continuous splash does not appear to be enabled\n");
		return -1;
	}

	stride = readl(PIPE_BASE + PIPE_SSPP_SRC_YSTRIDE);
	src_size = readl(PIPE_BASE + PIPE_SSPP_SRC_IMG_SIZE);
	out_size = readl(PIPE_BASE + PIPE_SSPP_SRC_OUT_SIZE);
	src_xy = readl(PIPE_BASE + PIPE_SSPP_SRC_XY);
	out_xy = readl(PIPE_BASE + PIPE_SSPP_OUT_XY);

	dprintf(SPEW, "Continuous splash detected: base: %p, stride: %d, "
		"source: %dx%d (%d,%d), output: %dx%d (%d,%d)\n", fb->base, stride,
		src_size & 0xffff, src_size >> 16, src_xy & 0xffff, src_xy >> 16,
		out_size & 0xffff, out_size >> 16, out_xy & 0xffff, out_xy >> 16
	);

	fb->stride = stride / (fb->bpp/8);
	fb->width = fb->stride;
	fb->height = out_size >> 16;

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

	return 0;
}

void target_display_init(const char *panel_name)
{
	dprintf(SPEW, "Panel: %s\n", panel_name);

	if (mdss_read_config(&fb))
		return;

	// Setup framebuffer
	fbcon_setup(&fb);
}
