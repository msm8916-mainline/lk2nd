/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Code Aurora nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CLOCK_H
#define CLOCK_H

#undef readl_relaxed
#undef writel_relaxed

#ifdef MSM_SECURE_IO
#define readl_relaxed secure_readl
#define writel_relaxed secure_writel
#else
#define readl_relaxed readl
#define writel_relaxed writel
#endif

enum clk_reset_action {
	CLK_RESET_DEASSERT	= 0,
	CLK_RESET_ASSERT	= 1
};

struct clk;
struct clk_ops {
	int (*enable)(struct clk *clk);
	void (*disable)(struct clk *clk);
	void (*auto_off)(struct clk *clk);
	int (*reset)(struct clk *clk, enum clk_reset_action action);
	int (*set_rate)(struct clk *clk, unsigned rate);
	int (*set_min_rate)(struct clk *clk, unsigned rate);
	int (*set_max_rate)(struct clk *clk, unsigned rate);
	int (*set_flags)(struct clk *clk, unsigned flags);
	unsigned (*get_rate)(struct clk *clk);
	int (*list_rate)(struct clk *clk, unsigned n);
	int (*is_enabled)(struct clk *clk);
	long (*round_rate)(struct clk *clk, unsigned rate);
	int (*set_parent)(struct clk *clk, struct clk *parent);
	struct clk *(*get_parent)(struct clk *clk);
	bool (*is_local)(struct clk *clk);
};

/**
 * struct clk
 * @count: enable refcount
 * @lock: protects clk_enable()/clk_disable() path and @count
 */
struct clk {
	uint32_t flags;
	struct clk_ops *ops;
	const char *dbg_name;
	unsigned count;
};

/**
 * clk_get - lookup and obtain a reference to a clock producer.
 * @dev: device for clock "consumer"
 * @id: clock comsumer ID
 *
 * Returns a struct clk corresponding to the clock producer, or
 * valid IS_ERR() condition containing errno.  The implementation
 * uses @dev and @id to determine the clock consumer, and thereby
 * the clock producer.  (IOW, @id may be identical strings, but
 * clk_get may return different clock producers depending on @dev.)
 *
 * Drivers must assume that the clock source is not enabled.
 *
 * clk_get should not be called from within interrupt context.
 */
struct clk *clk_get(const char *id);


/**
 * clk_enable - inform the system when the clock source should be running.
 * @clk: clock source
 *
 * If the clock can not be enabled/disabled, this should return success.
 *
 * Returns success (0) or negative errno.
 */
int clk_enable(struct clk *clk);

/**
 * clk_disable - inform the system when the clock source is no longer required.
 * @clk: clock source
 *
 * Inform the system that a clock source is no longer required by
 * a driver and may be shut down.
 *
 * Implementation detail: if the clock source is shared between
 * multiple drivers, clk_enable() calls must be balanced by the
 * same number of clk_disable() calls for the clock source to be
 * disabled.
 */
void clk_disable(struct clk *clk);

/**
 * clk_get_rate - obtain the current clock rate (in Hz) for a clock source.
 *		  This is only valid once the clock source has been enabled.
 * @clk: clock source
 */
unsigned long clk_get_rate(struct clk *clk);

/**
 * clk_set_rate - set the clock rate for a clock source
 * @clk: clock source
 * @rate: desired clock rate in Hz
 *
 * Returns success (0) or negative errno.
 */
int clk_set_rate(struct clk *clk, unsigned long rate);

/**
 * clk_set_parent - set the parent clock source for this clock
 * @clk: clock source
 * @parent: parent clock source
 *
 * Returns success (0) or negative errno.
 */
int clk_set_parent(struct clk *clk, struct clk *parent);

/**
 * clk_get_parent - get the parent clock source for this clock
 * @clk: clock source
 *
 * Returns struct clk corresponding to parent clock source, or
 * valid IS_ERR() condition containing errno.
 */
struct clk *clk_get_parent(struct clk *clk);

/**
 * clk_get_set_enable -
 * -- get the clock.
 * -- set the rate to @rate if @rate is non-zero
 * -- enable the clock if @enable = ture;
 * @id: clock identifier (char *)
 * @rate: desired clock rate in Hz
 *
 * Returns success (0) or negative errno.
 */
int clk_get_set_enable(char *id, unsigned long rate, bool enable);

struct clk_lookup {
	const char		*con_id;
	struct clk		*clk;
};

struct clk_list {
	struct clk_lookup *clist;
	unsigned num;
};

#define CLK_LOOKUP(con, c) { .con_id = con, .clk = &c }

#ifdef DEBUG_CLOCK
struct clk_list *clk_get_list(void);
#endif

#endif
