/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_HYP_TAKEOVER_H
#define LK2ND_HYP_TAKEOVER_H

/**
 * hvc() - Call hvc instruction with given arguments.
 * @x0: Value of x0 register
 * @x1: Value of x1 register
 *
 * Returns: Value of x0 register after return from hvc.
 */
int hvc(unsigned long x0, unsigned long x1);

/**
 * lk2nd_replace_hyp() - Replace the hyp with given payload.
 * @payload:      Data to replace the hyp with.
 * @payload_size: Size of the @payload.
 * @vbar_offt:    Offset from the start of the payload to vbar_el2
 *
 * Platform-specific implementation for hyp takeover. Implementation takes
 * over the el2 code by any means then places payload at the start of the
 * hyp memory, setting vbar_el2 register to hyp base + @vbar_offt.
 *
 * Returns: 0 on success and an error code otherwise.
 */
int lk2nd_replace_hyp(void *payload, int payload_size, unsigned long vbar_offt);

#endif /* LK2ND_HYP_TAKEOVER_H */
