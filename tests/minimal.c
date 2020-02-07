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

#include <gdk-pixbuf/gdk-pixbuf.h>

gint main(gint argc, gchar **argv)
{
    GError *error = NULL;
    gchar **env = g_get_environ();

    g_warning("%s", g_environ_getenv(env, "TEST_FILE"));
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(g_environ_getenv(env, "TEST_FILE"), &error);

    if(error)
    {
        g_error("%s", error->message);
    }

    g_assert(error == NULL);

    g_assert(gdk_pixbuf_get_width(pixbuf) == 1);

    g_assert(gdk_pixbuf_get_height(pixbuf) == 1);

    g_strfreev(env);

    g_object_unref(pixbuf);

    return 0;
}
