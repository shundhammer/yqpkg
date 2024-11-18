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


/**
 @file		YTypes.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

  Header file for frequently used simple types to reduce interdependencies
  between important headers (e.g., YWidget.h, YUI.h).

**/


#ifndef YTypes_h
#define YTypes_h

#include <list>

typedef double		YLayoutSize_t;
typedef long long	YFileSize_t;

class YWidget;

typedef std::list<YWidget *>				YWidgetList;
typedef std::list<YWidget *>::iterator			YWidgetListIterator;
typedef std::list<YWidget *>::const_iterator		YWidgetListConstIterator;
typedef std::list<YWidget *>::reverse_iterator		YWidgetListReverseIterator;
typedef std::list<YWidget *>::const_reverse_iterator	YWidgetListConstReverseIterator;


#define YUIAllDimensions	2

enum YUIDimension
{
    YD_HORIZ	= 0,
    YD_VERT	= 1
};


enum YAlignmentType
{
    YAlignUnchanged,
    YAlignBegin,
    YAlignEnd,
    YAlignCenter
};


/**
 * Type of dialog: Main / Popup / Wizard.
 **/
enum YDialogType
{
    YMainDialog,
    YPopupDialog,
    YWizardDialog,
};


enum YDialogColorMode
{
    YDialogNormalColor,	// Default
    YDialogInfoColor,	// Brighter colors
    YDialogWarnColor	// Very bright Warning colors
};


enum YButtonRole
{
    YCustomButton = 0,	// No predefined role
    YOKButton,		// [OK], [Continue], [Yes], [Accept], [Next]
    YApplyButton,	// [Apply]
    YCancelButton,	// [Cancel]
    YHelpButton,	// [Help]
    YRelNotesButton,	// [Release Notes]

    YMaxButtonRole	// For use as array size
};


enum YButtonOrder
{
    YKDEButtonOrder,	// [OK] [Apply] [Cancel] [Custom1] [Custom2] [Help]
			// [Yes] [No]
			// [Continue] [Cancel]

    YGnomeButtonOrder	// [Help] [Custom1] [Custom2] [Apply] [Cancel] [OK]
			// [No] [Yes]
			// [Cancel] [Continue]
};



#endif // YTypes_h
