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

/** Module entry point - This is where it all starts
 *
 */
static GdkPixbuf *gdk_pixbuf__jp2_image_load(FILE *f, GError **error)
{
	GdkPixbuf *pixbuf = NULL;

	return pixbuf;
}

static gpointer gdk_pixbuf__jp2_image_begin_load
(
	GdkPixbufModuleSizeFunc size_func,
	GdkPixbufModulePreparedFunc prepare_func,
	GdkPixbufModuleUpdatedFunc update_func,
	gpointer user_data,
	GError **error
) {


	return NULL;
}

static gboolean gdk_pixbuf__jp2_image_stop_load(gpointer context, GError **error)
{
	return TRUE;
}

static gboolean gdk_pixbuf__jp2_image_load_increment(gpointer context, const guchar *buf, guint size, GError **error)
{
	return TRUE;
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

void fill_vtable(GdkPixbufModule *module)
{
	module->load             = gdk_pixbuf__jp2_image_load;
	module->save             = gdk_pixbuf__jp2_image_save;
	module->stop_load        = gdk_pixbuf__jp2_image_stop_load;
	module->begin_load       = gdk_pixbuf__jp2_image_begin_load;
	module->load_increment   = gdk_pixbuf__jp2_image_load_increment;
	module->save_to_callback = gdk_pixbuf__jp2_image_save_to_callback;
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
		NULL
	};

	static gchar *extensions[] =
	{
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
	info->flags       = GDK_PIXBUF_FORMAT_WRITABLE | GDK_PIXBUF_FORMAT_THREADSAFE;
	info->license     = "LGPL";
	info->mime_types  = mime_types;
	info->name        = "jp2";
	info->signature   = signature;
}
