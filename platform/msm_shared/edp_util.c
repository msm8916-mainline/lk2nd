/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include "edp.h"

extern struct edp_aux_ctrl edpctrl;

extern int edp_hpd_done;
extern int edp_video_ready;

/*
 * edp buffer operation
 */
char *edp_buf_init(struct edp_buf *eb, char *buf, int size)
{
	eb->start = buf;
	eb->size = size;
	eb->data = eb->start;
	eb->end = eb->start + eb->size;
	eb->len = 0;
	eb->trans_num = 0;
	eb->i2c = 0;
	return eb->data;
}

static char *edp_buf_reset(struct edp_buf *eb)
{
	eb->data = eb->start;
	eb->len = 0;
	eb->trans_num = 0;
	eb->i2c = 0;
	return eb->data;
}

static char *edp_buf_push(struct edp_buf *eb, int len)
{
	eb->data += len;
	eb->len += len;
	return eb->data;
}

static int edp_buf_trailing(struct edp_buf *eb)
{
	return (int)(eb->end - eb->data);
}

/*
 * edp aux edp_buf_add_cmd:
 * NO native and i2c command mix allowed
 */
static int edp_buf_add_cmd(struct edp_buf *eb, struct edp_cmd *cmd)
{
	char data;
	char *bp, *cp;
	int i, len;

	if (cmd->read)	/* read */
		len = 4;
	else
		len = cmd->len + 4;

	if (edp_buf_trailing(eb) < len)
		return 0;

	/*
	 * cmd fifo only has depth of 144 bytes
	 * limit buf length to 128 bytes here
	 */
	if ((eb->len + len) > 128)
		return 0;

	bp = eb->data;
	data = cmd->addr >> 16;
	data &=  0x0f;	/* 4 addr bits */
	if (cmd->read)
		data |=  BIT(4);
	*bp++ = data;
	*bp++ = cmd->addr >> 8;
	*bp++ = cmd->addr;
	*bp++ = cmd->len - 1;

	if (!cmd->read) { /* write */
		cp = cmd->datap;
		for (i = 0; i < cmd->len; i++)
			*bp++ = *cp++;
	}
	edp_buf_push(eb, len);

	if (cmd->i2c)
		eb->i2c++;

	eb->trans_num++;	/* Increase transaction number */

	return cmd->len - 1;
}

static int edp_cmd_fifo_tx(struct edp_buf *tp)
{
	int data;
	char *dp;
	int len, cnt;

	len = tp->len;	/* total byte to cmd fifo */
	if (len == 0)
		return 0;

	cnt = 0;
	dp = tp->start;

	while (cnt < len) {
		data = *dp; /* data byte */
		data <<= 8;
		data &= 0x00ff00; /* index = 0, write */
		if (cnt == 0)
			data |= BIT(31);  /* INDEX_WRITE */
		dprintf(SPEW, "%s: data=%x\n",__func__,  data);
		edp_write(EDP_BASE + EDP_AUX_DATA, data);
		cnt++;
		dp++;
	}

	data = (tp->trans_num - 1);
	if (tp->i2c)
		data |= BIT(8); /* I2C */

	data |= BIT(9); /* GO */
	dprintf(SPEW, "%s: data=%x\n",__func__,  data);
	edp_write(EDP_BASE + EDP_AUX_TRANS_CTRL, data);

	return tp->len;
}

static int edp_cmd_fifo_rx(struct edp_buf *rp, int len)
{
	int data;
	char *dp;
	int i;

	data = 0; /* index = 0 */
	data |= BIT(31);  /* INDEX_WRITE */
	data |= BIT(0);	/* read */
	edp_write(EDP_BASE + EDP_AUX_DATA, data);

	dp = rp->data;

	/* discard first byte */
	data = edp_read(EDP_BASE + EDP_AUX_DATA);
	for (i = 0; i < len; i++) {
		data = edp_read(EDP_BASE + EDP_AUX_DATA);
		dprintf(SPEW, "%s: data=%x\n", __func__, data);
		*dp++ = (char)((data >> 8) & 0xff);
	}

	rp->len = len;
	return len;
}


