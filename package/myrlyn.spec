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


%define         libzypp_devel_version libzypp-devel >= 17.21.0
Name:           myrlyn
# If you change the version here, don't forget ../VERSION.cmake !
# To increase only the last number, use  rake version:bump
Version:        0.8.02
Release:        0
Summary:        Myrlyn Linux Package Manager GUI
License:        GPL-2.0-only
Group:          System/Packages
URL:            https://github.com/shundhammer/myrlyn
Source:         %{name}-%{version}.tar.zst
BuildRequires:  %{libzypp_devel_version}
BuildRequires:  ImageMagick
BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.17
BuildRequires:  hicolor-icon-theme
# Workaround for boost issue, see boo#1225861
# FIXME: Is this still needed (gcc-fortran)?
BuildRequires:  gcc-fortran
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)

%if 0%{?suse_version} < 1600
BuildRequires:  gcc13
BuildRequires:  gcc13-c++
%else
BuildRequires:  gcc
BuildRequires:  gcc-c++
%endif
Requires:	xdg-utils

%description
Myrlyn is a graphical package manager to select software packages and patterns for installation, update and removal. It uses libzypp as its backend and Qt as its GUI toolkit.

%prep
%autosetup -p1

%build
%if 0%{?suse_version} < 1600
export CC=gcc-13
export CXX=g++-13
%endif
export CFLAGS="%{optflags} -fPIE"
export CXXFLAGS="%{optflags} -fPIE"
export LDFLAGS="-pie"

%if %{?_with_debug:1}%{!?_with_debug:0}
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELWITHDEBINFO"
%else
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=RELEASE"
%endif

%cmake \
 $CMAKE_OPTS
%cmake_build

%install
%cmake_install

%files
%doc README.md Contributing.md
%license LICENSE
%{_bindir}/myrlyn
%{_datadir}/applications/%{name}-*.desktop
%{_datadir}/icons/hicolor/*/apps/Myrlyn.png

%changelog
