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
  File:	      YQPkgChangesDialog.cc
  Author:     Stefan Hundhammer <shundhammer@suse.com>

  Textdomain "qt-pkg"

*/


#include <yui/qt/YQUI.h>
#include "YQi18n.h"

#include <zypp/ResStatus.h>
#include <zypp/VendorSupportOptions.h>
#include <zypp/ui/UserWantedPackages.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QStyle>
#include <QBoxLayout>
#include <QWindow>
#include <QScreen>

#include "YQPkgChangesDialog.h"
#include "YQPkgList.h"
#include "YQZypp.h"
#include "QY2LayoutUtils.h"

#include "Logger.h"
#include "Exception.h"


using std::set;
using std::string;

YQPkgChangesDialog::YQPkgChangesDialog( QWidget *		parent,
					const QString & 	message,
					const QString &		acceptButtonLabel,
					const QString &		rejectButtonLabel )
    : QDialog( parent )
    , _filter(0)
{
    // Dialog title
    setWindowTitle( _( "Changed Packages" ) );

    // Enable dialog resizing even without window manager
    setSizeGripEnabled( true );

    // Limit dialog size to available screen size
    setMaximumSize( QGuiApplication::primaryScreen()->availableSize() );

    // Layout for the dialog ( can't simply insert a QVBox )

    QVBoxLayout * layout = new QVBoxLayout();
    Q_CHECK_PTR( layout );
    setLayout(layout);

    QHBoxLayout * hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    layout->addLayout( hbox );


    // Icon

    QLabel * iconLabel = new QLabel( this );
    Q_CHECK_PTR( iconLabel );
    hbox->addWidget(iconLabel);
    iconLabel->setPixmap( YQUI::ui()->loadIcon( "dialog-information" ).pixmap(64) );
    iconLabel->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert

    // Label for the message
    QLabel * label = new QLabel( message, this );
    Q_CHECK_PTR( label );
    hbox->addWidget(label);
    label->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert

    _filter = new QComboBox(this);

    // add the items.
    _filter->addItem(_("All"), QVariant::fromValue(Filters(FilterAll)));
    _filter->addItem(_("Selected by the user"), QVariant::fromValue(Filters(FilterUser)));
    _filter->addItem(_("Automatic Changes"), QVariant::fromValue(Filters(FilterAutomatic)));

    _filter->setCurrentIndex(0);

    layout->addWidget(_filter);
    connect( _filter, SIGNAL(currentIndexChanged(int)),
             SLOT(slotFilterChanged(int)));

    // Pkg list

    _pkgList = new YQPkgList( this );
    Q_CHECK_PTR( _pkgList );
    _pkgList->setEditable( false );

    layout->addWidget( _pkgList );


    // Button box

    hbox = new QHBoxLayout();
    Q_CHECK_PTR( hbox );
    layout->addLayout( hbox );

    hbox->addStretch();

    // Accept button - usually "OK" or "Continue"
    QPushButton * button = new QPushButton( acceptButtonLabel, this );
    Q_CHECK_PTR( button );
    hbox->addWidget( button );
    button->setDefault( true );

    connect( button,	SIGNAL( clicked() ),
	     this,      SLOT  ( accept()  ) );

    hbox->addStretch();

    if ( ! rejectButtonLabel.isEmpty() )
    {
	// Reject button ( if desired ) - usually "Cancel"

	button = new QPushButton( rejectButtonLabel, this );
	Q_CHECK_PTR( button );
        hbox->addWidget(button);
	connect( button,	SIGNAL( clicked() ),
		 this,      	SLOT  ( reject()  ) );

	hbox->addStretch();
    }
}

void
YQPkgChangesDialog::filter( Filters f )
{
    filter( QRegExp( "" ), f );
}

void
YQPkgChangesDialog::slotFilterChanged( int index )
{
    logInfo() << "filter index changed to: " << index << endl;
    QVariant v = _filter->itemData(index);

    if ( v.isValid() && v.canConvert<Filters>() )
    {
        Filters f = v.value<Filters>();
        filter(f);
    }
    else
    {
        logError() << "Can't find filter for index " << index << endl;
    }

}

void
YQPkgChangesDialog::setFilter( Filters f )
{
    setFilter(QRegExp(""), f);
}

void
YQPkgChangesDialog::setFilter( const QRegExp &regexp, Filters f )
{
    logInfo() << "filter changed to: " << f << endl;

    int index = -1;
    for ( int k = 0; k < _filter->count(); ++k )
    {
        QVariant v = _filter->itemData(k);
        if ( v.isValid() && v.canConvert<Filters>() )
        {

            Filters setf = v.value<Filters>();
            if ( setf == f )
                index = k;
        }
    }

    if ( index != -1 )
    {
        // so we dont get called again
        _filter->blockSignals(true);
        // try to set the widget
        _filter->setCurrentIndex(f);
        _filter->blockSignals(false);
        filter(regexp, f);
    }
    else
    {
        logError() << "Can't find index for filter " << f << endl;
    }
}


