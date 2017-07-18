/* Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <dev/fbcon.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <string.h>
#include <display_menu.h>
#include <qtimer.h>
#include <pm8x41.h>
#include <platform.h>
#include "mdtp.h"
#include "mdtp_defs.h"
#include "mdtp_fs.h"

// Image releative locations
#define ERROR_MESSAGE_RELATIVE_Y_LOCATION   (0.18)
#define MAIN_TEXT_RELATIVE_Y_LOCATION       (0.33)
#define PIN_RELATIVE_Y_LOCATION             (0.47)
#define PIN_TEXT_RELATIVE_Y_LOCATION        (0.57)
#define OK_BUTTON_RELATIVE_Y_LOCATION       (0.75)
#define OK_TEXT_RELATIVE_Y_LOCATION         (0.82)

#define MDTP_PRESSING_DELAY_MSEC            (400)
#define MDTP_MAX_IMAGE_SIZE                 (1183000)  //size in bytes, includes some extra bytes since we round up to block size in read
#define RGB888_BLACK                        (0x000000)
#define BITS_PER_BYTE                       (8)
#define POWER_KEY_LONG_PRESS                (4000) // 4 seconds in milliseconds


#define CENTER_IMAGE_ON_X_AXIS(image_width,screen_width)         (((screen_width)-(image_width))/2)

extern uint32_t target_volume_up();
extern uint32_t target_volume_down();
extern int msm_display_on();

struct mdtp_fbimage {
	uint32_t width;
	uint32_t height;
	uint8_t image[MDTP_MAX_IMAGE_SIZE];
};

/*----------------------------------------------------------------------------
 * Global Variables
 * -------------------------------------------------------------------------*/

static uint32_t g_pin_frames_x_location[MDTP_PIN_LEN] = {0};
static uint32_t g_pin_frames_y_location = 0;
static bool g_diplay_pin = false;

static bool g_initial_screen_displayed = false;

static struct mdtp_fbimage *g_mdtp_header = NULL;
static struct fbcon_config *fb_config = NULL;


/*----------------------------------------------------------------------------
 * Local Functions
 * -------------------------------------------------------------------------*/

/**
 * Allocate mdtp image
 */
static void alloc_mdtp_image() {
	if (!g_mdtp_header) {
		g_mdtp_header = (struct mdtp_fbimage *)malloc(sizeof(struct mdtp_fbimage));
		ASSERT(g_mdtp_header);
	}
}

/**
 * Free mdtp image
 */
void free_mdtp_image() {
	if (g_mdtp_header) {
		free(g_mdtp_header);
		g_mdtp_header = NULL;
	}
}

/**
 * Load images from EMMC
 */
static struct mdtp_fbimage* mdtp_read_mmc_image(uint32_t offset, uint32_t width, uint32_t height)
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct mdtp_fbimage *logo = g_mdtp_header;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return NULL;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return NULL;
	}

	if (fb_config)
	{
		uint8_t *base = logo->image;
		unsigned bytes_per_bpp = ((fb_config->bpp) / BITS_PER_BYTE);
		unsigned int data_len = ROUNDUP(width*height*bytes_per_bpp, block_size);
		if (data_len > MDTP_MAX_IMAGE_SIZE) {
			dprintf(CRITICAL, "ERROR: incorrect mdtp image size\n");
			return NULL;
		}

		if (mmc_read(ptn+offset, (void*)base, data_len)) {
			fbcon_clear();
			dprintf(CRITICAL, "ERROR: mdtp image read failed\n");
			return NULL;
		}

		logo->width = width;
		logo->height = height;
	}

	return logo;
}

/**
 * flush fbcon display
 *
 * The function is duplicated from fbcon.c
 */
static void fbcon_flush(void)
{
	if (fb_config->update_start)
		fb_config->update_start();
	if (fb_config->update_done)
		while (!fb_config->update_done());
}

/**
 * Clear complete section on the screen.
 * The section is of section_height, and is located from the y
 * coordinate and down.
 */
