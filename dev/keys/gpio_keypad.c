/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <bits.h>
#include <stdlib.h>
#include <string.h>
#include <dev/keys.h>
#include <dev/gpio.h>
#include <dev/gpio_keypad.h>
#include <dev/ssbi.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <platform.h>

#define LINUX_MACHTYPE_8660_QT      3298

struct gpio_kp {
	struct gpio_keypad_info *keypad_info;
	struct timer timer;
	event_t full_scan;
	int current_output;
	unsigned int some_keys_pressed:2;
	unsigned long keys_pressed[0];
};

struct gpio_qwerty_kp {
	struct qwerty_keypad_info *keypad_info;
	struct timer timer;
	event_t full_scan;
	unsigned int some_keys_pressed:2;
	unsigned long keys_pressed[0];
};

static struct gpio_qwerty_kp *qwerty_keypad;
/* TODO: Support multiple keypads? */
static struct gpio_kp *keypad;

static void check_output(struct gpio_kp *kp, int out, int polarity)
{
	struct gpio_keypad_info *kpinfo = kp->keypad_info;
	int key_index;
	int in;
	int gpio;
	int changed = 0;

	key_index = out * kpinfo->ninputs;
	for (in = 0; in < kpinfo->ninputs; in++, key_index++) {
		gpio = kpinfo->input_gpios[in];
		changed = 0;
		if (gpio_get(gpio) ^ !polarity) {
			if (kp->some_keys_pressed < 3)
				kp->some_keys_pressed++;
			changed = !bitmap_set(kp->keys_pressed, key_index);
		} else {
			changed = bitmap_clear(kp->keys_pressed, key_index);
		}
		if (changed) {
			int state = bitmap_test(kp->keys_pressed, key_index);
			keys_post_event(kpinfo->keymap[key_index], state);
		}
	}

	/* sets up the right state for the next poll cycle */
	gpio = kpinfo->output_gpios[out];
	if (kpinfo->flags & GPIOKPF_DRIVE_INACTIVE)
		gpio_set(gpio, !polarity);
	else
		gpio_config(gpio, GPIO_INPUT);
}

static enum handler_return
gpio_keypad_timer_func(struct timer *timer, time_t now, void *arg)
{
	struct gpio_kp *kp = keypad;
	struct gpio_keypad_info *kpinfo = kp->keypad_info;
	int polarity = !!(kpinfo->flags & GPIOKPF_ACTIVE_HIGH);
	int out;
	int gpio;

	out = kp->current_output;
	if (out == kpinfo->noutputs) {
		out = 0;
		kp->some_keys_pressed = 0;
	} else {
		check_output(kp, out, polarity);
		out++;
	}

	kp->current_output = out;
	if (out < kpinfo->noutputs) {
		gpio = kpinfo->output_gpios[out];
		if (kpinfo->flags & GPIOKPF_DRIVE_INACTIVE)
			gpio_set(gpio, polarity);
		else
			gpio_config(gpio, polarity ? GPIO_OUTPUT : 0);
		timer_set_oneshot(timer, kpinfo->settle_time,
				  gpio_keypad_timer_func, NULL);
		goto done;
	}

	if (/*!kp->use_irq*/ 1 || kp->some_keys_pressed) {
		event_signal(&kp->full_scan, false);
		timer_set_oneshot(timer, kpinfo->poll_time,
				  gpio_keypad_timer_func, NULL);
		goto done;
	}

done:
	return INT_RESCHEDULE;
}

void gpio_keypad_init(struct gpio_keypad_info *kpinfo)
{
	int key_count;
	int output_val;
	int output_cfg;
	int i;
	int len;

	ASSERT(kpinfo->keymap && kpinfo->input_gpios && kpinfo->output_gpios);
	key_count = kpinfo->ninputs * kpinfo->noutputs;

	len = sizeof(struct gpio_kp) + (sizeof(unsigned long) *
					BITMAP_NUM_WORDS(key_count));
	keypad = malloc(len);
	ASSERT(keypad);

	memset(keypad, 0, len);
	keypad->keypad_info = kpinfo;

	output_val = (!!(kpinfo->flags & GPIOKPF_ACTIVE_HIGH)) ^
		     (!!(kpinfo->flags & GPIOKPF_DRIVE_INACTIVE));
	output_cfg = kpinfo->flags & GPIOKPF_DRIVE_INACTIVE ? GPIO_OUTPUT : 0;
	for (i = 0; i < kpinfo->noutputs; i++) {
		gpio_set(kpinfo->output_gpios[i], output_val);
		gpio_config(kpinfo->output_gpios[i], output_cfg);
	}
	for (i = 0; i < kpinfo->ninputs; i++)
		gpio_config(kpinfo->input_gpios[i], GPIO_INPUT);

	keypad->current_output = kpinfo->noutputs;

	event_init(&keypad->full_scan, false, EVENT_FLAG_AUTOUNSIGNAL);
	timer_initialize(&keypad->timer);
	timer_set_oneshot(&keypad->timer, 0, gpio_keypad_timer_func, NULL);

	/* wait for the keypad to complete one full scan */
	event_wait(&keypad->full_scan);
}

