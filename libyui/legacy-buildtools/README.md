# Support for the Legacy Libyui CMake Environment

The contents of this directory are installed to /usr/share/libyui/buildtools to
support existing community-supported libyui plugins and extensions.

This used to be the starting point of the previous libyui build system where a
toplevel _bootstrap.h_ script created a symlink

    CMakeLists.txt -> /usr/share/libyui/buildtools/CMakeLists.common

as a starting point. That toplevel CMakeLists.txt also included a number of
files from this directory.


## Migration Path

We will need this until all the community-supported libyui plugins and
extensions (libyui-gtk, libyui-mga, libyui-mga-qt, ...) are ported to the
new cmake environment.

See

  https://gist.github.com/shundhammer/d96a63a1cab8245d20c2e3556238a114


## Notice

When this is removed, remember to also remove it from the files list in the
.spec file!
