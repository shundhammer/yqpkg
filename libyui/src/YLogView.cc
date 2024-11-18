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

  File:         YLogView.cc

  Author:       Stefan Hundhammer <shundhammer@suse.de>

/-*/

#include <deque>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YLogView.h"

using std::string;

typedef std::deque<string>                      StringDeque;
typedef std::deque<string>::iterator            StringDequeIterator;
typedef std::deque<string>::const_iterator      StringDequeConstIterator;



struct YLogViewPrivate
{
    YLogViewPrivate( const string & label, int visibleLines, int maxLines )
        : label( label )
        , visibleLines( visibleLines )
        , maxLines( maxLines )
        {}

    string	label;
    int		visibleLines;
    int		maxLines;

    StringDeque logText;
};




YLogView::YLogView( YWidget * parent, const string & label, int visibleLines, int maxLines )
    : YWidget( parent )
    , priv( new YLogViewPrivate( label, visibleLines, maxLines ) )
{
    YUI_CHECK_NEW( priv );

    setDefaultStretchable( YD_HORIZ, true );
    setDefaultStretchable( YD_VERT,  true );
}


YLogView::~YLogView()
{
    // NOP
}


string
YLogView::label() const
{
    return priv->label;
}


void
YLogView::setLabel( const string & label )
{
    priv->label = label;
}


int
YLogView::visibleLines() const
{
    return priv->visibleLines;
}


void
YLogView::setVisibleLines( int newVisibleLines )
{
    priv->visibleLines = newVisibleLines;
}


int
YLogView::maxLines() const
{
    return priv->maxLines;
}


void
YLogView::setMaxLines( int newMaxLines )
{
    int linesToDelete = priv->maxLines - newMaxLines;
    priv->maxLines = newMaxLines;

    for ( int i=0; i < linesToDelete; i++ )
        priv->logText.pop_front();

    if ( linesToDelete > 0 )
	updateDisplay();
}


string
YLogView::logText() const
{
    string text;

    for ( StringDequeConstIterator it = priv->logText.begin();
          it != priv->logText.end();
          ++it )
    {
        text += *it;
    }

    if ( ! text.empty() )
    {
	// Cut off last newline

	if ( *(text.rbegin()) == '\n' ) // Last char is a newline?
	{
	    text.resize( text.size() - 1 ); // make one char shorter
	}
    }

    return text;
}


string
YLogView::lastLine() const
{
    if ( priv->logText.empty() )
        return "";
    else
        return priv->logText.back();
}


void
YLogView::appendLines( const string & newText )
{
    string			text	= newText;
    string::size_type	from	= 0;
    string::size_type	to	= 0;


    // Split the text into single lines

    while ( to < text.size() )
    {
        from = to;
        to   = text.find( '\n', from );
        if ( to == string::npos )               // no more newline
            to = text.size();
        else
            to++;                               // include the newline

        // Output one single line
        appendLine( text.substr( from, to - from ) );
    }

    if ( to < text.size() )             // anything left over?
    {
        // Output the rest
        appendLine( text.substr( to, text.size() - to ) );
    }

    updateDisplay();
}


void
YLogView::appendLine( const string & line )
{
    priv->logText.push_back( line );

    if ( maxLines() > 0 && priv->logText.size() > (unsigned) maxLines() )
    {
        priv->logText.pop_front();
    }
}

void
YLogView::setLogText(const string & text)
{
  // optimize for regular updating widget when no new content appear
  if (text == logText())
    return;

  // do not use clearText as it do render and cause segfault in qt (bnc#989155)
  priv->logText.clear();
  appendLines(text);
}


void
YLogView::clearText()
{
    priv->logText.clear();
    updateDisplay();
}


int YLogView::lines() const
{
    return priv->logText.size();
}


void
YLogView::updateDisplay()
{
    displayLogText( logText() );
}



const YPropertySet &
YLogView::propertySet()
{
    static YPropertySet propSet;

    if ( propSet.isEmpty() )
    {
        /*
         * @property string	Value		All log lines.
         * @property string	LastLine	The last log line(s). Use this to append lines.
         * @property integer	VisibleLines	Number of lines to display. Call RecalcLayout() afterwards.
         * @property integer	MaxLines	Number of lines to store (0 for all).
         * @property string	Label		Caption above the log text
         */
        propSet.add( YProperty( YUIProperty_Value,              YStringProperty ) );
        propSet.add( YProperty( YUIProperty_LastLine,           YStringProperty ) );
        propSet.add( YProperty( YUIProperty_VisibleLines,       YIntegerProperty ) );
        propSet.add( YProperty( YUIProperty_MaxLines,           YIntegerProperty ) );
        propSet.add( YProperty( YUIProperty_Label,              YStringProperty  ) );
        propSet.add( YWidget::propertySet() );
    }

    return propSet;
}


bool
YLogView::setProperty( const string & propertyName, const YPropertyValue & val )
{
    propertySet().check( propertyName, val.type() ); // throws exceptions if not found or type mismatch

    if      ( propertyName == YUIProperty_Value         )       setLogText	( val.stringVal()  );
    else if ( propertyName == YUIProperty_LastLine      )       appendLines     ( val.stringVal()  );
    else if ( propertyName == YUIProperty_VisibleLines  )       setVisibleLines ( val.integerVal() );
    else if ( propertyName == YUIProperty_MaxLines      )       setMaxLines     ( val.integerVal() );
    else if ( propertyName == YUIProperty_Label         )       setLabel        ( val.stringVal()  );
    else
    {
        return YWidget::setProperty( propertyName, val );
    }

    return true; // success -- no special processing necessary
}


YPropertyValue
YLogView::getProperty( const string & propertyName )
{
    propertySet().check( propertyName ); // throws exceptions if not found

    if      ( propertyName == YUIProperty_Value         )       return YPropertyValue( logText()      );
    if      ( propertyName == YUIProperty_LastLine      )       return YPropertyValue( lastLine()     );
    if      ( propertyName == YUIProperty_VisibleLines  )       return YPropertyValue( visibleLines() );
    if      ( propertyName == YUIProperty_MaxLines      )       return YPropertyValue( maxLines()     );
    else if ( propertyName == YUIProperty_Label         )       return YPropertyValue( label()        );
    else
    {
        return YWidget::getProperty( propertyName );
    }
}
