# YQPkg - Standalone Qt Package Selector

This is a fork of the YaST Qt package selector as a standalone Qt program
without any YaST dependencies. It works directly with libzypp.


## License

GPL V2; see file LICENSE.


## Development Status

Late Alpha.

It is now well usable and fairly feature complete.


## Features

### Features Taken over from YaST

- Repos are automatically refreshed at the start of the program with some
  visual feedback what is going on.

- Search for packages
  - by name, summary, description
  - by RPM provides and requires
  - in their file list (installed packages only)

- See package details:
  - Package description
  - Technical details (`rpm -qi`)
  - Dependencies (RPM provides, pre-requires requires)
  - Versions (installed and from all active repositories)
  - File list (installed packages only)
  - Change log (installed packages only)

- Install, update and remove packages
- Pick a specific package version for installation or update
- Install and remove patterns
- Browse and select packages by repository
- Browse and select language/locale-specific packages by language/locale
- Browse and select packages by classification:
  - Suggested packages
  - Recommended packages
  - Orphaned packages
  - Unneeded packages
  - Retracted packages
  - Retracted installed packages
  - All packages


### New Features in YQPkg (No YaST Counterpart)

- "Updates" view that shows packages that could be updated (where a newer
  version than the installed one is available)

- Update all packages that can be updated without a dependency problem (the
  same as `zypper up`)

- Dist Upgrade (the same as `zypper dup`): Take package splits and renames,
  pattern updates and more into account and update all packages that can be
  updated without a dependency problem

- **Read-only mode** for non-root users: You can search packages, view their
  details, even experiment what would happen if you tried to install or remove
  them and see the dependency resolver results.

  You can do everything except actually apply any changes.

- During the commit phase (where packages are actually installed, updated or
  removed), inaddition to the large progress bar, you can now also switch to a
  details view to see which packages are waiting, downloaded, being processed,
  or finished.

- You can decide during the commit phase whether or not you want to see a
  summary page as the next step. That summary page exits the program after a
  selectable countdown (30 seconds by default), but you can go back to the
  package selector to mark more packages for installation, update or removal.

  It's now very easy to keep working with the package selection and install,
  update or remove a few packages at a time and then do some more without fear
  that a lot of work might be lost when you get into a large dependency problem
  cycle.


### Fixes

- Improved performance during startup as well as during package selection.

- Improved visual appearance; we don't need to "pixel pinch" anymore with
  today's screen resolutions, unlike back when the YaST package selector
  created.

- The annoying tendency to get too narrow columns in the package list is now
  fixed. You will find yourself very rarely adjusting the column widths
  manually.

- The package list is now filled automatically (as it should always have) when
  appropriate; you don't need to click on the left "filter" pane anymore to
  make that happen. See also [issue #10](https://github.com/shundhammer/yqpkg/issues/10).

- Similar with the details views (bottom right): They are now shown
  automatically when a package is selected, be it manually or
  programmatically. Previously, you had to click on the automatically selected
  package again to see anything there. This is also explained in
  [issue #10](https://github.com/shundhammer/yqpkg/issues/10).

- Reasonable initial window sizes for the main window as well as for the many
  pop-up dialogs.



## Limitations

- No GPG key import from repos yet for repos that were just added and never
  refreshed yet.

- No repo operations like adding, removing, setting priority, enabling, disabling.
  You get the repo view, though, where you can view and select packages by repo.


### Work in Progress

- Patches view. You can already use it if you hit the `[F2]` button (like in
  all YaST versions since time immemorial), but it may still have some quirks
  and bugs.

  But since SUSE patches are only (mostly?) a collection of updated package
  versions, the new "Updates" view and the "Package Update" button there is a
  very adequate replacement. And of course only Leap has patches anyway;
  Tumbleweed and Slowroll don't have any.



## Development Progress and Screenshots

See [issue #1](https://github.com/shundhammer/yqpkg/issues/1).

Scroll down all the way to see the latest news and screenshots.


## Stability

It's quite stable and usable now.

Of course, since it's still in development, the occasional bug may appear, but
it should now only be minor ones. See also the
[open issues](https://github.com/shundhammer/yqpkg/issues)
in the GitHub issue tracker.

I have been using it exclusively since Christmas 2024 for all my package
operations, including keeping my Slowroll laptop, my Leap 15.6 desktop PC and
my Tumbleweed virtual machine up to date. There was _never_ real problem,
especially not during the commit phase when packages are actually installed,
updated or removed.


## Ready-Made Packages

- The latest version from Git [yqpkg-git](https://software.opensuse.org/download/package?package=yqpkg&project=home%3Ashundhammer%3Ayqpkg-git)


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
sudo zypper install cmake libzypp-devel
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