static void fbcon_clear_section(uint32_t y, uint32_t section_height)
{
	unsigned image_base;
	unsigned bytes_per_bpp;

	if (fb_config)
	{
		image_base = (y *(fb_config->width));
		bytes_per_bpp = ((fb_config->bpp) / BITS_PER_BYTE);

		unsigned count = fb_config->width*section_height;
		memset(fb_config->base + image_base*bytes_per_bpp, RGB888_BLACK, count*bytes_per_bpp);

		fbcon_flush();
	}
	else
	{
		dprintf(CRITICAL,"ERROR: fbcon_config struct is NULL\n");
		display_error_msg(); /* This will never return */
	}
}

/**
 * Put image at a specific (x,y) location on the screen.
 * Duplicated from fbcon.c, with modifications to allow (x,y) location (instead of a centered image),
 * and display bmp images properly (order of copying the lines to the screen was reversed)
 */
static void fbcon_putImage_in_location(struct mdtp_fbimage *fbimg, uint32_t x, uint32_t y)
{
	unsigned i = 0;
	unsigned bytes_per_bpp;
	unsigned image_base;
	unsigned width, pitch, height;
	unsigned char *logo_base = NULL;

	if (!fb_config) {
		dprintf(CRITICAL,"ERROR: NULL configuration, image cannot be displayed\n");
		return;
	}

	if(fbimg) {
		width = pitch = fbimg->width;
		height = fbimg->height;
		logo_base = (unsigned char *)fbimg->image;
	}
	else {
		dprintf(CRITICAL,"ERROR: invalid image struct\n");
		return;
	}

	bytes_per_bpp = ((fb_config->bpp) / BITS_PER_BYTE);

#if DISPLAY_TYPE_MIPI
	if (bytes_per_bpp == 3)
	{
		if (fbimg->width == fb_config->width && fbimg->height == fb_config->height)
		{
			dprintf(CRITICAL,"ERROR: full screen image, cannot be displayed\n");
			return;
		}

		if (fbimg->width > fb_config->width || fbimg->height > fb_config->height ||
				(x > (fb_config->width - fbimg->width)) || (y > (fb_config->height - fbimg->height)))
		{
			dprintf(CRITICAL,"ERROR: invalid image size, larger than the screen or exceeds its margins\n");
			return;
		}

		image_base = ( (y *(fb_config->width)) + x);
		for (i = 0; i < height; i++) {
			memcpy (fb_config->base + ((image_base + (i * (fb_config->width))) * bytes_per_bpp),
					logo_base + ((height - 1 - i) * pitch * bytes_per_bpp), width * bytes_per_bpp);
		}
	}
	else
	{
		dprintf(CRITICAL,"ERROR: invalid bpp value\n");
		display_error_msg(); /* This will never return */
	}

	/* Flush the contents to memory before giving the data to dma */
	arch_clean_invalidate_cache_range((addr_t) fb_config->base, (fb_config->height * fb_config->width * bytes_per_bpp));

	fbcon_flush();
	msm_display_on();

#endif
}

/**
 * Display main error message
 */
static int display_error_message()
{
	struct mdtp_fbimage *fbimg;

	if (fb_config)
	{
		uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(ALERT_MESSAGE),fb_config->width);
		uint32_t y = ((fb_config->height)*ERROR_MESSAGE_RELATIVE_Y_LOCATION);
		fbimg = mdtp_read_mmc_image(get_image_offset(ALERT_MESSAGE),get_image_width(ALERT_MESSAGE), get_image_height(ALERT_MESSAGE));
		if (NULL == fbimg)
		{
			dprintf(CRITICAL,"ERROR: failed to read error image from mmc\n");
			return -1;
		}

		fbcon_putImage_in_location(fbimg, x, y);

		return 0;
	}
	else
	{
		dprintf(CRITICAL,"ERROR: fbcon_config struct is NULL\n");
		return -1;
	}
}

/**
 * Read from mmc the image in the given offset, of the given width and height.
 * Then, display the image on the screen in the given (x,y) location.
 */
