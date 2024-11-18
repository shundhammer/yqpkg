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
  File:	      YQPkgVersionsView.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/qt/YQSignalBlocker.h>
#include <yui/qt/YQi18n.h>
#include <yui/qt/utf8.h>

#include <zypp/Repository.h>

#include <QTabWidget>
#include <QRegExp>
#include <QHeaderView>
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QMessageBox>
#include <QApplication>

#include "YQZypp.h"
#include "YQPkgVersionsView.h"
#include "YQPkgRepoList.h"
#include "YQIconPool.h"

#include "Logger.h"
#include "Exception.h"



YQPkgVersionsView::YQPkgVersionsView( QWidget * parent )
    : QScrollArea( parent )
    , _buttonGroup( 0 )
    , _layout( 0 )
{
    _selectable		 = 0;
    _isMixedMultiVersion = false;
    _parentTab		 = dynamic_cast<QTabWidget *> (parent);

    if ( _parentTab )
    {
	connect( _parentTab, SIGNAL( currentChanged( int ) ),
		 this,	     SLOT  ( reload	   ( int ) ) );
    }
}


YQPkgVersionsView::~YQPkgVersionsView()
{
    // NOP
}


void
YQPkgVersionsView::reload( int newCurrent )
{
    if ( _parentTab && _parentTab->widget( newCurrent ) == this )
	showDetailsIfVisible( _selectable );
}


void
YQPkgVersionsView::showDetailsIfVisible( ZyppSel selectable )
{
    _selectable = selectable;
    _isMixedMultiVersion = isMixedMultiVersion( selectable );

    if ( _parentTab )	// Is this view embedded into a tab widget?
    {
	if ( _parentTab->currentWidget() == this )  // Is this page the topmost?
	    showDetails( selectable );
    }
    else		// No tab parent - simply show data unconditionally.
    {
	showDetails( selectable );
    }
}


void
YQPkgVersionsView::showDetails( ZyppSel selectable )
{
    _selectable = selectable;
    _isMixedMultiVersion = isMixedMultiVersion( selectable );
    QWidget * content = widget();
    delete content; // This recursively deletes all child widgets and qobjects

    content	 = new QWidget( this );
    _buttonGroup = new QButtonGroup( content );
    _layout	 = new QVBoxLayout( content );
    content->setLayout( _layout );

    if ( ! selectable )
    {
	setWidget( content );	// Prevent mem leak
	return;
    }

    QLabel * pkgNameLabel = new QLabel( this );

    if ( ! selectable->theObj() )
	return;

    _layout->addWidget( pkgNameLabel );

    QFont font = pkgNameLabel->font();
    font.setBold( true );

    QFontMetrics fm( font) ;
    font.setPixelSize( (int) ( fm.height() * 1.1 ) );

    pkgNameLabel->setFont( font );
    pkgNameLabel->setText( fromUTF8(selectable->theObj()->name().c_str()) );

    if ( selectable->multiversionInstall() ) // at least one (!) PoolItem is multiversion
    {
	//
	// Find installed and available objects (for multiversion view)
	//
	{
	    zypp::ui::Selectable::picklist_iterator it = selectable->picklistBegin();

	    while ( it != selectable->picklistEnd() )
	    {
		YQPkgMultiVersion * version = new YQPkgMultiVersion( this, selectable, *it );

		_layout->addWidget( version );

		connect( version, SIGNAL( statusChanged() ),
			 this,	  SIGNAL( statusChanged() ) );

		connect( this,	  SIGNAL( statusChanged() ),
			 version, SLOT	( update()	  ) );

		++it;
	    }

	}
    }
    else
    {
	//
	// Fill installed objects
	//
	{
	    zypp::ui::Selectable::installed_iterator it = selectable->installedBegin();

	    while ( it != selectable->installedEnd() )
	    {
		// Cache this, it's somewhat expensive
		bool retracted = installedIsRetracted( selectable, *it );
		QString text;

		if ( retracted )
		{
		    text = _( "%1-%2 [RETRACTED] from vendor %3 (installed)" )
			.arg( fromUTF8( (*it)->edition().asString().c_str() ) )
			.arg( fromUTF8( (*it)->arch().asString().c_str() ) )
			.arg( fromUTF8( (*it)->vendor().c_str() ) ) ;

		}
		else
		{
		    text = _( "%1-%2 from vendor %3 (installed)" )
			.arg( fromUTF8( (*it)->edition().asString().c_str() ) )
			.arg( fromUTF8( (*it)->arch().asString().c_str() ) )
			.arg( fromUTF8( (*it)->vendor().c_str() ) ) ;
		}

		QWidget * installedVersion = new QWidget( this );
		QHBoxLayout * instLayout = new QHBoxLayout( installedVersion );
		instLayout->setContentsMargins( 0, 0, 0, 0 );

		QLabel * icon = new QLabel( installedVersion );
		icon->setPixmap( YQIconPool::pkgSatisfied() );
		instLayout->addWidget( icon );

		QLabel * textLabel = new QLabel( text, installedVersion );
		instLayout->addWidget( textLabel );
		instLayout->addStretch();

		if ( retracted )
		    setRetractedColor( textLabel );

		_layout->addWidget( installedVersion );

		++it;
	    }
	}


	//
	// Fill available objects
	//

	{
	    zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

	    while ( it != selectable->availableEnd() )
	    {
		YQPkgVersion *radioButton = new YQPkgVersion( this, selectable, *it );

		connect( radioButton, SIGNAL( clicked( bool )		 ),
			 this,	      SLOT  ( checkForChangedCandidate() ) );

		_buttonGroup->addButton( radioButton );
		_layout->addWidget( radioButton );

		if ( ! _buttonGroup->checkedButton() &&
		     selectable->hasCandidateObj() &&
		     selectable->candidateObj()->edition() == (*it)->edition() &&
		     selectable->candidateObj()->arch()	   == (*it)->arch() )
		{
		    radioButton->setChecked( true );
		}

		++it;
	    }
	}
    }

    _layout->addStretch();

    // This really needs to wait until this point, or the content will never
    // get its correct size and become visible.

    setWidget( content );
    content->show();
}