void edp_aux_native_handler(unsigned int isr)
{

	dprintf(SPEW, "%s: isr=%x\n", __func__, isr);

	if (isr & EDP_INTR_AUX_I2C_DONE)
		edpctrl.aux_error_num = EDP_AUX_ERR_NONE;
	else if (isr & EDP_INTR_WRONG_ADDR)
		edpctrl.aux_error_num = EDP_AUX_ERR_ADDR;
	else if (isr & EDP_INTR_TIMEOUT)
		edpctrl.aux_error_num = EDP_AUX_ERR_TOUT;
	if (isr & EDP_INTR_NACK_DEFER)
		edpctrl.aux_error_num = EDP_AUX_ERR_NACK;
}

void edp_aux_i2c_handler(unsigned int isr)
{

	dprintf(SPEW, "%s: isr=%x\n", __func__, isr);

	if (isr & EDP_INTR_AUX_I2C_DONE) {
		if (isr & (EDP_INTR_I2C_NACK | EDP_INTR_I2C_DEFER))
			edpctrl.aux_error_num = EDP_AUX_ERR_NACK;
		else
			edpctrl.aux_error_num = EDP_AUX_ERR_NONE;
	} else {
		if (isr & EDP_INTR_WRONG_ADDR)
			edpctrl.aux_error_num = EDP_AUX_ERR_ADDR;
		else if (isr & EDP_INTR_TIMEOUT)
			edpctrl.aux_error_num = EDP_AUX_ERR_TOUT;
		if (isr & EDP_INTR_NACK_DEFER)
			edpctrl.aux_error_num = EDP_AUX_ERR_NACK;
		if (isr & EDP_INTR_I2C_NACK)
			edpctrl.aux_error_num = EDP_AUX_ERR_NACK;
		if (isr & EDP_INTR_I2C_DEFER)
			edpctrl.aux_error_num = EDP_AUX_ERR_NACK;
	}
}

void mdss_edp_irq_enable(void)
{
        edp_write(EDP_BASE + 0x308, EDP_INTR_MASK1);
        edp_write(EDP_BASE + 0x30c, EDP_INTR_MASK2);
}

void mdss_edp_irq_disable(void)
{
        edp_write(EDP_BASE + 0x308, 0);
        edp_write(EDP_BASE + 0x30c, 0);
}

int edp_isr_read(unsigned int *isr1, unsigned int *isr2)
{
        unsigned int data1, data2, mask1, mask2;
        unsigned int ack;

        data1 = edp_read(EDP_BASE + 0x308);
        data2 = edp_read(EDP_BASE + 0x30c);

	if (data1 == 0 && data2 == 0)
		return 0;

        mask1 = data1 & EDP_INTR_MASK1;
        mask2 = data2 & EDP_INTR_MASK2;

        data1 &= ~mask1; /* remove masks bit */
        data2 &= ~mask2;

        dprintf(SPEW, "%s: isr=%x mask=%x isr2=%x mask2=%x\n",
                        __func__, data1, mask1, data2, mask2);

	if (data1 == 0 && data2 == 0)	/* no irq set */
		return 0;

        ack = data1 & EDP_INTR_STATUS1;
        ack <<= 1;      /* ack bits */
        ack |= mask1;
        edp_write(EDP_BASE + 0x308, ack);

        ack = data2 & EDP_INTR_STATUS2;
        ack <<= 1;      /* ack bits */
        ack |= mask2;
        edp_write(EDP_BASE + 0x30c, ack);

	if (data1 & EDP_INTR_HPD) {
		edp_hpd_done++;
		dprintf(INFO, "%s: got EDP_INTR_HOD\n", __func__);
		data1 &= ~EDP_INTR_HPD;
	}

	if (data2 & EDP_INTR_READY_FOR_VIDEO) {
		edp_video_ready++;
		dprintf(INFO, "%s: got EDP_INTR_READY_FOR_VIDEO\n", __func__);
		data2 &= ~EDP_INTR_READY_FOR_VIDEO;
	}

	if (data1 == 0 && data2 == 0)	/* only hpd set */
		return 0;

	*isr1 = data1;
	*isr2 = data2;

	return 1;
}

