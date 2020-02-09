minimal = executable('minimal', 'minimal.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])
basic = executable('basic', 'basic.c', link_with: pixbuf_loader_openjpeg, dependencies: [gdk_pixbuf, openjpeg])

loaders_data = configuration_data()
loaders_data.set('MODULE_PATH', pixbuf_loader_openjpeg.full_path())
loaders = configure_file(
    input: 'loaders.cache.in',
    output: 'loaders.cache',
    configuration: loaders_data,
)

test(
    'Load minimal image, 1 pixel',
    minimal,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/minimal.jp2',
    ],
)

test(
    'Load minimal image, 1 pixel',
    basic,
    env: [
        'GDK_PIXBUF_MODULE_FILE=' + meson.current_build_dir() + '/loaders.cache',
        'TEST_FILE=' + meson.current_source_dir() + '/basic.jp2',
    ],
)

