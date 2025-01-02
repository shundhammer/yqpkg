# YQPkg - Standalone Qt Package Selector

This is a fork of the YaST Qt package selector as a standalone Qt program
without any YaST dependencies. It works directly with libzypp.


## License

GPL V2; see file LICENSE.


## Development Status

Alpha.

It is usable, and it has most of the key features.

- You can run it as root with all functions, and as a normal user in read-only mode.

- You can select packages and patterns for installation / update / removal.

- It does dependency resolution.

- You can commit the changes, and packages or patterns are actually
being installed / updated / removed.

- You get a plain moving progress bar on the _commit_ page while the
  transactions are performed.

- You can switch details on or off on the _commit_ page.

- You get a summary page from where you can go back to select more.

- There is a configurable countdown on the summary page to close the program
  automatically. And a "Stop" button to halt the countdown, of course.

Limitations:

- ~~No repo refresh yet. If needed, use `sudo zypper ref` before program start.~~

  _Repo refresh is now implemented; now even with progress feedback.__

- No GPG key import from repos, no failed package GPG signature fail prompt.

- No repo operations like adding, removing, setting priority, enabling, disabling.
  You get the repo view, though, where you can view and select packages by repo.

For details and screenshots, see [issue #1](https://github.com/shundhammer/yqpkg/issues/1).


### Caveat

**_There be dragons._** And bugs.

Nothing bad should happen, but don't use it on a critical production system for now.

_You've been warned._


## The Name: YQPkg

_(Pronounced Y-Q-Package)_

YQPkg is just a working title. Suggestions for a better one are welcome; see the
[naming suggestions issue](https://github.com/shundhammer/yqpkg/issues/19).


## Motivation: Why?

_Explained in greater detail at the Hack Week 11/2024
[YQPkg project page](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)_

YaST will be phased out soon in favor of Agama and Cockpit, and then there will
be a huge gap between low-level `zypper in` and high-level application
installers of the desktop environments; those know and promote their own
desktop's video post-production software, but what about other packages?

How do you browse through all the existing packages of a large distro like
Tumbleweed or Leap? How do you find software about some specific topic?

How do you install a different kernel version from a specific repo when your
new laptop doesn't want to suspend?

How do you help a friend who asks you what software openSUSE has to offer for
his HAM radio or his music band?

How do you find language-specific packages for your native Estonian?

Right now, you use YaST sw_single. What if there is no YaST anymore? That's
where this project comes in.


## Is this an Official SUSE Project?

Not yet. It started as a one-man project during the SUSE Hack Week in 11/2024,
and it's been a mixture of private Open Source project and a SUSE-sponsored one
at the same time.

Hopefully SUSE will recognize the importance of this and keep sponsoring it.


## Build Requirements

You need at least a C++ and Qt5 development environment plus CMake and
libzypp-devel.

```
sudo zypper install -t pattern devel_C_C++ devel_qt5
sudo zypper install cmake libzypp-devel libqt5-qtbase-devel
```


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


## Reference

- Development status and screenshots: [issue #1](https://github.com/shundhammer/yqpkg/issues/1)

- [Original project](https://github.com/libyui/libyui) that this was forked from
- [2024/11 SUSE Hack Week Project](https://hackweek.opensuse.org/24/projects/yqpkg-bringing-the-single-package-selection-back-to-life)