void YQPkgVersionsView::setRetractedColor( QWidget * widget )
{
    QPalette pal = widget->palette();
    pal.setColor( QPalette::WindowText, Qt::red );
    widget->setPalette( pal );
}


bool YQPkgVersionsView::installedIsRetracted( ZyppSel selectable, ZyppObj installed )
{
    zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

    while ( it != selectable->availableEnd() )
    {
	if ( (*it)->isRetracted() )
	{
	    if ( installed->edition() == (*it)->edition() &&
		 installed->arch()    == (*it)->arch()	  &&
		 installed->vendor()  == (*it)->vendor()    )
	    {
		return true;
	    }
	}

	++it;
    }

    return false;
}


void
YQPkgVersionsView::checkForChangedCandidate()
{
    QListIterator<QAbstractButton*> it( _buttonGroup->buttons() );

    while ( it.hasNext() )
    {
	YQPkgVersion * versionItem = dynamic_cast<YQPkgVersion *> (it.next());

	if ( versionItem && versionItem->isChecked() )
	{
	    ZyppObj newCandidate = versionItem->zyppObj();

	    if ( _selectable && *newCandidate != _selectable->candidateObj() )
	    {
		logInfo() << "Candidate changed" << endl;

		// Change status of selectable

		ZyppStatus status = _selectable->status();

		if ( !_selectable->installedEmpty() &&
		     _selectable->installedObj()->arch()    == newCandidate->arch() &&
		     _selectable->installedObj()->edition() == newCandidate->edition() )
		{
		    // Switch back to the original instance -
		    // the version that was previously installed
		    status = S_KeepInstalled;
		}
		else
		{
		    switch ( status )
		    {
			case S_KeepInstalled:
			case S_Protected:
			case S_AutoDel:
			case S_AutoUpdate:
			case S_Del:
			case S_Update:

			    status = S_Update;
			    break;

			case S_NoInst:
			case S_Taboo:
			case S_Install:
			case S_AutoInstall:
			    status = S_Install;
			    break;
		    }
		}

		_selectable->setStatus( status );


		// Set candidate

		_selectable->setCandidate( newCandidate );
		emit candidateChanged( newCandidate );
		return;
	    }
	}
    }
}


