minimal = executable('minimal', 'minimal.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
basic = executable('basic', 'basic.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
relax = executable('relax', 'relax.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
codestream = executable('codestream', 'codestream.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
codestream_mono = executable('codestream_mono', 'codestream_mono.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
codestream_color = executable('codestream_color', 'codestream_color.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
advanced = executable('advanced', 'advanced.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
complex = executable('complex', 'complex.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
normal = executable('normal', 'normal.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
large = executable('large', 'large.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
cmyk = executable('cmyk', 'cmyk.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
save = executable('save', 'save.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])

loaders_data = configuration_data()
loaders_data.set('MODULE_PATH', pixbuf_loader_openjpeg.full_path())
loaders = configure_file(
    input: 'loaders.cache.in',
    output: 'loaders.cache',
    configuration: loaders_data,
)

test(
    'minimal',
    minimal,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/minimal.jp2',
    ],
)

test(
    'basic',
    basic,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/basic.jp2',
    ],
)

test(
    'relax',
    relax,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/relax.jp2',
    ],
)

test(
    'codestream',
    codestream,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/codestream.j2k',
    ],
)

test(
    'codestream_mono',
    codestream_mono,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/codestream_mono.j2c',
    ],
)

test(
    'codestream_color',
    codestream_color,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/codestream_color.j2c',
    ],
)

test(
    'advanced',
    advanced,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/advanced.jp2',
    ],
)

test(
    'complex',
    complex,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/complex.jp2',
    ],
)

test(
    'normal',
    normal,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/normal.jp2',
    ],
)

test(
    'large',
    large,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/large.jpf',
    ],
)

test(
    'cmyk',
    cmyk,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/cmyk.jp2',
    ],
)

test(
    'save',
    save,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/normal.jp2',
    ],
)
