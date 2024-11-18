/*
  Copyright (c) 2012 Björn Esser

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
  SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


/*-/

  File:		YPath.h

  Author:	Björn Esser <bjoern.esser@gmail.com>

/-*/

#ifndef YPath_h
#define YPath_h

#include <string>
#include <vector>

/**
 * Finds files (e.g. plugins or theme pixmaps) recursively inside
 * a directory.
 **/
class YPath
{

public:

    /**
     * Constructor
     *
     * to be called with the directory where to look inside
     * and filename which to lookup.
     *
     * YSettings::progSubDir will be preferred by the lookup.
     **/
    YPath( const std::string & directory,
           const std::string & filename );
    /**
     * Destructor
     **/
    ~YPath();

    /**
     * Returns the full path of the file if found;
     * if not found just the filename given in constructor.
     **/
    std::string path();

    /**
     * Returns the directory where the file is found;
     * if not found just the subdir part (if there's any) of
     * the filename given in constructor.
     **/
    std::string dir();

private:

    std::vector<std::string> lsDir( const std::string & directory );
    std::string lookRecursive( const std::string & directory, const std::string & filename );
    std::string fullPath;

};

#endif // YUIDIRFINDER_H