QSize
YQPkgVersionsView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


bool
YQPkgVersionsView::handleMixedMultiVersion( YQPkgMultiVersion * newSelected )
{
    ZyppPoolItem poolItem = newSelected->zyppPoolItem();
    Q_CHECK_PTR( poolItem );

    bool multiVersion = poolItem->multiversionInstall();

    logInfo() << "Selected: "
              << ( multiVersion ? "Multiversion " : "Non-Multiversion " )
              << newSelected->text()
              << endl;

    if ( anyMultiVersionToInstall( !multiVersion ) )
    {
	logInfo() << "Multiversion and non-multiversion conflict!" << endl;
	bool forceContinue = mixedMultiVersionPopup( multiVersion );

	if ( forceContinue )
	{
	    _selectable->setPickStatus( poolItem, S_Install );
	    emit statusChanged(); // update status icons for all versions
	}
	else
	{
	    // Nothing to do here: The status of this item was not changed yet;
	    // simply leave it like it was.
	}

	return true; // handled here
    }
    else
    {
	return false; // Not handled here
    }
}


bool
YQPkgVersionsView::mixedMultiVersionPopup( bool multiversion ) const
{
    // Translators: Popup dialog text. Try to keep the lines about the same length.
    QString msg = _( "You are trying to install multiversion-capable\n"
		     "and non-multiversion-capable versions of this\n"
		     "package at the same time." );
    msg += "\n\n";

    if ( multiversion )
    {
	msg +=
	    _( "This version is multiversion-capable.\n"
	       "\n"
	       "Press \"Continue\" to install this version\n"
	       "and unselect the non-multiversion-capable version,\n"
	       "\"Cancel\" to unselect this version and keep the other one." );
    }
    else
    {
	msg +=
	    _( "This version is not multiversion-capable.\n"
	       "\n"
	       "Press \"Continue\" to install only this version\n"
	       "and unselect all other versions,\n"
	       "\"Cancel\" to unselect this version and keep the other ones." );
    }

    // Dialog heading
    QString heading = _( "Incompatible Package Versions" );

    int buttonNo = QMessageBox::question( 0, // parent
					  heading,
					  msg,
					  _( "C&ontinue" ),	// button #0
					  _( "&Cancel" ) );	// button #1
    logInfo() << "User hit " << (buttonNo == 0 ? "[Continue]" : "[Cancel]" ) << endl;

    return buttonNo == 0;
}



bool
YQPkgVersionsView::anyMultiVersionToInstall( bool multiversion ) const
{
    if ( ! _selectable )
	return false;

    zypp::ui::Selectable::available_iterator it = _selectable->availableBegin();

    while ( it != _selectable->availableEnd() )
    {
	if ( it->multiversionInstall() == multiversion )
	{
	    switch ( _selectable->pickStatus( *it ) )
	    {
		case S_Install:
		case S_AutoInstall:
		    logInfo() << "Found " << ( multiversion ? "multiversion" : "non-multiversion" )
                              << " to install" << endl;
		    return true;

		default:
		    break;
	    }
	}

	++it;
    }

    logInfo() << "No " << ( multiversion ? "multiversion" : "non-multiversion" )
              << " to install" << endl;
    return false;
}


void
YQPkgVersionsView::unselectAllMultiVersion()
{
    if ( ! _selectable )
	return;

    zypp::ui::Selectable::available_iterator it = _selectable->availableBegin();

    while ( it != _selectable->availableEnd() )
    {
	if ( it->multiversionInstall() )
	{
	    switch ( _selectable->pickStatus( *it ) )
	    {
		case S_Install:
		case S_AutoInstall:
		    _selectable->setPickStatus( *it, S_NoInst );
		    logInfo() << "Unselecting " << *it << endl;
		    break;

		default:
		    break;
	    }
	}

	++it;
    }
}


