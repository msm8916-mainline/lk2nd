#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <string.h>
#include <target.h>

#include <dev/fbcon.h>
#include <kernel/event.h>
#include <kernel/thread.h>
#include <mdp3.h>
#include <platform.h>
#include <platform/clock.h>
#include <platform/iomap.h>
#include <platform/timer.h>


static struct fbcon_config fb = {
	.bpp    = 24,
	.format = FB_FORMAT_RGB888,
};

static void mdss_mdp_cmd_kickoff(void) {
	writel(0x00000001, MDP_DMA_P_START);
	mdelay(10);
}

int mdp3_read_config(struct fbcon_config *fb)
{
	fb->base = (void *) readl(MDP_DMA_P_BUF_ADDR);
	fb->width = readl(MDP_DMA_P_BUF_Y_STRIDE)/3;
	fb->height = readl(MDP_DMA_P_SIZE)>>16;
	fb->stride = fb->width;

	fb->update_start = mdss_mdp_cmd_kickoff;
}

void target_display_init(const char *panel_name)
{
	if (mdp3_read_config(&fb))
		return;
	
	// Setup framebuffer
	fbcon_setup(&fb);
}
