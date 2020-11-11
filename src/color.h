/* GdkPixbuf library - JPEG2000 Image Loader
 *
 * Copyright © 2020 Nichlas Severinsen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef COLOR_H
#define COLOR_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <openjpeg.h>

typedef enum {
	COLOR_SPACE_RGB = 1, // r,g,b, alpha optional
	COLOR_SPACE_GRAY = 2, // g, alpha optional
	COLOR_SPACE_GRAY12 = 3, // g, 12 bit, alpha optional
	COLOR_SPACE_SYCC420 = 4, // Y, Cb, Cr. 4:2:0 (2x2 share chroma), alpha optional
	COLOR_SPACE_SYCC422 = 5, // Y, Cb, Cr. 4:2:2 (2x1 share chroma), alpha optional
	COLOR_SPACE_SYCC444 = 6, // Y, Cb, Cr. 4:4:4, alpha optional
	COLOR_SPACE_CMYK = 7, // C, M, Y, K
} COLOR_SPACE;

/*
 * Sets number of components and colorspace
 */
gboolean color_info(opj_image_t *image, int *components, COLOR_SPACE *colorspace)
{
	*components = image->numcomps;

	switch(image->color_space)
	{
		case OPJ_CLRSPC_GRAY:
		case OPJ_CLRSPC_UNKNOWN:
		case OPJ_CLRSPC_UNSPECIFIED:
			if(image->numcomps < 3)
			{
				if(image->comps[0].prec == 12)
				{
					*colorspace = COLOR_SPACE_GRAY12;
				} else {
					*colorspace = COLOR_SPACE_GRAY;
				}

				// If alpha, two components, otherwize, one component
				*components = (image->numcomps == 4 || image->numcomps == 2) ? 4 : 3;
				return TRUE;
			} else if (image->numcomps == 3 || image->numcomps == 4) {
				// Fallback to RGB if unspecified and seemingly RGB or RGBA
				*colorspace = COLOR_SPACE_RGB;
			} else {
				// Only gets here if there are 5 components
				return FALSE;
			}
			break;
		case OPJ_CLRSPC_SYCC:
				if(
					(image->comps[0].dx == 1) &&
					(image->comps[1].dx == 2) &&
					(image->comps[2].dx == 2) &&
					(image->comps[0].dy == 1) &&
					(image->comps[1].dy == 2) &&
					(image->comps[2].dy == 2)
				) { // Horizontal and vertical sub-sample
					*colorspace = COLOR_SPACE_SYCC420;
				}
				else if(
					(image->comps[0].dx == 1) &&
					(image->comps[1].dx == 2) &&
					(image->comps[2].dx == 2) &&
					(image->comps[0].dy == 1) &&
					(image->comps[1].dy == 1) &&
					(image->comps[2].dy == 1)
				) { // Horizontal sub-sample
					*colorspace = COLOR_SPACE_SYCC422;
				}
				else if(
					(image->comps[0].dx == 1) &&
					(image->comps[1].dx == 1) &&
					(image->comps[2].dx == 1) &&
					(image->comps[0].dy == 1) &&
					(image->comps[1].dy == 1) &&
					(image->comps[2].dy == 1)
				) { // No sub-sample
					*colorspace = COLOR_SPACE_SYCC444;
				} else {
					return FALSE;
				}
			break;
		case OPJ_CLRSPC_SRGB:
			*colorspace = COLOR_SPACE_RGB;
			break;
		case OPJ_CLRSPC_CMYK:
			if(
				// Less than 4 components? Not CMYK then.
				(image->numcomps < 4) ||
				// C's XRsiz and YRsiz has to match MYK's
				(image->comps[0].dx != image->comps[1].dx) ||
				(image->comps[0].dx != image->comps[2].dx) ||
				(image->comps[0].dx != image->comps[3].dx) ||
				(image->comps[0].dy != image->comps[1].dy) ||
				(image->comps[0].dy != image->comps[2].dy) ||
				(image->comps[0].dy != image->comps[3].dy)
			) {
				return FALSE;
			}
			*components = image->numcomps - 1;
			*colorspace = COLOR_SPACE_CMYK;
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

/*
 * Converts decoded data from opj_decode RGB to GdkPixbuf RGB
 */
void color_convert_rgb(opj_image_t *image, guint8 *data)
{
	int counter = 0;
	int max = (1 << image->comps[0].prec) - 1;
	int adjustR = 0, adjustG = 0, adjustB = 0, adjustA = 0;
	gboolean has_alpha = (image->numcomps == 4 || image->numcomps == 2);

	adjustR = (image->comps[0].sgnd ? 1 << (image->comps[0].prec - 1) : 0);
	adjustG = (image->comps[1].sgnd ? 1 << (image->comps[1].prec - 1) : 0);
	adjustB = (image->comps[2].sgnd ? 1 << (image->comps[2].prec - 1) : 0);

	if(has_alpha)
	{
		adjustA = (image->comps[image->numcomps - 1].sgnd ? 1 << (image->comps[image->numcomps - 1].prec - 1) : 0);
	}

	for(int i = 0; i < (int) image->comps[0].w * (int) image->comps[0].h; i++)
	{
		data[counter++] = util_clamp(image->comps[0].data[i] + adjustR, max);
		data[counter++] = util_clamp(image->comps[1].data[i] + adjustG, max);
		data[counter++] = util_clamp(image->comps[2].data[i] + adjustB, max);

		if(has_alpha)
		{
			data[counter++] =  util_clamp(image->comps[image->numcomps - 1].data[i] + adjustA, max);
		}
	}
}

/**
 * Converts decoded data from opj_decode CMYK to GdkPixbuf RGB
 */
void color_convert_cmyk(opj_image_t *image, guint8 *data)
{
	int counter = 0;
	float C, M, Y, K;
	float sC, sM, sY, sK;

	sC = 1.0F / (float)((1 << image->comps[0].prec) - 1);
	sM = 1.0F / (float)((1 << image->comps[1].prec) - 1);
	sY = 1.0F / (float)((1 << image->comps[2].prec) - 1);
	sK = 1.0F / (float)((1 << image->comps[3].prec) - 1);

	for(int i = 0; i < (int) image->comps[0].w * (int) image->comps[0].h; i++)
	{
		C = 1.0F - (float)(image->comps[0].data[i]) * sC;
		M = 1.0F - (float)(image->comps[1].data[i]) * sM;
		Y = 1.0F - (float)(image->comps[2].data[i]) * sY;
		K = 1.0F - (float)(image->comps[3].data[i]) * sK;

		data[counter++] = (int)(255.0F * C * K);
		data[counter++] = (int)(255.0F * M * K);
		data[counter++] = (int)(255.0F * Y * K);
	}
}

/*
 * Converts decoded data from opj_decode GRAY to GdkPixbuf RGB
 */
void color_convert_gray(opj_image_t *image, guint8 *data)
{
	int buffer = 0;
	int counter = 0;
	int max = (1 << image->comps[0].prec) - 1;
	gboolean has_alpha = (image->numcomps == 4 || image->numcomps == 2);

	for(int i = 0; i < (int) image->comps[0].w * (int) image->comps[0].h; i++)
	{
		buffer = util_clamp(image->comps[0].data[i], max);

		data[counter++] = buffer;
		data[counter++] = buffer;
		data[counter++] = buffer;

		if(has_alpha)
		{
			data[counter++] = util_clamp(image->comps[1].data[i], max);
		}
	}
}

/*
 * Converts decoded data from opj_decode GRAY 12 bit to GdkPixbuf RGB
 */
void color_convert_gray12(opj_image_t *image, guint8 *data)
{
	int buffer = 0;
	int counter = 0;
	int max = (1 << image->comps[0].prec) - 1;
	gboolean has_alpha = (image->numcomps == 4 || image->numcomps == 2);

	for(int i = 0; i < (int) image->comps[0].w * (int) image->comps[0].h; i++)
	{
		buffer = util_clamp(image->comps[0].data[i], max) / 16;

		data[counter++] = buffer;
		data[counter++] = buffer;
		data[counter++] = buffer;

		if(has_alpha)
		{
			data[counter++] = util_clamp(image->comps[1].data[i], max) / 16;
		}
	}
}

/*
 * Converts input sYCC to RGB, putting RGB into data
 */
void color_convert_sycc(guint8 *data, int pos, int offset, int upb, int y, int cb, int cr)
{
	cb -= offset;
	cr -= offset;

	data[pos] = util_clamp(y + (int)(1.402 * (float)cr), upb);
	data[pos+1] = util_clamp(y - (int)(0.344 * (float)cb + 0.714 * (float)cr), upb);
	data[pos+2] = util_clamp(y + (int)(1.772 * (float)cb), upb);
}

/*
 * Converts decoded data from opj_decode sYCC420 to GdkPixbuf RGB
 */
void color_convert_sycc420(opj_image_t *image, guint8 *data)
{
	const int *base, *y, *cb, *cr, *ny;
	size_t maxw, maxh, offx, loopmaxw, offy, loopmaxh;
	int offset, upb;
	size_t i;

	offset = 1 << ((int)image->comps[0].prec - 1);
	upb = (1 << (int)image->comps[0].prec) - 1;

	maxw = (size_t)image->comps[0].w;
	maxh = (size_t)image->comps[0].h;

	base = image->comps[0].data;
	y = image->comps[0].data;
	cb = image->comps[1].data;
	cr = image->comps[2].data;

	offx = image->x0 & 1U;
	loopmaxw = maxw - offx;
	offy = image->y0 & 1U;
	loopmaxh = maxh - offy;

	if (offy > 0U) {
		size_t j;

		for (j = 0; j < maxw; ++j) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, 0, 0);
			++y;
		}
	}
	for (i = 0U; i < (loopmaxh & ~(size_t)1U); i += 2U) {
		size_t j;

		ny = y + maxw;

		if (offx > 0U) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, 0, 0);
			++y;
			color_convert_sycc(data, (ny - base)*3, offset, upb, *ny, *cb, *cr);
			++ny;
		}

		for (j = 0; j < (loopmaxw & ~(size_t)1U); j += 2U) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;

			color_convert_sycc(data, (ny - base)*3, offset, upb, *ny, *cb, *cr);
			++ny;
			color_convert_sycc(data, (ny - base)*3, offset, upb, *ny, *cb, *cr);
			++ny;
			++cb;
			++cr;
		}
		if (j < loopmaxw) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;

			color_convert_sycc(data, (ny - base)*3, offset, upb, *ny, *cb, *cr);
			++ny;
			++cb;
			++cr;
		}
		y += maxw;
	}
	if (i < loopmaxh) {
		size_t j;

		for (j = 0U; j < (maxw & ~(size_t)1U); j += 2U) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;

			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;
			++cb;
			++cr;
		}
		if (j < maxw) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
		}
	}
}