static void display_image(uint32_t offset, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
{
	struct mdtp_fbimage *fbimg;

	if (fb_config)
	{
		fbimg = mdtp_read_mmc_image(offset, width, height);
		if (NULL == fbimg)
		{
			dprintf(CRITICAL,"ERROR: failed to read image from mmc\n");
			display_error_msg(); /* This will never return */
		}

		fbcon_putImage_in_location(fbimg, x, y);
	}
	else
	{
		dprintf(CRITICAL,"ERROR: fbcon_config struct is NULL\n");
		display_error_msg(); /* This will never return */
	}
}

/**
 * Display initial delay message
 */
static void display_initial_delay()
{
	uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(MAINTEXT_5SECONDS),fb_config->width);
	uint32_t y = (fb_config->height)*MAIN_TEXT_RELATIVE_Y_LOCATION;
	display_image(get_image_offset(MAINTEXT_5SECONDS), get_image_width(MAINTEXT_5SECONDS), get_image_height(MAINTEXT_5SECONDS), x, y);
}

/**
 * Display "enter PIN" message
 */
static void display_enter_pin()
{
	uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(MAINTEXT_ENTERPIN),fb_config->width);
	uint32_t y = (fb_config->height)*MAIN_TEXT_RELATIVE_Y_LOCATION;
	display_image(get_image_offset(MAINTEXT_ENTERPIN), get_image_width(MAINTEXT_ENTERPIN), get_image_height(MAINTEXT_ENTERPIN), x, y);
}

/**
 * Display invalid PIN message
 */
static void display_invalid_pin()
{
	uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(MAINTEXT_INCORRECTPIN),fb_config->width);
	uint32_t y = (fb_config->height)*MAIN_TEXT_RELATIVE_Y_LOCATION;

	display_image(get_image_offset(MAINTEXT_INCORRECTPIN), get_image_width(MAINTEXT_INCORRECTPIN), get_image_height(MAINTEXT_INCORRECTPIN), x, y);
}

/**
 * Clear digits instructions
 */
static void display_digits_instructions()
{
	uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(PINTEXT),fb_config->width);
	uint32_t y = (fb_config->height)*PIN_TEXT_RELATIVE_Y_LOCATION;

	display_image(get_image_offset(PINTEXT), get_image_width(PINTEXT), get_image_height(PINTEXT), x, y);
}

/**
 * Clear digits instructions
 */
static void clear_digits_instructions()
{
	uint32_t y = (fb_config->height)*PIN_TEXT_RELATIVE_Y_LOCATION;
	fbcon_clear_section(y, get_image_height(PINTEXT));
}

/**
 * Display a digit as un-selected.
 */
static void display_digit(uint32_t x, uint32_t y, uint32_t digit)
{
	if (g_diplay_pin == false)
		return;

	display_image(get_image_offset(PIN_UNSELECTED_0 + digit),
			get_image_width(PIN_UNSELECTED_0 + digit), get_image_height(PIN_UNSELECTED_0 + digit), x, y);
}

/**
 * Display a digit as selected.
 */
static void display_selected_digit(uint32_t x, uint32_t y, uint32_t digit)
{
	if (g_diplay_pin == false)
		return;

	display_image(get_image_offset(PIN_SELECTED_0 + digit),
			get_image_width(PIN_SELECTED_0 + digit),
			get_image_height(PIN_SELECTED_0 + digit), x, y);
}

/**
 * Display OK button as un-selected.
 */
static void display_ok_button()
{
	uint32_t ok_x = CENTER_IMAGE_ON_X_AXIS(get_image_width(BTN_OK_OFF),fb_config->width);
	uint32_t ok_y = (fb_config->height)*OK_BUTTON_RELATIVE_Y_LOCATION;

	display_image(get_image_offset(BTN_OK_OFF), get_image_width(BTN_OK_OFF),get_image_height(BTN_OK_OFF), ok_x, ok_y);
}

/**
 * Display OK button as selected.
 */