void edp_isr_poll(void)
{
	int cnt;
        unsigned int isr1, isr2;

	isr1 = 0;
	isr2 = 0;

	/* one second loop here to cover
	 * the worst case for i2c edid 128 bytes read
	 */
	cnt = 1000;
	while(cnt--) {
		if (edp_isr_read(&isr1, &isr2))
			break;
		udelay(1000);
	}

	if(cnt <= 0) {
	        dprintf(INFO, "%s: NO isr\n", __func__);
		return;
	}

	dprintf(SPEW, "%s: isr1=%x isr2=%x\n", __func__, isr1, isr2);

        if (isr2 & EDP_INTR_READY_FOR_VIDEO) {
        }

        if (isr1 && edpctrl.aux_cmd_busy) {
                /* clear EDP_AUX_TRANS_CTRL */
                edp_write(EDP_BASE + 0x318, 0);
                /* read EDP_INTERRUPT_TRANS_NUM */
                edpctrl.aux_trans_num = edp_read(EDP_BASE + 0x310);

                if (edpctrl.aux_cmd_i2c)
                        edp_aux_i2c_handler(isr1);
                else
                        edp_aux_native_handler(isr1);
        }
}

int edp_aux_write_cmds(struct edp_aux_ctrl *ep,
				struct edp_cmd *cmd)
{
	struct edp_cmd *cm;
	struct edp_buf *tp;
	int len, ret;

	ep->aux_cmd_busy = 1;

	tp = &ep->txp;
	edp_buf_reset(tp);

	cm = cmd;
	while (cm) {
		dprintf(SPEW, "%s: i2c=%d read=%d addr=%x len=%d next=%d\n",
		__func__, cm->i2c, cm->read, cm->addr, cm->len, cm->next);
		ret = edp_buf_add_cmd(tp, cm);
		if (ret <= 0)
			break;
		if (cm->next == 0)
			break;
		cm++;
	}

	if (tp->i2c)
		ep->aux_cmd_i2c = 1;
	else
		ep->aux_cmd_i2c = 0;

	len = edp_cmd_fifo_tx(&ep->txp);

	edp_isr_poll();

	if (ep->aux_error_num == EDP_AUX_ERR_NONE)
		ret = len;
	else
		ret = ep->aux_error_num;

	ep->aux_cmd_busy = 0;
	return  ret;
}

int edp_aux_read_cmds(struct edp_aux_ctrl *ep,
				struct edp_cmd *cmds)
{
	struct edp_cmd *cm;
	struct edp_buf *tp;
	struct edp_buf *rp;
	int len, ret;

	ep->aux_cmd_busy = 1;

	tp = &ep->txp;
	rp = &ep->rxp;
	edp_buf_reset(tp);
	edp_buf_reset(rp);

	cm = cmds;
	len = 0;
	while (cm) {
		dprintf(SPEW, "%s: i2c=%d read=%d addr=%x len=%d next=%d\n",
		__func__, cm->i2c, cm->read, cm->addr, cm->len, cm->next);
		ret = edp_buf_add_cmd(tp, cm);
		len += cm->len;
		if (ret <= 0)
			break;
		if (cm->next == 0)
			break;
		cm++;
	}

	if (tp->i2c)
		ep->aux_cmd_i2c = 1;
	else
		ep->aux_cmd_i2c = 0;
{
        unsigned int isr1, isr2;

        isr1 = edp_read(EDP_BASE + 0x308);
        isr2 = edp_read(EDP_BASE + 0x30c);

	if (isr1 != EDP_INTR_MASK1)
	dprintf(INFO, "%s: BEFORE: isr1=%x isr2=%x\n", __func__, isr1, isr2);
}

	edp_cmd_fifo_tx(tp);

	edp_isr_poll();

	if (ep->aux_error_num == EDP_AUX_ERR_NONE)
		ret = edp_cmd_fifo_rx(rp, len);
	else
		ret = ep->aux_error_num;

	ep->aux_cmd_busy = 0;

	return ret;
}


int edp_aux_write_buf(struct edp_aux_ctrl *ep, int addr,
				char *buf, int len, int i2c)
{
	struct edp_cmd	cmd;

	cmd.read = 0;
	cmd.i2c = i2c;
	cmd.addr = addr;
	cmd.datap = buf;
	cmd.len = len & 0x0ff;
	cmd.next = 0;

	return edp_aux_write_cmds(ep, &cmd);
}

int edp_aux_read_buf(struct edp_aux_ctrl *ep, int addr,
				int len, int i2c)
{
	struct edp_cmd cmd;

	cmd.read = 1;
	cmd.i2c = i2c;
	cmd.addr = addr;
	cmd.datap = NULL;
	cmd.len = len & 0x0ff;
	cmd.next = 0;

	return edp_aux_read_cmds(ep, &cmd);
}
