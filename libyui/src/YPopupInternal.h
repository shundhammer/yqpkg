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

#include <string>
#include <vector>

// Internal helper class for YDialogSpy and YPropertyEditor
class YPopupInternal
{
public:

    /**
     * Display a simple popup dialog with OK button
     * @param label the message to display
     */
    static void message(const std::string &label);

    typedef std::vector<std::string> StringArray;

    /**
     * Display a popup dialog with several input fields
     * @param  array fields to edit
     * @param  label title of the dialog
     * @return true if dialog was closed by [OK], false otherwise
     */
    static bool editStringArray(StringArray &array, const std::string &label);

    /**
     * Display a popup dialog with 3 initially empty input fields
     * @param  label title of the dialog
     * @return Entered values in a StringArray, if canceled the array is empty.
     */
    static StringArray editNewStringArray(const std::string &label);
};