bool
YQPkgVersionsView::isMixedMultiVersion( ZyppSel selectable )
{
    if ( ! selectable )
	return false;

    zypp::ui::Selectable::available_iterator it = selectable->availableBegin();

    if ( it == selectable->availableEnd() )
	return false;

    bool multiversion = it->multiversionInstall();

    while ( it != selectable->availableEnd() )
    {
	if ( it->multiversionInstall() != multiversion )
	{
	    logInfo() << "Mixed multiversion" << endl;
	    return true;
	}

	++it;
    }

    return false;
}






YQPkgVersion::YQPkgVersion( QWidget *	parent,
			    ZyppSel	selectable,
			    ZyppObj	zyppObj )
    : QRadioButton( parent )
    , _selectable( selectable )
    , _zyppObj( zyppObj )
{
    if ( zyppObj->isRetracted() )
    {
	// Translators: %1 is a package version, %2 the package architecture,
	// %3 describes the repository where it comes from,
	// %4 is the repository's priority
	// %5 is the vendor of the package
	// Examples:
	//	   2.5.23-i568 from Packman with priority 100 and vendor openSUSE
	//	   3.17.4-i386 from openSUSE-11.1 update repository with priority 20 and vendor openSUSE
	//	   ^^^^^^ ^^^^	    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^		  ^^		^^^^^^^^
	//	      %1   %2		     %3					  %4		    %5
	setText( _( "%1-%2 [RETRACTED] from %3 with priority %4 and vendor %5" )
		 .arg( fromUTF8( zyppObj->edition().asString().c_str() ) )
		 .arg( fromUTF8( zyppObj->arch().asString().c_str() ) )
		 .arg( fromUTF8( zyppObj->repository().info().name().c_str() ) )
		 .arg( zyppObj->repository().info().priority() )
		 .arg( fromUTF8( zyppObj->vendor().c_str() ) ) );

	YQPkgVersionsView::setRetractedColor( this );
    }
    else
    {
	// Translators: %1 is a package version, %2 the package architecture,
	// %3 describes the repository where it comes from,
	// %4 is the repository's priority
	// %5 is the vendor of the package
	// Examples:
	//	   2.5.23-i568 from Packman with priority 100 and vendor openSUSE
	//	   3.17.4-i386 from openSUSE-11.1 update repository with priority 20 and vendor openSUSE
	//	   ^^^^^^ ^^^^	    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^		  ^^		^^^^^^^^
	//	      %1   %2		     %3					  %4		    %5
	setText( _( "%1-%2 from %3 with priority %4 and vendor %5" )
		 .arg( fromUTF8( zyppObj->edition().asString().c_str() ) )
		 .arg( fromUTF8( zyppObj->arch().asString().c_str() ) )
		 .arg( fromUTF8( zyppObj->repository().info().name().c_str() ) )
		 .arg( zyppObj->repository().info().priority() )
		 .arg( fromUTF8( zyppObj->vendor().c_str() ) ) );
    }
}


YQPkgVersion::~YQPkgVersion()
{
    // NOP
}


QString
YQPkgVersion::toolTip(int)
{
    QString tip;

    if ( *zyppObj() == selectable()->installedObj() )
	tip = _( "This version is installed in your system." );

    return tip;
}




YQPkgMultiVersion::YQPkgMultiVersion( YQPkgVersionsView * parent,
				      ZyppSel		  selectable,
				      ZyppPoolItem	  zyppPoolItem )
    : QCheckBox( parent )
    , _parent( parent )
    , _selectable( selectable )
    , _zyppPoolItem( zyppPoolItem )
{
    setText (_( "%1-%2 from %3 with priority %4 and vendor %5" )
	     .arg( fromUTF8( zyppPoolItem->edition().asString().c_str() ) )
	     .arg( fromUTF8( zyppPoolItem->arch().asString().c_str() ) )
	     .arg( fromUTF8( zyppPoolItem->repository().info().name().c_str() ) )
	     .arg( zyppPoolItem->repository().info().priority() )
	     .arg( fromUTF8( zyppPoolItem->vendor().c_str() ) ));

    connect( this, SIGNAL( toggled( bool)    ),
	     this, SLOT	 ( slotIconClicked() ) );
}


