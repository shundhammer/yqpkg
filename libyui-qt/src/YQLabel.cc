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

  File:	      YQLabel.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <qlabel.h>

#define YUILogComponent "qt-ui"
#include <yui/YUILog.h>

#include "utf8.h"
#include "YQUI.h"
#include "YQApplication.h"
#include "YQLabel.h"

#define AUTO_WRAP_WIDTH  150
#define AUTO_WRAP_HEIGHT  10

using std::string;



YQLabel::YQLabel( YWidget * 		parent,
		  const string &	text,
		  bool 			isHeading,
		  bool 			isOutputField )
    : QLabel( (QWidget *) parent->widgetRep() )
    , YLabel( parent, text, isHeading, isOutputField )
    , _layoutPass1Width( 0 )
{
    setWidgetRep( this );

    setTextInteractionFlags( Qt::TextSelectableByMouse );
    setTextFormat( Qt::PlainText );
    QLabel::setText( fromUTF8( text ) );
    setIndent(0);

    if ( isHeading )
    {
	setFont( YQUI::yqApp()->headingFont() );
    }
    else if ( isOutputField )
    {
	setFrameStyle ( QFrame::Panel | QFrame::Sunken );
	setLineWidth(2);
	setMidLineWidth(2);
    }

    setMargin( YQWidgetMargin );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );
}


YQLabel::~YQLabel()
{
    // NOP
}


void YQLabel::setText( const string & newText )
{
    YLabel::setText( newText );
    QLabel::setText( fromUTF8( newText ) );
}


void YQLabel::setUseBoldFont( bool useBold )
{
    setFont( useBold ?
	     YQUI::yqApp()->boldFont() :
	     YQUI::yqApp()->currentFont() );

    YLabel::setUseBoldFont( useBold );
}


void YQLabel::setAutoWrap( bool autoWrap )
{
    YLabel::setAutoWrap( autoWrap );
    QLabel::setWordWrap( autoWrap );
}


void YQLabel::setEnabled( bool enabled )
{
    QLabel::setEnabled( enabled );
    YWidget::setEnabled( enabled );
}


int YQLabel::preferredWidth()
{
    int width;

    if ( autoWrap() )
    {
        if ( layoutPass() == 2 )
        {
            // Use the width passed down to us from the parent layout manager
            // in the last setSize() call. This is the definitive width that
            // will be used after taking all other children of the layout into
            // consideration, also including making widgets smaller due to size
            // restrictions, or redistributing excess space.
            //
            // Since this widget can auto-wrap its contents, we accept
            // basically any width; we just need to adapt the preferred height
            // accordingly.
            width = _layoutPass1Width;
        }
        else
        {
            // Use a preliminary width. Typically, this widget should be
            // wrapped into a MinSize or MinWidth which hopefully gives us a
            // much more useful width than this.
            //
            // We would also just use 0 here, but that would make debugging
            // really hard since the widget might completly disappear.
            //
            // The real width that will be used will be set in the setSize()
            // call following the recursive preferredWidth() /
            // preferredHeight() calls in the widget tree.
            width = AUTO_WRAP_WIDTH;
        }
    }
    else  // ! autoWrap()
    {
        width = sizeHint().width();
    }

    return width;
}


int YQLabel::preferredHeight()
{
    int height;

    if ( autoWrap() )
    {
        if ( layoutPass() == 2 )
        {
            // This is where the magic happens:
            //
            // setSize() in the first layout pass gave us the real width which
            // we stored in _layoutPass1Width. We can now calculate the height
            // that is really needed based on that width. QLabel provides this
            // handy function that takes word wrapping and font metrics into
            // account (remember, we are using a proportional font, so every
            // letter has a different width).

            height = heightForWidth( _layoutPass1Width );
        }
        else
        {
            height = AUTO_WRAP_HEIGHT;
        }
    }
    else  // ! autoWrap()
    {
        height = sizeHint().height();
    }

    return height;
}


void YQLabel::setSize( int newWidth, int newHeight )
{
    if ( autoWrap() )
    {
        _layoutPass1Width = layoutPass() == 1 ?
            newWidth : 0;
    }

    resize( newWidth, newHeight );
}
