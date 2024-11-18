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

  File:		YBuiltinCaller.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YBuiltinCaller_h
#define YBuiltinCaller_h


/**
 * Abstract base class for transparently calling a built-in function.
 * Derived classes will want to add some methods to store the function to be
 * called, arguments to that function and its result and to retrieve the result
 * when needed.
 *
 * See YCPBuiltinCaller.h for an implementation.
 **/
class YBuiltinCaller
{
protected:

    YBuiltinCaller() {}

public:

    virtual ~YBuiltinCaller() {}

    /**
     * Call the built-in. This will be called in the UI thread with appropriate
     * syncing between the threads.
     *
     * Derived classes might want to store the result of the call in a member
     * variable in this class so it can later be queried.
     *
     * Derived classes are required to implement this method.
     **/
    virtual void call() = 0;
};


#endif // YBuiltinCaller_h