int pm8058_gpio_config(int gpio, struct pm8058_gpio *param)
{
	int	rc;
        write_func wr_function = (qwerty_keypad->keypad_info)->wr_func;
	unsigned char bank[8];
	static int dir_map[] = {
		PM8058_GPIO_MODE_OFF,
		PM8058_GPIO_MODE_OUTPUT,
		PM8058_GPIO_MODE_INPUT,
		PM8058_GPIO_MODE_BOTH,
	};

	if (param == 0) {
	  dprintf (INFO, "pm8058_gpio struct not defined\n");
          return -1;
	}

	/* Select banks and configure the gpio */
	bank[0] = PM8058_GPIO_WRITE |
		((param->vin_sel << PM8058_GPIO_VIN_SHIFT) &
			PM8058_GPIO_VIN_MASK) |
		PM8058_GPIO_MODE_ENABLE;
	bank[1] = PM8058_GPIO_WRITE |
		((1 << PM8058_GPIO_BANK_SHIFT) & PM8058_GPIO_BANK_MASK) |
		((dir_map[param->direction] << PM8058_GPIO_MODE_SHIFT) &
			PM8058_GPIO_MODE_MASK) |
		((param->direction & PM_GPIO_DIR_OUT) ?
			PM8058_GPIO_OUT_BUFFER : 0);
	bank[2] = PM8058_GPIO_WRITE |
		((2 << PM8058_GPIO_BANK_SHIFT) & PM8058_GPIO_BANK_MASK) |
		((param->pull << PM8058_GPIO_PULL_SHIFT) &
			PM8058_GPIO_PULL_MASK);
	bank[3] = PM8058_GPIO_WRITE |
		((3 << PM8058_GPIO_BANK_SHIFT) & PM8058_GPIO_BANK_MASK) |
		((param->out_strength << PM8058_GPIO_OUT_STRENGTH_SHIFT) &
			PM8058_GPIO_OUT_STRENGTH_MASK);
	bank[4] = PM8058_GPIO_WRITE |
		((4 << PM8058_GPIO_BANK_SHIFT) & PM8058_GPIO_BANK_MASK) |
		((param->function << PM8058_GPIO_FUNC_SHIFT) &
			PM8058_GPIO_FUNC_MASK);

	rc = (*wr_function)(bank, 5, SSBI_REG_ADDR_GPIO(gpio));
	if (rc) {
	        dprintf(INFO, "Failed on 1st ssbi_write(): rc=%d.\n", rc);
		return 1;
	}
	return 0;
}

int pm8058_gpio_config_kypd_drv(int gpio_start, int num_gpios, unsigned mach_id)
{
	int	rc;
	struct pm8058_gpio kypd_drv = {
		.direction	= PM_GPIO_DIR_OUT,
		.pull		= PM_GPIO_PULL_NO,
		.vin_sel	= 2,
		.out_strength	= PM_GPIO_STRENGTH_LOW,
		.function	= PM_GPIO_FUNC_1,
		.inv_int_pol	= 1,
	};

	if(mach_id == LINUX_MACHTYPE_8660_QT)
		kypd_drv.function = PM_GPIO_FUNC_NORMAL;

	while (num_gpios--) {
		rc = pm8058_gpio_config(gpio_start++, &kypd_drv);
		if (rc) {
		        dprintf(INFO, "FAIL pm8058_gpio_config(): rc=%d.\n", rc);
			return rc;
		}
	}

	return 0;
}

int pm8058_gpio_config_kypd_sns(int gpio_start, int num_gpios)
{
	int	rc;
	struct pm8058_gpio kypd_sns = {
		.direction	= PM_GPIO_DIR_IN,
		.pull		= PM_GPIO_PULL_UP1,
		.vin_sel	= 2,
		.out_strength	= PM_GPIO_STRENGTH_NO,
		.function	= PM_GPIO_FUNC_NORMAL,
		.inv_int_pol	= 1,
	};

	while (num_gpios--) {
		rc = pm8058_gpio_config(gpio_start++, &kypd_sns);
		if (rc) {
		        dprintf(INFO, "FAIL pm8058_gpio_config(): rc=%d.\n", rc);
			return rc;
		}
	}

	return 0;
}

