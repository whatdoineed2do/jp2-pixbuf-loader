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

#include <openjpeg.h>

// The following defines and functions were copied from openjpeg.c
// They are not included in libopenjp2 for whatever reason.
// If they are ever included we should remove these and use them from libopenjp2 instead.

#if defined(WIN32) && !defined(Windows95) && !defined(__BORLANDC__) && \
  !(defined(_MSC_VER) && _MSC_VER < 1400) && \
  !(defined(__MINGW32__) && __MSVCRT_VERSION__ < 0x800)
	#  define OPJ_FSEEK(stream, offset, whence) _fseeki64(stream, offset, whence)
	#  define OPJ_FTELL(stream) _ftelli64(stream)
#else
	#  define OPJ_FSEEK(stream, offset, whence) fseek(stream, offset, whence)
	#  define OPJ_FTELL(stream) ftell(stream)
#endif

#define JP2_RFC3745_MAGIC "\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a"
#define JP2_MAGIC "\x0d\x0a\x87\x0a"
/* position 45: "\xff\x52" */
#define J2K_CODESTREAM_MAGIC "\xff\x4f\xff\x51"

static OPJ_SIZE_T opj_read_from_file(void *p_buffer, OPJ_SIZE_T p_nb_bytes, FILE *p_file)
{
	OPJ_SIZE_T l_nb_read = fread(p_buffer, 1, p_nb_bytes, p_file);
	return l_nb_read ? l_nb_read : (OPJ_SIZE_T) -1;
}

static OPJ_BOOL opj_seek_from_file(OPJ_OFF_T p_nb_bytes, FILE *p_user_data)
{
	if (OPJ_FSEEK(p_user_data, p_nb_bytes, SEEK_SET)) {
		return OPJ_FALSE;
	}

	return OPJ_TRUE;
}

static OPJ_OFF_T opj_skip_from_file(OPJ_OFF_T p_nb_bytes, FILE *p_user_data)
{
	if (OPJ_FSEEK(p_user_data, p_nb_bytes, SEEK_CUR)) {
		return -1;
	}

	return p_nb_bytes;
}

static OPJ_UINT64 opj_get_data_length_from_file(FILE *p_file)
{
	OPJ_OFF_T file_length = 0;

	OPJ_FSEEK(p_file, 0, SEEK_END);
	file_length = (OPJ_OFF_T) OPJ_FTELL(p_file);
	OPJ_FSEEK(p_file, 0, SEEK_SET);

	return (OPJ_UINT64) file_length;
}

static OPJ_SIZE_T opj_write_from_file(void *p_buffer, OPJ_SIZE_T p_nb_bytes, FILE *p_file)
{
	return fwrite(p_buffer, 1, p_nb_bytes, p_file);
}

// End of defines and functions copied from openjpeg.c

/**
 * Create stream from file pointer.
 * A similar funtion was deprecated and removed from openjpeg.c.
 */
opj_stream_t* util_create_stream(FILE *fp)
{
	opj_stream_t *l_stream;

	l_stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE, 1);
	if(!l_stream)
	{
		return NULL;
	}

	opj_stream_set_read_function(l_stream, (opj_stream_read_fn) opj_read_from_file);
	opj_stream_set_seek_function(l_stream, (opj_stream_seek_fn) opj_seek_from_file);
	opj_stream_set_skip_function(l_stream, (opj_stream_skip_fn) opj_skip_from_file);
	opj_stream_set_user_data(l_stream, fp, NULL);
	opj_stream_set_user_data_length(l_stream, opj_get_data_length_from_file(fp));
	opj_stream_set_write_function(l_stream, (opj_stream_write_fn) opj_write_from_file);

	return l_stream;
}

/**
 * Destroy stream, codec, and image. As long as they aren't null pointers.
 */
void util_destroy(opj_stream_t* stream, opj_codec_t *codec, opj_image_t *image)
{
	if(stream != NULL)
	{
		opj_stream_destroy(stream);
	}
	if(codec != NULL)
	{
		opj_destroy_codec(codec);
	}
	if(image != NULL)
	{
		opj_image_destroy(image);
	}
}

/**
 * Identify what OPJ_CODEC to use for input file.
 */
int util_identify(FILE *fp)
{
	int length;
	unsigned char buffer[12];

	memset(buffer, 0, 12);
	length = fread(buffer, 1, 12, fp);
	if(length != 12)
	{
		return -1;
	}
	fseek(fp, 0, SEEK_SET);

	if(memcmp(buffer, JP2_RFC3745_MAGIC, 12) == 0 || memcmp(buffer, JP2_MAGIC, 4) == 0)
	{
		return OPJ_CODEC_JP2;
	}
	else if(memcmp(buffer, J2K_CODESTREAM_MAGIC, 4) == 0)
	{
		return OPJ_CODEC_J2K;
	}

	// TODO: OPJ_CODEC_JPT? OPJ_CODEC_JPP? OPJ_CODEC_JPX? OPJ_CODEC_JPH?

	return -1;
}

/**
 * Calculate rowstride for image
 */
int util_rowstride(opj_image_t *image, int comps_needed)
{
	return image->comps[0].w * comps_needed;
	// return ((image->comps[0].w * comps_needed * image->comps[0].prec + 7U) / 8U);
}

/**
 * Gets actual 8bit data out of the image; R, G, B, or A.
 */
int util_get(opj_image_t *image, int component, int index, int adjust)
{;
	return image->comps[component].data[index] + adjust;
}

/**
 * Clamp value between 0 and max
 */
int util_clamp(int value, int max)
{
	if(value > max)
	{
		value = max;
	}
	else if(value < 0)
	{
		value = 0;
	}

	return value;
}

// Debug functions

#if DEBUG == TRUE
	static void error_callback(const char *msg, void *client_data)
	{
		(void)client_data;
		g_print("[ERROR] %s", msg);
	}

	static void warning_callback(const char *msg, void *client_data)
	{
		(void)client_data;
		g_print("[WARNING] %s", msg);
	}

	static void info_callback(const char *msg, void *client_data)
	{
		(void)client_data;
		g_print("[INFO] %s", msg);
	}
#endif