static void display_selected_ok_button()
{
	uint32_t ok_x = CENTER_IMAGE_ON_X_AXIS(get_image_width(BTN_OK_ON),fb_config->width);
	uint32_t ok_y = (fb_config->height)*OK_BUTTON_RELATIVE_Y_LOCATION;

	display_image(get_image_offset(BTN_OK_ON), get_image_width(BTN_OK_ON), get_image_height(BTN_OK_ON),  ok_x, ok_y);
}


/**
 * Display the instructions for the OK button.
 */
static void display_pin_instructions()
{
	uint32_t x = CENTER_IMAGE_ON_X_AXIS(get_image_width(ACCEPTEDIT_TEXT),fb_config->width);
	uint32_t y = (fb_config->height)*OK_TEXT_RELATIVE_Y_LOCATION;

	display_image(get_image_offset(ACCEPTEDIT_TEXT), get_image_width(ACCEPTEDIT_TEXT), get_image_height(ACCEPTEDIT_TEXT), x, y);
}

/**
 * Clear the instructions for the OK button.
 */
static void clear_pin_message()
{
	uint32_t y = (fb_config->height)*OK_TEXT_RELATIVE_Y_LOCATION;

	fbcon_clear_section(y,get_image_height(ACCEPTEDIT_TEXT));
}

/**
 * Initialize data structures required for MDTP UI.
 */
static void init_mdtp_ui_data()
{
	fb_config = fbcon_display();
	alloc_mdtp_image();
}

/**
 * Display the basic layout of the screen (done only once).
 */
static void display_initial_screen(uint32_t pin_length)
{
	if (g_initial_screen_displayed == true)
		return;

	init_mdtp_ui_data();

	if (fb_config)
	{
		fbcon_clear();

		if (display_error_message())
			display_error_msg(); /* This will never return */
		display_initial_delay();

		mdelay(INITIAL_DELAY_MSECONDS);

		g_pin_frames_y_location = ((fb_config->height)*PIN_RELATIVE_Y_LOCATION);

		uint32_t total_pin_length = pin_length*get_image_width(PIN_UNSELECTED_0) + mdtp_fs_get_param(DIGIT_SPACE)*(pin_length - 1);

		if (fb_config->width > total_pin_length)
		{
			g_diplay_pin = true;

			uint32_t complete_pin_centered = (fb_config->width - total_pin_length)/2;

			for (uint32_t i=0; i<pin_length; i++)
			{
				g_pin_frames_x_location[i] = complete_pin_centered + i*(mdtp_fs_get_param(DIGIT_SPACE) + get_image_width(PIN_UNSELECTED_0));
			}

			for (uint32_t i=0; i<pin_length; i++)
			{
				display_digit(g_pin_frames_x_location[i], g_pin_frames_y_location, 0);
			}
		}
		else
		{
			dprintf(CRITICAL,"ERROR: screen is not wide enough, PIN digits can't be displayed\n");
		}

		display_ok_button();

		g_initial_screen_displayed = true;
	}
	else
	{
		dprintf(CRITICAL,"ERROR: fbcon_config struct is NULL\n");
		display_error_msg(); /* This will never return */
	}
}

/**
 * Display the recovery PIN screen and set received buffer
 * with the PIN the user has entered.
 * The entered PIN will be validated by the calling function.
 */
