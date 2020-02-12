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
} COLOR_SPACE;

/*
 * Sets number of components and colorspace
 */
gboolean color_info(opj_image_t *image, int *components, COLOR_SPACE *colorspace)
{
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
			} else {
				*colorspace = COLOR_SPACE_RGB;
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
					return FALSE;
					return COLOR_SPACE_SYCC420;
				}
				else if(
					(image->comps[0].dx == 1) &&
					(image->comps[1].dx == 2) &&
					(image->comps[2].dx == 2) &&
					(image->comps[0].dy == 1) &&
					(image->comps[1].dy == 1) &&
					(image->comps[2].dy == 1) 
				) { // Horizontal sub-sample
					return FALSE;
					return COLOR_SPACE_SYCC422;
				}
				else if(
					(image->comps[0].dx == 1) &&
					(image->comps[1].dx == 2) &&
					(image->comps[2].dx == 2) &&
					(image->comps[0].dy == 1) &&
					(image->comps[1].dy == 1) &&
					(image->comps[2].dy == 1) 
				) { // No sub-sample
					return FALSE;
					return COLOR_SPACE_SYCC444;
				}
			break;
		case OPJ_CLRSPC_SRGB:
			*colorspace = COLOR_SPACE_RGB;
			break;
		default:
			return FALSE;
	}

	*components = image->numcomps;
	
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

// TODO: support sYCC

/*
double Y = 0, Cb = 0, Cr = 0;
Y = util_get(image, 0, i, adjustR); Cb = util_get(image, 1, i, adjustR); Cr = util_get(image, 2, i, adjustR);

data[counter++] = util_clamp((int) (Y + 1.40200 * (Cr - 0x80)), 255);
data[counter++] = util_clamp((int) (Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80)), 255);
data[counter++] = util_clamp((int) (Y - 0.34414 * (Cb - 0x80) - 0.71414 * (Cr - 0x80)), 255);
*/

#endif