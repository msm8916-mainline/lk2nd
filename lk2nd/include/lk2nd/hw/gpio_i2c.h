/* vim: set expandtab ts=4 sw=4 tw=100: */
/*
 * Copyright (c) 2013 Google Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __GPIO_I2C__
#define __GPIO_I2C__

#include <stdint.h>

#include <lk2nd/hw/gpio.h>

typedef struct gpio_i2c_info {
    struct gpiol_desc sda;
    struct gpiol_desc scl;
    uint32_t  hcd;  /* 1/2 I2C clock delay in microseconds */
    uint32_t  qcd;  /* 1/4 I2C clock delay in microseconds */
} gpio_i2c_info_t;

status_t gpio_i2c_transmit(const gpio_i2c_info_t*, uint8_t, const void*, size_t);
status_t gpio_i2c_receive(const gpio_i2c_info_t*, uint8_t, void*, size_t);
status_t gpio_i2c_write_reg_bytes(const gpio_i2c_info_t*, uint8_t, uint8_t, const uint8_t*, size_t);
status_t gpio_i2c_read_reg_bytes(const gpio_i2c_info_t*, uint8_t, uint8_t, uint8_t*, size_t);

/**
 * gpio_i2c_get() - Get GPIO I2C bus from the DT definition.
 * @dtb:   Pointer to the DT.
 * @node:  Offset of the node containing the i2c-sda/scl-gpios properties.
 * @i:     Pointer to the I2C info that will be filled.
 * @addr:  Optional pointer that will be filled with the I2C address (i2c-reg).
 *
 * Returns: Status code (0 on success)
 */
status_t gpio_i2c_get(const void *dtb, int node, gpio_i2c_info_t *i, uint8_t *addr);

#endif  // __GPIO_I2C__
