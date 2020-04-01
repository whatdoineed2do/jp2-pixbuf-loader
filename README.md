
# JPEG2000 Pixbuf Loader

This is a [GdkPixbuf](https://gitlab.gnome.org/GNOME/gdk-pixbuf) loader module for JPEG2000. It uses [OpenJPEG](https://github.com/uclouvain/openjpeg) to load JPEG2000 images as linear buffers in memory.

Note: by default there's already some JPEG2000 support in GdkPixbuf, but that uses the [JasPer](https://github.com/mdadams/jasper) image library, which only support certain parts of the JPEG2000 standard, meaning certain images don't work.

## Installing

### Option 1. Compiling

```
meson build && cd build && ninja install
```

### Option 2. Arch linux / AUR:

```
sudo aura -A jp2-pixbuf-loader -x
```

## Copying / License

Copyright © 2020 Nichlas Severinsen

This loader module is licensed under the GNU Lesser General Public License, version 2.1; [LGPLv2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html). also see [COPYING](COPYING)

OpenJPEG is licensed under the 2-clauses BSD License.

## Development

- [GdkPixbuf Reference Manual](https://developer.gnome.org/gdk-pixbuf/)
- [Examples](https://gitlab.gnome.org/GNOME/gdk-pixbuf/tree/master/gdk-pixbuf)

### Debug

Build project then generate loaders.cache.in:

```
gdk-pixbuf-query-loaders libpixbufloader-jp2.so > ../tests/loaders.cache.in
```

Run tests:

`ninja test` or `meson test --print-errorlogs`

Run tests with gdb:

```
meson test --gdb
```

Run tests with valgrind:

```
meson test --wrap='valgrind --leak-check=full'
```

Run a specific test:

```
meson test basic --print-errorlogs
```

#### Todo

- Better tests
- Tests and testfiles for sYCC422 and sYCC444
- Implement image_save;
- Implement image_stop_load;
- Implement image_begin_load;
- Implement image_load_increment;
- Implement image_save_to_callback;