/*
 * Converts decoded data from opj_decode sYCC422 to GdkPixbuf RGB
 */
void color_convert_sycc422(opj_image_t *image, guint8 *data)
{
	const int *base, *y, *cb, *cr;
	size_t maxw, maxh, offx, loopmaxw;
	int offset, upb;
	size_t i;

	upb = (int)image->comps[0].prec;
	offset = 1 << (upb - 1);
	upb = (1 << upb) - 1;

	maxw = (size_t)image->comps[0].w;
	maxh = (size_t)image->comps[0].h;

	base = image->comps[0].data;
	y = image->comps[0].data;
	cb = image->comps[1].data;
	cr = image->comps[2].data;

	offx = image->x0 & 1U;
	loopmaxw = maxw - offx;

	for (i = 0U; i < maxh; ++i) {
		size_t j;

		if (offx > 0U) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, 0, 0);
			++y;
		}

		for (j = 0U; j < (loopmaxw & ~(size_t)1U); j += 2U) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;
			++cb;
			++cr;
		}
		if (j < loopmaxw) {
			color_convert_sycc(data, (y - base)*3, offset, upb, *y, *cb, *cr);
			++y;
			++cb;
			++cr;
		}
	}
}

/*
 * Converts decoded data from opj_decode sYCC444 to GdkPixbuf RGB
 */
void color_convert_sycc444(opj_image_t *image, guint8 *data)
{
	const int *y, *cb, *cr;
	size_t maxw, maxh, max;
	int offset, upb;

	upb = (int)image->comps[0].prec;
	offset = 1 << (upb - 1);
	upb = (1 << upb) - 1;

	maxw = (size_t)image->comps[0].w;
	maxh = (size_t)image->comps[0].h;
	max = maxw * maxh;

	y = image->comps[0].data;
	cb = image->comps[1].data;
	cr = image->comps[2].data;

	for (size_t i = 0U; i < max; ++i) {
		color_convert_sycc(data, i, offset, upb, *y, *cb, *cr);
		++y;
		++cb;
		++cr;
	}
}

#endif
