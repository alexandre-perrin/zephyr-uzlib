# Zephyr uzlib

This repository bridges [uzlib](https://github.com/pfalcon/uzlib)
to Zephyr as a standalone module.
Upstream sources remain unmodified in the `uzlib/` Git submodule.
The root `CMakeLists.txt` and `Kconfig` provide the integration,
registered by `zephyr/module.yml`.

## Add to a west workspace

Add this repository to the application manifest
with submodule updates enabled:

```yaml
manifest:
  projects:
    - name: zephyr-uzlib
      url: https://github.com/alexandre-perrin/zephyr-uzlib
      revision: main
      path: modules/lib/zephyr-uzlib
      submodules: true
```

Then run `west update` to fetch the bridge
and initialize its upstream submodule.

## Configuration

Enable `CONFIG_UZLIB` in the application configuration.
Compression is enabled by default with `CONFIG_UZLIB_COMPRESS`.
Enable `CONFIG_UZLIB_DECOMPRESS` for decompression.
Include `<uzlib.h>` to use the upstream API.

For standalone Git checkouts,
run `git submodule update --init --recursive` first.

## Tests

From a configured Zephyr workspace,
run the decompression test with Twister:

```sh
west twister -p native_sim -T tests
```

Twister discovers the test under `tests`.