void
YQPkgChangesDialog::filter( const QRegExp & regexp, Filters f )
{
    YQUI::ui()->busyCursor();
    _pkgList->clear();

    bool byAuto = f.testFlag(FilterAutomatic);
    bool byUser = f.testFlag(FilterUser);
    bool byApp = f.testFlag(FilterUser);

    int discard_regex = 0;
    int discard_ignored = 0;
    int discard_extra = 0;
    int discard_notmodified = 0;
    int discard_whomodified = 0;

    set<string> ignoredNames;

    if ( ! byUser || ! byApp )
	ignoredNames = zypp::ui::userWantedPackageNames();

    for ( ZyppPoolIterator it = zyppPkgBegin();
	  it != zyppPkgEnd();
	  ++it )
    {
	ZyppSel selectable = *it;

	if ( selectable->toModify() )
	{
            zypp::ResStatus::TransactByValue modifiedBy = selectable->modifiedBy();

	    if ( ( ( modifiedBy == zypp::ResStatus::SOLVER     ) && byAuto ) ||
           ( ( modifiedBy == zypp::ResStatus::APPL_LOW ||
               modifiedBy == zypp::ResStatus::APPL_HIGH  ) && byApp ) ||
           ( ( modifiedBy == zypp::ResStatus::USER       ) && byUser )  )
	    {
                if ( regexp.isEmpty()
                     || regexp.indexIn( selectable->name().c_str() ) >= 0 )
                {
                    if ( ! contains( ignoredNames, selectable->name() ) )
                    {
                        ZyppPkg pkg = tryCastToZyppPkg( selectable->theObj() );
                        if ( extraFilter( selectable, pkg ) )
                            _pkgList->addPkgItem( selectable, pkg );
                        else
                            discard_extra++;
                    }
                    else
                    { discard_ignored++; }
                }
                else
                { discard_regex++; }
	    }
            else
            { discard_whomodified++; }

	}
        else
        { discard_notmodified++; }

    }

    logInfo() << "Filter result summary: " << endl;
    logInfo() << "Discarded by extra filter: " << discard_extra << endl;
    logInfo() << "Discarded by ignored: " << discard_ignored << endl;
    logInfo() << "Discarded by regex: " << discard_regex << endl;
    logInfo() << "Discarded because not modified: " << discard_notmodified << endl;
    logInfo() << "Discarded by who modified: " << discard_whomodified << endl;
    YQUI::ui()->normalCursor();
}

bool
YQPkgChangesDialog::extraFilter( ZyppSel sel, ZyppPkg pkg )
{
    return true;
}

bool
YQPkgChangesDialog::isEmpty() const
{
    return _pkgList->topLevelItemCount() == 0;
}


QSize
YQPkgChangesDialog::sizeHint() const
{
    return limitToScreenSize( this, QDialog::sizeHint() );
}


bool
YQPkgChangesDialog::showChangesDialog( QWidget *	parent,
				       const QString & 	message,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
                                       Filters f,
                                       Options o )
{
    YQPkgChangesDialog dialog( parent,
			       message,
			       acceptButtonLabel,
			       rejectButtonLabel );

    dialog.setFilter(f);

    if ( dialog.isEmpty() && o.testFlag(OptionAutoAcceptIfEmpty) )
    {
        logInfo() << "No items to show in changes dialog, accepting it automatically" << endl;
	return true;
    }


    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}


bool
YQPkgChangesDialog::showChangesDialog( QWidget *	parent,
				       const QString & 	message,
				       const QRegExp &  regexp,
				       const QString &	acceptButtonLabel,
				       const QString &	rejectButtonLabel,
                                       Filters f,
                                       Options o )
{
    YQPkgChangesDialog dialog( parent,
			       message,
			       acceptButtonLabel,
			       rejectButtonLabel );
    dialog.setFilter(regexp,f);

    if ( dialog.isEmpty() &&  o.testFlag(OptionAutoAcceptIfEmpty) )
    {
        logInfo() << "No items to show in dialog, accepting it automatically" << endl;
	return true;
    }

    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}

YQPkgUnsupportedPackagesDialog::YQPkgUnsupportedPackagesDialog( QWidget *parent,
                                                                const QString &	message,
                                                                const QString &	acceptButtonLabel,
                                                                const QString &	rejectButtonLabel )
    : YQPkgChangesDialog( parent, message, acceptButtonLabel, rejectButtonLabel )
{
}

bool YQPkgUnsupportedPackagesDialog::extraFilter( ZyppSel sel, ZyppPkg pkg )
{
    if (!pkg || !sel)
        return false;

    return pkg->maybeUnsupported() && sel->toInstall();
}

bool
YQPkgUnsupportedPackagesDialog::showUnsupportedPackagesDialog( QWidget *	parent,
                                                               const QString & 	message,
                                                               const QString &	acceptButtonLabel,
                                                               const QString &	rejectButtonLabel,
                                                               Filters f,
                                                               Options o )
{
    YQPkgUnsupportedPackagesDialog dialog( parent,
                                           message,
                                           acceptButtonLabel,
                                           rejectButtonLabel );

    dialog.setFilter(f);

    if ( dialog.isEmpty() && o.testFlag(OptionAutoAcceptIfEmpty) )
    {
        logInfo() << "No items to show in unsupported packages dialog, accepting it automatically" << endl;
	return true;
    }

    dialog.exec();

    return dialog.result() == QDialog::Accepted;
}


