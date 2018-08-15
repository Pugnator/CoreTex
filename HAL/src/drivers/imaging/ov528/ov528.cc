#include <global.hpp>
#include <common.hpp>
#include <core/vmmu.hpp>
#include <drivers/ov528.hpp>
#include <drivers/xmodem.hpp>
#include <log.hpp>

#ifdef OV528_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

class ov528 *ov528::self = nullptr;

void ov528::ov528isr (void)
{
	if (self->Reg->SR & USART_SR_RXNE) //receive
	{
		short tmp = self->Reg->DR;
		self->Reg->SR &= ~USART_SR_RXNE;
		if (!self->pictransfer && self->buf_size < sizeof self->buf)
		{
			//DEBUG_LOG(0, "0x%X\r\n", tmp);
			self->buf[self->buf_size++] = tmp;
		}
		else if (self->pictransfer)
		{
			self->pic_size++;
			self->imageblk[self->imageblk_size++] = tmp;
		}
	}
	else if (self->Reg->SR & USART_SR_TC) //transfer
	{
		self->Reg->SR &= ~USART_SR_TC;
	}
}

void
ov528::buf_reset ()
{
	buf_size = 0;
	memset ((void*) buf, 0, sizeof buf);
}

void
ov528::docmd (uint8_t *cmd)
{
	buf_reset ();
	for (short i = 0; i < CMD_SIZE; i++)
	{
		write (cmd[i]);
	}
}

void
ov528::wait_reply (word expected)
{
	WAIT_FOR(500);
	do
	{
		if (0 == expected && buf_size >= CMD_SIZE * 2)
		{
			break;
		}
	}
	while (STILL_WAIT);
}

bool
ov528::wakeup (void)
{
	uint8_t wake[] =
	{ 0xaa, 0x0d, 0, 0, 0, 0 };
	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x0d, 0, 0, 0 };
	for (word i = 0; i < WAKEUP_RETRY_COUNT; ++i)
	{
		DEBUG_LOG (0, "Waking camera up...\r\n");
		docmd (wake);
		wait_reply ();

		if (0 == memcmp ((void *) buf, ack, sizeof ack))
		{
			DEBUG_LOG (0, "Camera answered\r\n");
			for (word j = 0; j < sizeof ack; j++)
			{
				write (ack[j]);
			}
			return true;
		}
	}
	return false;
}

bool
ov528::setup (void)
{
	uint8_t cmd[] =
	{ 0xaa, 0x1, 0, 0x7, 3, 0x7 };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x01 };
	docmd (cmd);
	wait_reply ();

	if (0 == memcmp ((void *) buf, ack, sizeof ack))
	{
		DEBUG_LOG (0, "Camera was set up\r\n");
		return true;
	}
	return false;
}

bool
ov528::snapshot (void)
{
	DEBUG_LOG (0, "Image snapshot\r\n");
	uint8_t cmd[] =
	{ 0xaa, 0x5, 0, 0, 0, 0 };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x05 };
	docmd (cmd);
	wait_reply ();

	if (0 == memcmp ((void *) buf, ack, sizeof ack))
	{
		DEBUG_LOG (0, "Snapshot OK\r\n");
		return true;
	}
	return false;
}

bool
ov528::set_packet_size (void)
{
	DEBUG_LOG (0, "Setting packet size\r\n");
	uint8_t cmd[] =
	{ 0xaa, 0x6, 0x8, 0, 0x4, 0 };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x06 };
	docmd (cmd);
	wait_reply ();

	if (0 == memcmp ((void *) buf, ack, sizeof ack))
	{
		DEBUG_LOG (0, "Packet size set OK\r\n");
		return true;
	}
	return false;
}

