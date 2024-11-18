/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:         YIconLoader.cc

  Author:       Katarína Machálková <kmachalkova@novell.com>

/-*/


#ifndef YIconLoader_H
#define YIconLoader_H

#include <string>
#include <list>

class YIconLoader
{
public:

    YIconLoader();
    ~YIconLoader();

    std::string findIcon( std::string name );

    // FIXME: these two are here for compatibility reasons.
    // Deprecate them in due course and treat base path just
    // like any other search path
    void setIconBasePath( std::string path );
    std::string iconBasePath() const;

    void addIconSearchPath( std::string path );

private:

    std::string                 _iconBasePath;
    std::list <std::string>	_iconDirs;

    bool fileExists( std::string fname );
};

#endif
