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

  File:		YSquash.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YSquash_h
#define YSquash_h

#include "YSingleChildContainerWidget.h"
#include "ImplPtr.h"


class YSquashPrivate;

/**
 * HSquash, VSquash HVSquash: reduce child to its preferred size.
 *
 * Squash is a widget that "squashes" its one child during layout, i.e., it
 * reduces it in size down to its preferred size. It may squash vertically,
 * horizontally or in both dimensions.
 **/
class YSquash : public YSingleChildContainerWidget
{
protected:
    /**
     * Constructor.
     *
     * Squashes horizontally if 'horSquash' is 'true',
     * vertically if 'vertSquash' is 'true'.
     **/
    YSquash( YWidget * parent, bool horSquash, bool vertSquash );

public:
    /**
     * Destructor.
     **/
    virtual ~YSquash();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const;

    /**
     * Returns 'true' if this widget squashes horizontally.
     **/
    bool horSquash() const;

    /**
     * Returns 'true' if this widget squashes vertically.
     **/
    bool vertSquash() const;


    /**
     * In a squashed dimension the widget NOT stretchable.
     * In an unsquashed dimension the widget is stretchable if the
     * child is stretchable.
     **/
    bool stretchable( YUIDimension dim ) const;

private:

    ImplPtr<YSquashPrivate> priv;
};


#endif // YSquash_h
