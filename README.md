# YQPkg - Standalone Qt Package Selector

This is a fork of the YaST Qt package selector as a standalone Qt program
without any YaST dependencies. It works directly with libzypp.


## License

GPL V2; see file LICENSE.


## Development Status

Pre-alpha.

For details and screenshots, see [issue #1](https://github.com/shundhammer/yqpkg/issues/1).


## Building

```
make -f Makefile.repo
cd build
make
```

### Installing

After building, add

```
sudo make install
```

### Cleaning up a Build

```
rm -rf build
```

### Starting the Built Executable

From the build directory (without `sudo make install`):

```
cd build
src/yqpkg
```

After `sudo make install`:

```
yqpkg
```

(it's now in `/usr/bin/yqpkg`)


## TO DO

_TO DO: More information_


## Reference

- Development status and screenshots: [issue #1](https://github.com/shundhammer/yqpkg/issues/1)

- [Original project](https://github.com/libyui/libyui) that this was forked from
- [2024/11 SUSE Hack Week Project](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)
