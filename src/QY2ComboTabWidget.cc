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

  File:	      QY2ComboTabWidget.cc

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt widget - it can be used independently of YaST2.

/-*/


#include <QComboBox>
#include <QLabel>
#include <QStackedWidget>
#include <QHBoxLayout>

#include <QFrame>

#include "Logger.h"
#include "Exception.h"
#include "QY2ComboTabWidget.h"

using std::string;



QY2ComboTabWidget::QY2ComboTabWidget( const QString &	label,
				      QWidget *		parent )
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin( 0 );

    QHBoxLayout *hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );

    hbox->setSpacing( 0 );
    hbox->setMargin ( 0  );

    vbox->addLayout(hbox);
    this->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ) ); // hor/vert

    _comboLabel = new QLabel(label);
    hbox->addWidget(_comboLabel);
    Q_CHECK_PTR( _comboLabel );

    _comboBox = new QComboBox( this );
    Q_CHECK_PTR( _comboBox );
    hbox->addWidget(_comboBox);
    _comboLabel->setBuddy( _comboBox );
    _comboBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
    
    connect( _comboBox, SIGNAL( activated    ( int ) ),
             this,      SLOT  ( showPageIndex( int ) ) );

    _widgetStack = new QStackedWidget( this );
    Q_CHECK_PTR( _widgetStack );
    vbox->addWidget(_widgetStack);
}



QY2ComboTabWidget::~QY2ComboTabWidget()
{

}


void
QY2ComboTabWidget::addPage( const QString & page_label, QWidget * new_page )
{
    _pages.insert( _comboBox->count(), new_page );
    _comboBox->addItem( page_label );
    _widgetStack->addWidget( new_page );

    if ( ! _widgetStack->currentWidget() )
	_widgetStack->setCurrentWidget( new_page );
}


void
QY2ComboTabWidget::showPageIndex( int index )
{
    if ( _pages.contains(index) )
    {
        QWidget * page = _pages[ index ];
	_widgetStack->setCurrentWidget( page );
	// yuiDebug() << "Changing current page" << endl;
	emit currentChanged( page );
    }
    else
    {
	qWarning( "QY2ComboTabWidget: Page #%d not found", index );
	return;
    }
}


void
QY2ComboTabWidget::showPage( QWidget * page )
{
    _widgetStack->setCurrentWidget( page );

    if ( page == _pages[ _comboBox->currentIndex() ] )
    {
          // Shortcut: If the requested page is the one that belongs to the item
          // currently selected in the combo box, don't bother searching the
          // correct combo box item.
          return;
    }

    // Search the dict for this page

    QHashIterator<int, QWidget *> it( _pages );

    while ( it.hasNext() )
    {
        it.next();
	if ( page == it.value() )
	{
	    _comboBox->setCurrentIndex( it.key() );
	    return;
	}
    }

    // If we come this far, that page isn't present in the dict.

    qWarning( "QY2ComboTabWidget: Page not found" );
}