bool
ov528::request_picture (void)
{
	DEBUG_LOG (0, "Request a picture\r\n");
	uint8_t cmd[] =
	{ 0xaa, 0x4, 0x1, 0, 0, 0 };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x04 };

	uint8_t size_ack[] =
	{ 0xaa, 0x0a, 0x01 };

	docmd (cmd);
	wait_reply ();

	if (0 == memcmp ((void *) buf, ack, sizeof ack))
	{
		DEBUG_LOG (0, "Image request OK, waiting for data to be prepared\r\n");
		for (word i = 0; i < 10; ++i)
		{
			buf_reset ();
			//wait for snapshot to complete, can take up to several minutes
			delay_ms (500);
			if (0 == memcmp ((void *) buf, size_ack, sizeof size_ack))
			{
				expected_pic_size = (buf[4] << 8) | (buf[3] & 0xff);
				DEBUG_LOG (0, "Camera completed snapshot, image size: %u\r\n",
				           expected_pic_size);
				return true;
			}
		}
		DEBUG_LOG (0, "Image shapshot failed\r\n");
	}
	return false;
}

void
ov528::hard_reset (void)
{
	DEBUG_LOG (0, "Camera hard reset\r\n");
	uint8_t cmd[] =
	{ 0xaa, 0x8, 0x0, 0, 0x0, 0 };

	docmd (cmd);
}

void
ov528::soft_reset (void)
{
	DEBUG_LOG (0, "Camera soft reset\r\n");
	uint8_t cmd[] =
	{ 0xaa, 0x8, 0x1, 0, 0x0, 0 };

	docmd (cmd);
}

bool
ov528::sleep (void)
{
	uint8_t cmd[] =
	{ 0xaa, 0x9, 0, 0, 0, 0 };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x09 };
	docmd (cmd);
	wait_reply ();

	if (0 == memcmp ((void *) buf, ack, sizeof ack))
	{
		return true;
	}
	return false;
}

void
ov528::start_transfer (void)
{
	FIL pic;
	FRESULT res = disk.open (&pic, "image.jpg", FA_WRITE | FA_CREATE_ALWAYS);
	if (FR_OK != res)
	{
		DEBUG_LOG (0, "Failed to create the file\r\n");

	}
	else
	{
		DEBUG_LOG (0, "Created the image file\r\n");
	}

	DEBUG_LOG (0, "Starting image transfer\r\n");
	pictransfer = true;
	pic_size = 0;
	uint8_t cmd[] =
	{ 0xaa, 0xe, 0, 0, 0, 0 };

	uint8_t stop[] =
	{ 0xaa, 0xe, 0, 0, 0xf, 0xf };

	uint8_t ack[] =
	{ 0xaa, 0x0e, 0x0e };

	volatile uint8_t image[1100] =
	{ 0 };
	imageblk = image;
	docmd (cmd);

	DEBUG_LOG (0, "Starting transfer\r\n");
	word blocksn =
	    expected_pic_size % 1024 ?
	        expected_pic_size / 1024 + 1 : expected_pic_size / 1024;
	DEBUG_LOG (0, "Blocks to read: %u\r\n", blocksn);
	for (word i = 0; i < blocksn; ++i)
	{
		imageblk_size = 0;
		cmd[4] = i;
		docmd (cmd);

		for (;;)
		{
			word tmp = imageblk_size;
			delay_ms (25);
			if (tmp == imageblk_size)
			{
				break;
			}
		}

		/* TRANSFER */
		word id = (imageblk[1] << 8) | (imageblk[0] & 0xFF);
		word size = (imageblk[3] << 8) | (imageblk[2] & 0xFF);
		word crc = imageblk[imageblk_size - 1];
		DEBUG_LOG (0, "Writing image block, ID:%u Size:%u CRC: 0x%X\r\n", id, size,
		           crc);
		unsigned written = 0;
		res = disk.f_write(&pic, (void*)imageblk + 4, imageblk_size - 6, &written);
		if (written == size)
		{
			DEBUG_LOG (0, "Wrote %u bytes\r\n", written);
		}

	}
	docmd (stop);
	pictransfer = false;
	DEBUG_LOG (0, "Transfer completed, %u == %u\r\n", pic_size,
	           expected_pic_size);
	disk.close (&pic);
}

uint8_t*
ov528::next_block (word *size)
{
	*size = imageblk_size - 6;
	return nullptr;
}

bool
ov528::default_setup (void)
{
	bool res = true;
	res &= wakeup ();
	res &= setup ();
	res &= set_packet_size ();
	DEBUG_LOG (0, "Camera setup %s\r\n", res ? "OK" : "FAILED");
	return res;
}

