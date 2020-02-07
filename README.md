
# JPEG2000 Pixbuf Loader

This is a [GdkPixbuf](https://gitlab.gnome.org/GNOME/gdk-pixbuf) loader module for JPEG2000. It uses [OpenJPEG](https://github.com/uclouvain/openjpeg) to load JPEG2000 images as linear buffers in memory.

Note: by default there already some JPEG2000 support in GdkPixbuf, but that uses the [JasPer](https://github.com/mdadams/jasper) image library, which only supports certain parts of the massive JPEG2000 standard, meaning certain images don't work.

## Copying / License

Copyright © 2020 Nichlas Severinsen

This loader module is licensed under the GNU Lesser General Public License, version 2.1; [LGPLv2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html). also see [COPYING](COPYING)

OpenJPEG is licensed under the 2-clauses BSD License.



