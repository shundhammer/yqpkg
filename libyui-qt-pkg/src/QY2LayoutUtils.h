/*
  Copyright (c) 2000 - 2010 Novell, Inc.
  Copyright (c) 2021 SUSE LLC

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


/*
  File:	      QY2LayoutUtils.h
  Author:     Stefan Hundhammer <shundhammer.de>

  These are pure Qt functions - they can be used independently of YaST2.
*/


#ifndef QY2LayoutUtils_h
#define QY2LayoutUtils_h

class QWidget;


/**
 * Add vertical stretchable space.
 **/
QWidget * addVStretch( QWidget * parent );

/**
 * Add horizontal stretchable space.
 **/
QWidget * addHStretch( QWidget * parent );

/**
 * Add a fixed height vertical space
 **/
QWidget * addVSpacing( QWidget * parent, int height = 8 );

/**
 * Add a fixed height horizontal space
 **/
QWidget * addHSpacing( QWidget * parent, int width = 8 );

/**
 * Limit a QSize to what is available on the same screen as 'widget'.
 **/
QSize limitToScreenSize( const QWidget * widget, const QSize & desiredSize );
QSize limitToScreenSize( const QWidget * widget, int width, int height );

#endif // ifndef QY2LayoutUtils_h
