/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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
#ifndef __ARCH_CPU_H
#define __ARCH_CPU_H

/* arm specific stuff */
#define PAGE_SIZE 4096

#if defined(ARM_CPU_ARM1136)
 #define CACHE_LINE 32
#elif defined(ARM_CPU_CORE_A5)
 #define CACHE_LINE 32
#elif defined(ARM_CPU_CORE_SCORPION)
 #define CACHE_LINE 32
#elif defined(ARM_CPU_CORE_KRAIT) || defined(ARM_CPU_CORE_A7)
 #define CACHE_LINE 64
#else
 #error unknown cpu
#endif

#define IS_CACHE_LINE_ALIGNED(addr)  !((uint32_t) (addr) & (CACHE_LINE - 1))

#if ARM_ISA_ARMV7
#define dsb() __asm__ volatile ("dsb" : : : "memory");
#define dmb() __asm__ volatile ("dmb" : : : "memory");
#define isb() __asm__ volatile ("isb" : : : "memory");
#elif ARM_ISA_ARMV6
#define dsb() __asm__ volatile ("mcr p15, 0, %0, c7, c10, 4" : : "r" (0): "memory");
#define dmb() __asm__ volatile ("mcr p15, 0, %0, c7, c10, 5" : : "r" (0): "memory");
#define isb() __asm__ volatile ("mcr p15, 0, %0, c7, c5,  4" : : "r" (0): "memory");
#endif

#define GET_CAHE_LINE_START_ADDR(addr) ROUNDDOWN(addr, CACHE_LINE)

#endif
