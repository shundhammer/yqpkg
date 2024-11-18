/*
  Copyright (c) [2019] SUSE LLC
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

  File:		YItemCustomStatus.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YItemCustomStatus_h
#define YItemCustomStatus_h

#include <string>
#include <vector>


/**
 * Class describing a non-binary status for an item.
 *
 * This is an extension of normal boolean item states: Rather than just "on" or
 * "off" like for check boxes or radio buttons, a status of this kind can have
 * any nonnegative integer number. The number is implicitly the index of this
 * status in the corresponding vector.
 *
 * For symmetry with boolean states, a value of 0 is defined to have the same
 * semantics as "off" / "unselected", 1 has the semantics of "on" / "fully
 * selected", and all other values are purely application-defined.
 **/
class YItemCustomStatus
{
public:
    /**
     * Constructor.
     **/
    YItemCustomStatus( const std::string & iconName,
                       const std::string & textIndicator,
                       int                 nextStatus = -1 )
        : _iconName( iconName )
        , _textIndicator( textIndicator )
        , _nextStatus( nextStatus )
        {}

    /**
     * The name of an icon to use in the widget in a graphical UI if an item
     * has this status.
     **/
    const std::string & iconName() const { return _iconName; }

    /**
     * A text representation of this status in a text-based UI if an item has
     * this status, for example "[ ]", "[x]" or "[  ], "[ +]", "[a+]".
     *
     * It is recommended to use the same character length for all states so all
     * items line up properly, even if they have different states.
     **/
    const std::string & textIndicator() const { return _textIndicator; }

    /**
     * This returns the next status to cycle through if the user clicks on the
     * status or cycles through status values with the corresponding shortcut
     * key.
     *
     * If no such value was specified, this returns -1. The application can
     * then still choose to set a different status depending on other
     * application data.
     **/
    int nextStatus() const { return _nextStatus; }

    /**
     * Set the next status. This should only be done once when the status
     * transition map is evaluated.
     **/
    void setNextStatus( int value ) { _nextStatus = value; }

    /**
     * Return 'true' if a next status to cylce to is defined for this status,
     * 'false' if not.
     **/
    bool hasNextStatus() const { return _nextStatus != -1; }

protected:

    std::string _iconName;
    std::string _textIndicator;
    int         _nextStatus;
};


typedef std::vector<YItemCustomStatus> YItemCustomStatusVector;


#endif  // YItemCustomStatus_h