YQPkgMultiVersion::~YQPkgMultiVersion()
{
    // NOP
}


void YQPkgMultiVersion::slotIconClicked()
{
    {
	YQSignalBlocker sigBlocker( this );  // prevent checkmark, we draw the status icons ourselves
	setChecked( false );
    }
    cycleStatus();
}


void YQPkgMultiVersion::cycleStatus()
{
    ZyppStatus oldStatus = _selectable->pickStatus( _zyppPoolItem );
    ZyppStatus newStatus = oldStatus;

    switch ( oldStatus )
    {
	case S_Install:
	case S_AutoInstall:
	case S_Protected:
	    newStatus = S_NoInst;
	    break;

	case S_KeepInstalled:
	case S_Update:
	case S_AutoUpdate:
	    newStatus = S_Del;
	    break;


	case S_Del:
	case S_AutoDel:
	    newStatus = S_KeepInstalled;
	    break;

	case S_NoInst:
	case S_Taboo:
	    newStatus = S_Install;
	    break;
    }

    bool handled = false;

    if ( _parent->isMixedMultiVersion() &&
	 newStatus == S_Install &&
	 oldStatus != newStatus )
    {
	handled = _parent->handleMixedMultiVersion( this );
    }

    if ( ! handled )
	setStatus( newStatus );

    logInfo() << "oldStatus: " << oldStatus << endl;
    ZyppStatus actualStatus = _selectable->pickStatus( _zyppPoolItem );

    if ( actualStatus != newStatus )
	logWarning() << "FAILED to set new status: " << newStatus
		     << "  actual Status: " << actualStatus << endl;
    else
	logInfo() << "newStatus:" << newStatus << endl;

    if ( oldStatus != actualStatus )
    {
	update();
	emit statusChanged();
    }
}


void YQPkgMultiVersion::setStatus( ZyppStatus newStatus )
{
    logInfo() << "Setting pick status to " << newStatus << endl;
    _selectable->setPickStatus( _zyppPoolItem, newStatus );
}


void YQPkgMultiVersion::paintEvent(QPaintEvent *)
{
    // draw the usual checkbox
    QStylePainter p( this );
    QStyleOptionButton opt;
    initStyleOption( &opt );
    p.drawControl( QStyle::CE_CheckBox, opt );


    // calculate position and draw the status icon
    QRect elementRect = style()->subElementRect ( QStyle::SE_CheckBoxIndicator, &opt );
    QPixmap icon = statusIcon( _selectable->pickStatus( _zyppPoolItem ) );

    QPoint start = elementRect.center() - icon.rect().center();
    QRect rect = QRect( start.x(), start.y(), icon.width(), icon.height() );

    p.drawItemPixmap( rect, 0, icon );
}


QPixmap YQPkgMultiVersion::statusIcon( ZyppStatus status )
{
    QPixmap icon = YQIconPool::pkgNoInst();

    switch ( status )
    {
	case S_Del:		    icon = YQIconPool::pkgDel();	    break;
	case S_Install:		    icon = YQIconPool::pkgInstall();	    break;
	case S_KeepInstalled:	    icon = YQIconPool::pkgKeepInstalled();  break;
	case S_NoInst:		    icon = QPixmap();			    break;
	case S_Protected:	    icon = YQIconPool::pkgProtected();	    break;
	case S_Taboo:		    icon = YQIconPool::pkgTaboo();	    break;
	case S_Update:		    icon = YQIconPool::pkgUpdate();	    break;

	case S_AutoDel:		    icon = YQIconPool::pkgAutoDel();	    break;
	case S_AutoInstall:	    icon = YQIconPool::pkgAutoInstall();    break;
	case S_AutoUpdate:	    icon = YQIconPool::pkgAutoUpdate();	    break;

	// Intentionally omitting 'default' branch so the compiler can
	// catch unhandled enum states
    }
    return icon;
}



