#ifndef _SPI_H
#define _SPI_H
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/device.h>
struct spidev_data {
	dev_t			devt;
	spinlock_t		spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;

	/* TX/RX buffers are NULL unless this device is open (users > 0) */
	struct mutex		buf_lock;
	unsigned		users;
	u8			*tx_buffer;
	u8			*rx_buffer;
	u32			speed_hz;
};

int  spi_write_private(struct spi_device *device, u8 *in_buf, u32 in_len);
int spi_write_then_read_private(struct spi_device *device, u8 *in_buf, u32 in_len, u8 *out_buf, u32 out_len);
int spi_init(struct spi_device *spi);
void spi_uninit(struct spi_device *spi);
#endif
