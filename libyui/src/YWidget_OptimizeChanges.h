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

  File:		YWidget_OptimizeChanges.h

  Author:	Michael Andres <ma@suse.de>

/-*/

#ifndef YWidget_OptimizeChanges_h
#define YWidget_OptimizeChanges_h


// Moved out of YWidget.h because it's really utterly out of place there.

#ifndef YWidget_h
#error Do not include this file from anywhere outside YWidget.h!
#endif

// class YWidget
// {

public:
    /**
     * Helper class that calls startMultipleChanges() in its constructor
     * and cares about the necessary call to doneMultipleChanges() when it goes
     * out of scope.
     **/
    class OptimizeChanges
    {
    public:
	OptimizeChanges( YWidget & w ) : yw(w)	{ yw.startMultipleChanges(); }
	~OptimizeChanges()			{ yw.doneMultipleChanges();  }

    private:
	OptimizeChanges( const OptimizeChanges & ); // no copy
	void operator=( const OptimizeChanges & );  // no assign
	YWidget & yw;
    };

// }; // class YWidget


#endif // YWidget_OptimizeChanges_h
