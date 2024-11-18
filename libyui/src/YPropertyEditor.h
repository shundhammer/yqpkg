/*
  Copyright (C) 2016 SUSE LLC

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

#ifndef YPropertyEditor_h
#define YPropertyEditor_h

#include <string>

#include "ImplPtr.h"

class YWidget;
class YPropertyEditorPriv;

/**
 * An internal helper class for displaying the widget property editor
 * in the spy dialog.
 * @see YDialogSpy
 */
class YPropertyEditor
{
public:
    /**
     * Constructor
     * @param  widget the target widget
     */
    YPropertyEditor( YWidget * widget );

    /**
     * Destructor
     **/
    virtual ~YPropertyEditor();

    /**
     * Display a popup for editing a widget property.
     * @param  property name of the property to edit
     * @return true if the property has been changed, false otherwise
     */
    bool edit( const std::string & property );

private:

    ImplPtr<YPropertyEditorPriv> priv;
};

#endif
