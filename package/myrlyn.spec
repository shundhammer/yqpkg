#
# spec file for package myrlyn
#
# Copyright (c) 2025 SUSE LLC
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           myrlyn

# If you change the version here, don't forget ../VERSION.cmake !
Version:        0.8.02
Release:        0

%define         libzypp_devel_version libzypp-devel >= 17.21.0

BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.17
BuildRequires:  gcc-c++

# Workaround for boost issue, see boo#1225861
# FIXME: Is this still needed?
BuildRequires:  gcc-fortran

BuildRequires:  pkg-config
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  %{libzypp_devel_version}

# xdg-su
Requires:       xdg-utils

Summary:        Graphical Software Package Manager
License:        GPL-2.0
URL:            http://github.com/shundhammer/myrlyn
Source:         %{name}-%{version}.tar.bz2


%description
Myrlyn is a graphical package manager to select software packages and patterns
for installation, update and removal. It uses libzypp as its backend and Qt
as its GUI toolkit.

This started in the 11/2024 SUSE Hack Week to make the SUSE YaST Qt package
selector usable as a standalone Qt program without any YaST dependencies.


%prep
%setup -q -n %{name}-%{version}

%build
mkdir build
pushd build

export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG $(getconf LFS_CFLAGS)"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG $(getconf LFS_CFLAGS)"

%if %{?_with_debug:1}%{!?_with_debug:0}
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELWITHDEBINFO"
%else
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELEASE"
%endif

cmake .. \
 $CMAKE_OPTS

make %{?jobs:-j%jobs}
popd

%install
pushd build
make install DESTDIR="$RPM_BUILD_ROOT"
popd


%files
%{_bindir}/myr
### %{_datadir}/applications/myrlin-root.desktop
### %{_datadir}/applications/myrlin-user.desktop
### %dir %{_datadir}/icons/hicolor
### %dir %{_datadir}/icons/hicolor/32x32
### %dir %{_datadir}/icons/hicolor/32x32/apps
### %{_datadir}/icons/hicolor/32x32/apps/myrlin.png
### %license %{_docdir}/%{name}/LICENSE
### %doc README.md
# %doc doc/*.txt doc/*.md
%defattr(-,root,root)

%changelog
