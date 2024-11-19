# YQPkg - Standalone Qt Package Selector

This is a fork of the YaST Qt package selector as a standalone Qt program
without any YaST dependencies.


## License

GPL V2; see file LICENSE.


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
src/
```

After `sudo make install`:

```
y2pkg
```

(it's now in `/usr/bin/yqpkg`)


## TO DO

_TO DO: More information_


## Reference

- [Original project](https://github.com/libyui/libyui) that this was forked from
- [2024/11 SUSE Hack Week Project](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)
