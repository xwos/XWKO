/**
 * @file
 * @brief 点对点通讯协议：UART硬件接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0 (the "MPL"). If a copy of the MPL was not distributed
 * > with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * >
 * > Alternatively, the contents of this file may be used under the
 * > terms of the GNU General Public License version 2 (the "GPL"), in
 * > which case the provisions of the GPL are applicable instead of the
 * > above. If you wish to allow the use of your version of this file
 * > only under the terms of the GPL and not to allow others to use your
 * > version of this file under the MPL, indicate your decision by
 * > deleting the provisions above and replace them with the notice and
 * > other provisions required by the GPL. If you do not delete the
 * > provisions above, a recipient may use your version of this file
 * > under either the MPL or the GPL.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <xwos/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/hwif/uart.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t xwpcpif_uart_open(struct xwpcp * xwpcp);

static
xwer_t xwpcpif_uart_close(struct xwpcp * xwpcp);

static
xwer_t xwpcpif_uart_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size);

static
xwer_t xwpcpif_uart_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size);

static
void xwpcpif_uart_notify(struct xwpcp * xwpcp, xwsq_t ntf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwpcp_hwifal_operations xwpcpif_uart_ops = {
        .open = xwpcpif_uart_open,
        .close = xwpcpif_uart_close,
        .tx = xwpcpif_uart_tx,
        .rx = xwpcpif_uart_rx,
        .notify = xwpcpif_uart_notify,
};

struct ktermios xwpcpif_uart_dev_termios = {
        .c_cflag = CREAD | HUPCL | CLOCAL | CS8 | BRDCFG_UART_BAUDRATE,
        .c_iflag = IGNBRK | IGNPAR,
        .c_oflag = 0,
        .c_lflag = 0,
        .c_cc[VTIME] = 0,
        .c_cc[VMIN]  = 1,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t xwpcpif_uart_open(struct xwpcp * xwpcp)
{
        struct file * filp;
        struct tty_struct * tty;
        xwer_t rc;

        filp = filp_open(xwpcp->hwifcb, O_RDWR | O_NOCTTY, 0);
        if (is_err(filp)) {
                rc = ptr_err(filp);
                xwpcplogf(ERR, "open %s ... [%d].\n", xwpcp->hwifcb, rc);
	} else {
                xwpcplogf(INFO, "open %s ... [OK].\n", xwpcp->hwifcb);
                xwpcp->hwifcb = filp;
                tty = ((struct tty_file_private *)filp->private_data)->tty;
                /* set uart port attribute */
                tty_set_termios(tty, &xwpcpif_uart_dev_termios);
                rc = XWOK;
        }
        return rc;
}

static
xwer_t xwpcpif_uart_close(struct xwpcp * xwpcp)
{
        filp_close(xwpcp->hwifcb, current->files);
        xwpcp->hwifcb = NULL;
        xwpcplogf(INFO, "close hwif ... [OK].\n");
        return XWOK;
}

static
xwer_t xwpcpif_uart_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size)
{
        xwer_t rc;
	mm_segment_t fs;
	struct file * filp;
        xwssz_t ret, rest;

        rc = XWOK;
        fs = get_fs();
        filp = xwpcp->hwifcb;
        rest = (xwssz_t)size;
        set_fs(KERNEL_DS);
        do {
                ret = vfs_write(filp, &data[size - rest], rest, &filp->f_pos);
                if (ret > 0) {
                        rest -= ret;
                } else {
                        rc = (xwer_t)ret;
                        break;
                }
        } while (rest > 0);
        set_fs(fs);
        return rc;
}

static
xwer_t xwpcpif_uart_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size)
{
	mm_segment_t fs;
        struct file * filp;
        xwssz_t ret, rxsize, rest;
        xwer_t rc;

        rc = XWOK;
        rxsize = 0;
        rest = (xwssz_t)*size;
        filp = xwpcp->hwifcb;
        fs = get_fs(); /* get fs */
        set_fs(KERNEL_DS);
        do {
                ret = vfs_read(filp, &buf[rxsize], (xwsz_t)rest, &filp->f_pos);
                if (__xwcc_unlikely(ret < 0)) {
                        rc = (xwer_t)ret;
                        xwpcplogf(INFO, "vfs_read() ... [%d].", rc);
                        linux_thrd_clear_fake_signal(current);
                        break;
                } else {
                        rxsize += ret;
                        rest -= ret;
                }
        } while (rest > 0);
        set_fs(fs);
        *size = (xwsz_t)rxsize;
        return rc;
}

static
void xwpcpif_uart_notify(struct xwpcp * xwpcp, xwsq_t ntf)
{
        if (XWPCP_HWIFNTF_NETUNREACH == ntf) {
        }
}
