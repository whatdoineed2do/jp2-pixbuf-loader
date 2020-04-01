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

#define GDK_PIXBUF_ENABLE_BACKEND
	#include <gdk-pixbuf/gdk-pixbuf.h>
#undef  GDK_PIXBUF_ENABLE_BACKEND

#include <openjpeg.h>
#include <string.h>
#include <util.h>
#include <color.h>

typedef struct {
	GdkPixbufModuleSizeFunc size_func;
	GdkPixbufModuleUpdatedFunc update_func;
	GdkPixbufModulePreparedFunc prepare_func;
	gpointer user_data;
	GdkPixbuf *pixbuf;
	GError **error;
} JP2Context;

static void free_buffer (guchar *pixels, gpointer data)
{
	g_free(pixels);
}

static GdkPixbuf *gdk_pixbuf__jp2_image_load(FILE *fp, GError **error)
{
	int codec_type;
	GdkPixbuf *pixbuf = NULL;
	opj_codec_t *codec = NULL;
	opj_image_t *image = NULL;
	opj_stream_t *stream = NULL;
	opj_dparameters_t parameters;

	opj_set_default_decoder_parameters(&parameters);

	stream = util_create_stream(fp);
	if(!stream)
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Failed to create stream fom file");
		return FALSE;
	}

	codec_type = util_identify(fp);
	if(codec_type < 0)
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Unknown filetype!");
		return FALSE;
	}
	codec = opj_create_decompress(codec_type);

	#if DEBUG == TRUE
		opj_set_info_handler(codec, info_callback, 00);
		opj_set_warning_handler(codec, warning_callback, 00);
		opj_set_error_handler(codec, error_callback, 00);
	#endif

	if(!opj_setup_decoder(codec, &parameters))
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Failed to setup decoder");
		return FALSE;
	}

	if(!opj_codec_set_threads(codec, 1))
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Failed to set thread count");
		return FALSE;
	}

	if(!opj_read_header(stream, codec, &image))
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Failed to read header");
		return FALSE;
	}

	if(!opj_decode(codec, stream, image) && opj_end_decompress(codec, stream))
	{
		util_destroy(codec, stream, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Failed to decode the image");
		return FALSE;
	}

	opj_stream_destroy(stream);
	opj_destroy_codec(codec);

	// Get components and colorspace needed to convert to RGB

	int components = -1;
	COLOR_SPACE colorspace = -1;
	if(!color_info(image, &components, &colorspace))
	{
		util_destroy(NULL, NULL, image);
		g_set_error(error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Unsupported colorspace");
		return FALSE;
	}

	// Allocate space for GdkPixbuf RGB

	guint8 *data = g_malloc(sizeof(guint8) * (int) image->comps[0].w * (int) image->comps[0].h * components);

	// Convert image to RGB depending on the colorspace

	switch(colorspace)
	{
		case COLOR_SPACE_RGB:
			color_convert_rgb(image, data);
			break;
		case COLOR_SPACE_GRAY:
			color_convert_gray(image, data);
			break;
		case COLOR_SPACE_GRAY12:
			color_convert_gray12(image, data);
			break;
		case COLOR_SPACE_SYCC420:
			color_convert_sycc420(image, data);
			break;
		case COLOR_SPACE_SYCC422:
			color_convert_sycc422(image, data);
			break;
		case COLOR_SPACE_SYCC444:
			color_convert_sycc444(image, data);
			break;
	}

	pixbuf = gdk_pixbuf_new_from_data(
		(const guchar*) data,                           // Actual data. RGB: {0, 0, 0}. RGBA: {0, 0, 0, 0}.
		GDK_COLORSPACE_RGB,                             // Colorspace (only RGB supported, lol, what's the point)
		(image->numcomps == 4 || image->numcomps == 2), // has_alpha
		8,                                              // bits_per_sample (only 8 bit supported, again, why even bother)
		(int) image->comps[0].w,                        // width
		(int) image->comps[0].h,                        // height
		util_rowstride(image, components),              // rowstride: distance in bytes between row starts
		free_buffer,                                    // destroy function
		NULL                                            // closure data to pass to the destroy notification function
	);

	return pixbuf;
}

#if FALSE

static gpointer gdk_pixbuf__jp2_image_begin_load
(
	GdkPixbufModuleSizeFunc size_func,
	GdkPixbufModulePreparedFunc prepare_func,
	GdkPixbufModuleUpdatedFunc update_func,
	gpointer user_data,
	GError **error
) {
	JP2Context *context = g_new0 (JP2Context, 1);
	context->size_func = size_func;
	context->prepare_func = prepare_func;
	context->update_func  = update_func;
	context->user_data = user_data;
	return context;
}

static gboolean gdk_pixbuf__jp2_image_stop_load(gpointer context, GError **error)
{
	JP2Context *data = (JP2Context *) context;
	g_return_val_if_fail(data != NULL, TRUE);
	if (data->pixbuf) {
		g_object_unref(data->pixbuf);
	}
	return TRUE;
}

static gboolean gdk_pixbuf__jp2_image_load_increment(gpointer context, const guchar *buf, guint size, GError **error)
{
	JP2Context *data = (JP2Context *) context;

	g_set_error (error, GDK_PIXBUF_ERROR, GDK_PIXBUF_ERROR_FAILED, "Bro, I just can't...");
	return FALSE;
}

static gboolean gdk_pixbuf__jp2_image_save
(
	FILE *f,
	GdkPixbuf *pixbuf,
	gchar **keys,
	gchar **values,
	GError **error
) {
	return TRUE;
}

static gboolean gdk_pixbuf__jp2_image_save_to_callback
(
	GdkPixbufSaveFunc save_func,
	gpointer user_data,
	GdkPixbuf *pixbuf,
	gchar **keys,
	gchar **values,
	GError **error
) {
	return TRUE;
}

#endif

/**
 * Module entry points - This is where it all starts
 */

void fill_vtable(GdkPixbufModule *module)
{
	module->load             = gdk_pixbuf__jp2_image_load;
	// TODO: consider implementing these
	//module->save             = gdk_pixbuf__jp2_image_save;
	//module->stop_load        = gdk_pixbuf__jp2_image_stop_load;
	//module->begin_load       = gdk_pixbuf__jp2_image_begin_load;
	//module->load_increment   = gdk_pixbuf__jp2_image_load_increment;
	//module->save_to_callback = gdk_pixbuf__jp2_image_save_to_callback;
}

void fill_info(GdkPixbufFormat *info)
{
	static GdkPixbufModulePattern signature[] =
	{
		{ "    jP", "!!!!  ", 100 },		/* file begins with 'jP' at offset 4 */
		{ "\xff\x4f\xff\x51\x00", NULL, 100 },	/* file starts with FF 4F FF 51 00 */
		{ NULL, NULL, 0 }
	};

	static gchar *mime_types[] =
	{
		"image/jp2",
		"image/jpm",
		"image/jpx",
		"image/jpeg2000",
		"image/x-jp2-codestream",
		NULL
	};

	static gchar *extensions[] =
	{
		"j2c",
		"j2k",
		"jp2",
		"jpc",
		"jpf",
		"jpm",
		"jpx",
		NULL
	};

	info->description = "JPEG2000";
	info->extensions  = extensions;
	info->flags       = GDK_PIXBUF_FORMAT_THREADSAFE; // TODO: | GDK_PIXBUF_FORMAT_WRITABLE
	info->license     = "LGPL";
	info->mime_types  = mime_types;
	info->name        = "jp2";
	info->signature   = signature;
}