static void ssbi_gpio_init(unsigned int mach_id)
{
    unsigned char kypd_cntl_init;
    unsigned char kypd_scan_init = 0x20;
    int rows = (qwerty_keypad->keypad_info)->rows;
    int columns = (qwerty_keypad->keypad_info)->columns;
    write_func wr_function = (qwerty_keypad->keypad_info)->wr_func;
    unsigned char drv_bits[] = {
	    0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 6, 7, 7, 7 };
    unsigned char sns_bits[] = { 0, 0, 0, 0, 0, 0, 1, 2, 3 };

    kypd_cntl_init = ((drv_bits[rows] << 2) | (sns_bits[columns] << 5) | (1 << 7));

    if ((*wr_function)(&kypd_cntl_init, 1, SSBI_REG_KYPD_CNTL_ADDR))
      dprintf (CRITICAL, "Error in initializing SSBI_REG_KYPD_CNTL register\n");

    if ((*wr_function)(&kypd_scan_init, 1, SSBI_REG_KYPD_SCAN_ADDR))
      dprintf (CRITICAL, "Error in initializing SSBI_REG_KYPD_SCAN register\n");

    if(mach_id == LINUX_MACHTYPE_8660_QT)
    {
        pm8058_gpio_config_kypd_sns(QT_PMIC_GPIO_KYPD_SNS, rows);
        pm8058_gpio_config_kypd_drv(QT_PMIC_GPIO_KYPD_DRV,  columns, mach_id);
    }
    else
    {
        pm8058_gpio_config_kypd_sns(SSBI_OFFSET_ADDR_GPIO_KYPD_SNS, columns);
        pm8058_gpio_config_kypd_drv(SSBI_OFFSET_ADDR_GPIO_KYPD_DRV,  rows, mach_id);
    }
}

static enum handler_return
scan_qwerty_keypad(struct timer *timer, time_t now, void *arg)
{
    unsigned int rows = (qwerty_keypad->keypad_info)->rows;
    unsigned int columns = (qwerty_keypad->keypad_info)->columns;
    unsigned int num_of_ssbi_reads = (qwerty_keypad->keypad_info)->num_of_reads;
    read_func rd_function = (qwerty_keypad->keypad_info)->rd_func;
    unsigned char column_new_keys = 0x00;
    unsigned char column_old_keys = 0x00;
    int shift = 0;
    static int key_detected = -1;

    if ((*rd_function)((qwerty_keypad->keypad_info)->rec_keys, num_of_ssbi_reads,
                                                 SSBI_REG_KYPD_REC_DATA_ADDR))
      dprintf (CRITICAL, "Error in initializing SSBI_REG_KYPD_CNTL register\n");

    if ((*rd_function)((qwerty_keypad->keypad_info)->old_keys, num_of_ssbi_reads,
                                                 SSBI_REG_KYPD_OLD_DATA_ADDR))
      dprintf (CRITICAL, "Error in initializing SSBI_REG_KYPD_CNTL register\n");

    while (rows--) {
         columns = qwerty_keypad->keypad_info->columns;
         if (((qwerty_keypad->keypad_info)->rec_keys[rows]
	      != (qwerty_keypad->keypad_info)->old_keys[rows])
	      && ((qwerty_keypad->keypad_info)->rec_keys[rows] != 0x00)
	     && ((qwerty_keypad->keypad_info)->old_keys[rows] != 0x00)) {
	    while (columns--) {
	        column_new_keys = ((qwerty_keypad->keypad_info)->rec_keys[rows]);
	        column_old_keys = ((qwerty_keypad->keypad_info)->old_keys[rows]);
	        if (((0x01 << columns) & (~column_new_keys))
		    && !((0x01 << columns) & (~column_old_keys))) {
	            shift = (rows * 8) + columns;
	            if ((qwerty_keypad->keypad_info)->keymap[shift]) {
		      if (shift != key_detected) {
			    key_detected = shift;
			    keys_post_event((qwerty_keypad->keypad_info)->keymap[shift], 1);
		        }
	            }
		}
	    }
	}
    }

    event_signal(&qwerty_keypad->full_scan, false);
    return INT_RESCHEDULE;
}

