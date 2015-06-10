
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include <linux/slab.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/uaccess.h>
#include "spi.h"


static unsigned bufsiz = 4096;
module_param(bufsiz, uint, S_IRUGO);
MODULE_PARM_DESC(bufsiz, "data bytes in biggest supported SPI message");

static void spidev_complete(void *arg)
{
	complete(arg);
}

static ssize_t
spidev_sync(struct spidev_data *spidev, struct spi_message *message)
{
	DECLARE_COMPLETION_ONSTACK(done);
	int status;

	message->complete = spidev_complete;
	message->context = &done;

	spin_lock_irq(&spidev->spi_lock);
	if (spidev->spi == NULL)
		status = -ESHUTDOWN;
	else
		status = spi_async(spidev->spi, message);
	spin_unlock_irq(&spidev->spi_lock);

	if (status == 0) {
		wait_for_completion(&done);
		status = message->status;
		if (status == 0)
			status = message->actual_length;
	}
	return status;
}

static inline ssize_t spidev_sync_write(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.tx_buf		= spidev->tx_buffer,
			.len		= len,
			.speed_hz	= spidev->speed_hz,
		};
	struct spi_message	m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spidev_sync(spidev, &m);
}

static inline ssize_t spidev_sync_read(struct spidev_data *spidev, size_t len)
{
	struct spi_transfer	t = {
			.rx_buf		= spidev->rx_buffer,
			.len		= len,
			.speed_hz	= spidev->speed_hz,
		};
	struct spi_message	m;

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spidev_sync(spidev, &m);
}


int  spi_write_private(struct spi_device *device, u8 *in_buf, u32 in_len)
{

    struct spidev_data *spidev;
    int status = 0;
    unsigned long missing = 0;
	if(in_len <= 0)
            return -EINVAL;
        spidev = spi_get_drvdata(device);

	mutex_lock(&spidev->buf_lock);
	memcpy(spidev->tx_buffer, in_buf, in_len);
	if (missing == 0)
		status = spidev_sync_write(spidev, in_len);
	else
		status = -EFAULT;
	mutex_unlock(&spidev->buf_lock);

	return status;
}

int spi_write_then_read_private(struct spi_device *device, u8 *in_buf, u32 in_len, u8 *out_buf, u32 out_len)
{
    return spi_write_then_read(device, in_buf, in_len, out_buf, out_len);
}
extern int spidev_init_open(struct spidev_data *spidev);
int spi_init(struct spi_device *spi)
{
    spidev_init_open(spi_get_drvdata(spi));
    return 0;
}
extern void spidev_uninit(struct spidev_data *spidev);
void spi_uninit(struct spi_device *spi)
{
    spidev_uninit(spi_get_drvdata(spi));
}





