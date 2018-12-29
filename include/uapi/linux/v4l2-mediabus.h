/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * Media Bus API header
 *
 * Copyright (C) 2009, Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_V4L2_MEDIABUS_H
#define __LINUX_V4L2_MEDIABUS_H

#include <linux/media-bus-format.h>
#include <linux/types.h>
#include <linux/videodev2.h>

/**
 * struct v4l2_mbus_framefmt - frame format on the media bus
 * @width:	frame width
 * @height:	frame height
 * @code:	data format code (from enum v4l2_mbus_pixelcode)
 * @field:	used interlacing type (from enum v4l2_field)
 * @colorspace:	colorspace of the data (from enum v4l2_colorspace)
 * @ycbcr_enc:	YCbCr encoding of the data (from enum v4l2_ycbcr_encoding)
 * @quantization: quantization of the data (from enum v4l2_quantization)
 * @xfer_func:  transfer function of the data (from enum v4l2_xfer_func)
 */
struct v4l2_mbus_framefmt {
	__u32			width;
	__u32			height;
	__u32			code;
	__u32			field;
	__u32			colorspace;
	__u16			ycbcr_enc;
	__u16			quantization;
	__u16			xfer_func;
	__u16			reserved[11];
};

#ifndef __KERNEL__
/*
 * enum v4l2_mbus_pixelcode and its definitions are now deprecated, and
 * MEDIA_BUS_FMT_ definitions (defined in media-bus-format.h) should be
 * used instead.
 *
 * New defines should only be added to media-bus-format.h. The
 * v4l2_mbus_pixelcode enum is frozen.
 */

#define V4L2_MBUS_FROM_MEDIA_BUS_FMT(name)	\
	V4L2_MBUS_FMT_ ## name = MEDIA_BUS_FMT_ ## name

enum v4l2_mbus_pixelcode {
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(FIXED),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB444_2X8_PADHI_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB444_2X8_PADHI_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB555_2X8_PADHI_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB555_2X8_PADHI_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(BGR565_2X8_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(BGR565_2X8_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB565_2X8_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB565_2X8_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB666_1X18),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB888_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB888_2X12_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB888_2X12_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(ARGB8888_1X32),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RBG888_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(RGB888_1X32_PADHI),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(Y8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UV8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY8_1_5X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY8_1_5X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV8_1_5X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU8_1_5X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY8_2X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY8_2X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV8_2X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU8_2X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(Y10_1X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY10_2X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY10_2X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV10_2X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU10_2X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(Y12_1X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY8_1X16),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY8_1X16),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV8_1X16),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU8_1X16),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YDYUYDYV8_1X16),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY10_1X20),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY10_1X20),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV10_1X20),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU10_1X20),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUV10_1X30),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(AYUV8_1X32),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY12_2X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY12_2X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV12_2X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU12_2X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(UYVY12_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VYUY12_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YUYV12_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(YVYU12_1X24),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(VUY8_1X24),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGBRG8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGRBG8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SRGGB8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_ALAW8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGBRG10_ALAW8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGRBG10_ALAW8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SRGGB10_ALAW8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_DPCM8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGBRG10_DPCM8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGRBG10_DPCM8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SRGGB10_DPCM8_1X8),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_2X8_PADHI_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_2X8_PADHI_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_2X8_PADLO_BE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_2X8_PADLO_LE),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR10_1X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGBRG10_1X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGRBG10_1X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SRGGB10_1X10),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SBGGR12_1X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGBRG12_1X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SGRBG12_1X12),
	V4L2_MBUS_FROM_MEDIA_BUS_FMT(SRGGB12_1X12),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(JPEG_1X8),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(S5C_UYVY_JPEG_1X8),

	V4L2_MBUS_FROM_MEDIA_BUS_FMT(AHSV8888_1X32),
};
#endif /* __KERNEL__ */

#endif