static enum handler_return
scan_qt_keypad(struct timer *timer, time_t now, void *arg)
{
    unsigned int gpio;
    unsigned int last_state=0;
    unsigned int new_state=0;
    unsigned int bits_changed;
    static int key_detected=-1;

    /* Row GPIOs 8,9,10 are used for sensing here */
    for (gpio=8;gpio<=10;gpio++)
    {
        bool status;
        status = pm8058_gpio_get(gpio);
        if (status == 0)
            new_state |= (1<<(gpio-8));
    }

    bits_changed = last_state ^ new_state;

    if (bits_changed)
    {
        int shift = 0;
        for (unsigned int rows = 0; rows < (qwerty_keypad->keypad_info)->rows; rows++)
        {
            if ((bits_changed & (1<<rows)) == 0)
                continue;
            shift = rows * 8 + 3;
        }
        if ((qwerty_keypad->keypad_info)->keymap[shift])
        {
            if (shift != key_detected)
            {
                key_detected = shift;
                keys_post_event((qwerty_keypad->keypad_info)->keymap[shift], 1);
                timer_set_oneshot(timer,(qwerty_keypad->keypad_info)->poll_time,
                                  scan_qt_keypad, NULL);
                return INT_RESCHEDULE;
            }
        }
    }
    event_signal(&qwerty_keypad->full_scan, false);
    return INT_RESCHEDULE;
}

void ssbi_keypad_init(struct qwerty_keypad_info  *qwerty_kp)
{
    unsigned int mach_id;
    int len;

    len = sizeof(struct gpio_qwerty_kp);
    qwerty_keypad = malloc(len);
    ASSERT(qwerty_keypad);

    memset(qwerty_keypad, 0, len);
    qwerty_keypad->keypad_info = qwerty_kp;

    event_init(&qwerty_keypad->full_scan, false, EVENT_FLAG_AUTOUNSIGNAL);
    timer_initialize(&qwerty_keypad->timer);

    mach_id = board_machtype();
    ssbi_gpio_init(mach_id);

    if(mach_id == LINUX_MACHTYPE_8660_QT)
    {
        mdelay((qwerty_keypad->keypad_info)->settle_time);
#ifdef QT_8660_KEYPAD_HW_BUG
        timer_set_oneshot(&qwerty_keypad->timer, 0, scan_qt_keypad, NULL);
#endif
    }
    else
        timer_set_oneshot(&qwerty_keypad->timer, 0, scan_qwerty_keypad, NULL);

    /* wait for the keypad to complete one full scan */
    event_wait(&qwerty_keypad->full_scan);
}

static enum handler_return
scan_qwerty_gpio_keypad(struct timer *timer, time_t now, void *arg)
{
	int i=0;
	int num =0;
	uint8_t key_status;
	struct qwerty_keypad_info *keypad = qwerty_keypad->keypad_info;

	num = keypad->mapsize;

	for(i=0; i < num; i++)
	{
		/*continue if not interested in key*/
		if(!keypad->keymap[i])
			continue;

		/*read key gpio*/
		keypad->key_gpio_get(keypad->gpiomap[i], &key_status);

		/*Post event if key pressed*/
		if(key_status)
		{
			keys_post_event(keypad->keymap[i], 1);
			goto done;
		}
	}

done:
	event_signal(&qwerty_keypad->full_scan, false);
	return INT_RESCHEDULE;
}

void ssbi_gpio_keypad_init(struct qwerty_keypad_info  *qwerty_kp)
{
	int len;

	len = sizeof(struct gpio_qwerty_kp);
	qwerty_keypad = malloc(len);
	ASSERT(qwerty_keypad);

	memset(qwerty_keypad, 0, len);
	qwerty_keypad->keypad_info = qwerty_kp;

	event_init(&qwerty_keypad->full_scan, false, EVENT_FLAG_AUTOUNSIGNAL);
	timer_initialize(&qwerty_keypad->timer);

	timer_set_oneshot(&qwerty_keypad->timer, 0, scan_qwerty_gpio_keypad, NULL);

	/* wait for the keypad to complete one full scan */
	event_wait(&qwerty_keypad->full_scan);
}

void pmic_write(unsigned address, unsigned data)
{
  write_func wr_function = &i2c_ssbi_write_bytes;
  if (wr_function == NULL)
    return;
  if ((*wr_function)((unsigned char *) &data, 1, address))
    dprintf (CRITICAL, "Error in initializing register\n");

}
void toshiba_pmic_gpio_init(unsigned gpio)
{
  pmic_write(gpio,0x85);
  pmic_write(gpio,0x98);
  pmic_write(gpio,0xB8);
  pmic_write(gpio,0xC6);
}
