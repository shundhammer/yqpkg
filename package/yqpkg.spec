#
# spec file for package yqpkg
#
# Copyright (c) 2021 SUSE LLC
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


Name:           yqpkg

# If you change the version here, don't forget ../VERSION.cmake !
# To increase only the last number, use  rake version:bump
Version:        0.7.0
Release:        0

%define         libzypp_devel_version libzypp-devel >= 17.21.0

BuildRequires:  boost-devel
BuildRequires:  cmake >= 3.17
BuildRequires:  gcc-c++

# Workaround for boost issue, see boo#1225861
# FIXME: Is this still needed?
BuildRequires:  gcc-fortran

BuildRequires:  libboost_test-devel
BuildRequires:  pkg-config

BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5X11Extras)
BuildRequires:  %{libzypp_devel_version}

Summary:        GUI abstraction library
License:        GPL-2.0
URL:            http://github.com/shundhammer/yqpkg
Source:         %{name}-%{version}.tar.bz2

%description
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can also be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.


%package -n %{bin_name}
Summary:        yqpkg - Package selector as a standalone Qt program


%description -n %{bin_name}
This is the (formerly YaST) Qt package selector as a standalone program
without any YaST dependencies, just Qt and libzypp.

This is a fork of the YQPackageSelector from the YaST libyui / libyui-qt-pkg.


%prep
%setup -q -n %{name}-%{version}

%build
pushd %{name}
mkdir build
cd build

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
pushd %{name}
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
popd

%files -n %{bin_name}
%defattr(-,root,root)
%{_bindir}/yqpkg
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/LICENSE*

%changelog
