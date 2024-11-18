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

  File:	      YQWidgetCaption.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQWidgetCaption_h
#define YQWidgetCaption_h

#include <qlabel.h>
#include <string>
using std::string;


/**
 * Helper class for captions (labels) above a widget: Takes care of hiding
 * itself when its text is empty and showing when its text becomes non-empty.
 **/
class YQWidgetCaption: public QLabel
{
    Q_OBJECT

public:

    /**
     * Constuctors.
     *
     * If the text is empty, the widget will be created, but hidden right
     * away. It can later be made visible again with setText() with a non-empty
     * text.
     **/
    YQWidgetCaption( QWidget * parent, const std::string  & text );
    YQWidgetCaption( QWidget * parent, const QString &      text );

    /**
     * Destructor.
     **/
    virtual ~YQWidgetCaption();

    /**
     * Change the text and handle visibility:
     * If the new text is empty, hide this widget.
     * If the new text is non-empty, show this widget.
     **/
    virtual void setText ( const std::string  & newText );
    virtual void setText ( const QString &      newText );

private:

    /**
     * Make the widget visible if the text is non-empty or hide it if it is
     * empty.
     **/
    void handleVisibility( const std::string  & text );
    void handleVisibility( const QString &      text );
    void handleVisibility( bool textIsEmpty );
};


#endif // YQWidgetCaption_h