static void display_get_pin_interface(char *entered_pin, uint32_t pin_length)
{
	uint32_t previous_position = 0, current_position = 0;
	time_t pwrkey_press_timer = 0;

	display_initial_screen(pin_length);
	display_enter_pin();

	// Convert ascii to digits
	for (uint32_t i=0; i<pin_length; i++)
	{
		entered_pin[i] -= '0';
	}
	display_selected_digit(g_pin_frames_x_location[0], g_pin_frames_y_location, entered_pin[0]);
	display_digits_instructions();

	while (1)
	{
		// Check if the power key is being pressed
		if(pm8x41_get_pwrkey_is_pressed())
		{
			// Check if it is the first power key press event
			if (pwrkey_press_timer == 0)
			{
				// Init reference time to first power key press event detected
				pwrkey_press_timer = current_time();
			}
			// If the power key is being pressed long enough, shutdown the device
			if ((current_time() - pwrkey_press_timer) >= POWER_KEY_LONG_PRESS){
				dprintf(INFO, "Power key pressed - shutting down\n");
				shutdown_device();
			}
		}
		else
		{
			// Power key isn't being pressed, reset power key press reference time
			pwrkey_press_timer = 0;
		}

		if (target_volume_up())
		{
			// current position is the OK button
			if (current_position == pin_length)
			{
				// Convert digits to ascii and
				// validate entered PIN in the calling function
				for (uint32_t i=0; i<pin_length; i++)
				{
					entered_pin[i] += '0';
				}
				return;
			}

			// current position is a PIN slot
			entered_pin[current_position] = (entered_pin[current_position]+1) % 10;
			display_selected_digit(g_pin_frames_x_location[current_position], g_pin_frames_y_location, entered_pin[current_position]);
			mdelay(MDTP_PRESSING_DELAY_MSEC);
		}
		if (target_volume_down())
		{
			previous_position = current_position;
			current_position = (current_position+1) % (pin_length+1);

			// previous position was the OK button
			if (previous_position == pin_length)
			{
				clear_pin_message();
				display_ok_button();

				display_digits_instructions();
				display_selected_digit(g_pin_frames_x_location[current_position], g_pin_frames_y_location, entered_pin[current_position]);

			}

			// current position is the OK button
			else if (current_position == pin_length)
			{
				display_digit(g_pin_frames_x_location[previous_position], g_pin_frames_y_location, entered_pin[previous_position]);
				clear_digits_instructions();

				display_selected_ok_button();
				display_pin_instructions();
			}

			// both the previous and the current positions are PIN slots
			else
			{
				display_digit(g_pin_frames_x_location[previous_position], g_pin_frames_y_location, entered_pin[previous_position]);

				display_selected_digit(g_pin_frames_x_location[current_position], g_pin_frames_y_location, entered_pin[current_position]);
			}

			mdelay(MDTP_PRESSING_DELAY_MSEC);
		}
	}
}

/*----------------------------------------------------------------------------
 * External Functions
 * -------------------------------------------------------------------------*/

/**
 * Display the recovery PIN screen and set received buffer
 * with the PIN the user has entered.
 */
void get_pin_from_user(char *entered_pin, uint32_t pin_length)
{
	display_get_pin_interface(entered_pin, pin_length);

	return;
}

/**
 * User has entered invalid PIN, display error message and
 * allow the user to try again.
 */
void display_invalid_pin_msg()
{
	clear_pin_message();
	display_ok_button();

	display_invalid_pin();

	mdelay(INVALID_PIN_DELAY_MSECONDS);
}

/**
 *  Display error message and stop boot process.
 */
void display_error_msg()
{
	init_mdtp_ui_data();

	if (fb_config)
	{
		fbcon_clear();
		display_error_message();   // No point in checking the return value here
	}

	// Invalid state. Nothing to be done but contacting the OEM.
	// Stop boot process.
	dprintf(CRITICAL,"ERROR: blocking boot process\n");
	for(;;);
}

/**
 *  Display error message in case mdtp image is corrupted and stop boot process.
 */
void display_error_msg_mdtp()
{
	int big_factor = 8; // Font size
	char* str = "Device unable to boot";
	char* str2 = "\nError - mdtp image is corrupted\n";
	fbcon_clear();
	while(*str != 0) {
		fbcon_putc_factor(*str++, FBCON_COMMON_MSG, big_factor);
	}
	fbcon_draw_line(FBCON_COMMON_MSG);
	while(*str2 != 0) {
		fbcon_putc_factor(*str2++, FBCON_COMMON_MSG, big_factor);
	}
	fbcon_draw_line(FBCON_COMMON_MSG);

	// Invalid state. Nothing to be done but contacting the OEM.
	// Stop boot process.
	dprintf(CRITICAL,"ERROR: blocking boot process - mdtp image corrupted\n");
	for(;;);

}

