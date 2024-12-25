/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include <QCheckBox>
#include <QComboBox>
#include <QElapsedTimer>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSettings>

#include <zypp/PoolQuery.h>

#include "Exception.h"
#include "Logger.h"
#include "SearchFilter.h"
#include "QY2CursorHelper.h"
#include "QY2LayoutUtils.h"
#include "YQPkgSelector.h"
#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgSearchFilterView.h"


using std::string;


YQPkgSearchFilterView::YQPkgSearchFilterView( QWidget * parent )
    : QScrollArea( parent )
{
    _matchCount = 0;

#if 1
    QWidget * content = new QWidget;
    CHECK_NEW( content );

    QVBoxLayout * layout = new QVBoxLayout;
    CHECK_NEW( layout );
    content->setLayout( layout );

    // Box for the search button
    QHBoxLayout * hbox = new QHBoxLayout();
    CHECK_NEW( hbox );
    layout->addLayout( hbox );

    // Input field (combo box) for search text
    _searchText = new QComboBox( content );
    CHECK_NEW( _searchText );
    _searchText->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) );

    hbox->addWidget(_searchText);
    _searchText->setEditable( true );

    // Search button
    _searchButton = new QPushButton( _( "&Search" ), content );
    CHECK_NEW( _searchButton );
    hbox->addWidget(_searchButton);

    layout->addStretch();


    //
    // Where to search
    //

    QGroupBox * gbox = new QGroupBox( _( "Search in" ), content );
    CHECK_NEW( gbox );
    layout->addWidget( gbox );

    QVBoxLayout *vLayout = new QVBoxLayout;
    gbox->setLayout( vLayout );

    _searchInName        = new QCheckBox( _( "Nam&e"            ), gbox ); CHECK_NEW( _searchInName        );
    vLayout->addWidget(_searchInName);
    _searchInSummary     = new QCheckBox( _( "Su&mmary"         ), gbox ); CHECK_NEW( _searchInSummary     );
    vLayout->addWidget(_searchInSummary);
    _searchInDescription = new QCheckBox( _( "Descr&iption"     ), gbox ); CHECK_NEW( _searchInDescription );
    vLayout->addWidget(_searchInDescription);

    vLayout->addStretch();

    _searchInProvides    = new QCheckBox( _( "RPM \"P&rovides\""), gbox ); CHECK_NEW( _searchInProvides    );
    vLayout->addWidget(_searchInProvides);
    _searchInRequires    = new QCheckBox( _( "RPM \"Re&quires\""), gbox ); CHECK_NEW( _searchInRequires    );
    vLayout->addWidget(_searchInRequires);

    _searchInFileList    = new QCheckBox( _( "File list"        ), gbox ); CHECK_NEW( _searchInFileList    );
    vLayout->addWidget(_searchInFileList);

    _searchInName->setChecked( true );

    layout->addStretch();


    //
    // Search mode
    //

    QLabel * label = new QLabel( _( "Search &Mode:" ), content );
    CHECK_NEW( label );
    layout->addWidget( label );

    _searchMode = new QComboBox( content );
    CHECK_NEW( _searchMode );
    layout->addWidget( _searchMode );

    _searchMode->setEditable( false );

    label->setBuddy( _searchMode );

    // Caution: combo box items must be inserted in the same order as enum
    // SearchFilter::FilterMode in SearchFilter.h!

    _searchMode->addItem( _( "Auto"               ) );
    _searchMode->addItem( _( "Contains"           ) );
    _searchMode->addItem( _( "Starts With"        ) );
    _searchMode->addItem( _( "Exact Match"        ) );
    _searchMode->addItem( _( "Wildcards"          ) );
    _searchMode->addItem( _( "Regular Expression" ) );

    _searchMode->setCurrentIndex( SearchFilter::Auto );

    layout->addStretch();

    _caseSensitive = new QCheckBox( _( "Case Sensiti&ve" ), content );
    CHECK_NEW( _caseSensitive );
    layout->addWidget(_caseSensitive);

    for ( int i=0; i < 6; i++ )
        layout->addStretch();

    setWidgetResizable( true );
    setWidget( content );
#endif

    _searchText->lineEdit()->setClearButtonEnabled( true );

    connect( _searchButton, SIGNAL( clicked() ),
             this,          SLOT  ( filter()  ) );

    readSettings();
}


YQPkgSearchFilterView::~YQPkgSearchFilterView()
{
    writeSettings();
}


void
YQPkgSearchFilterView::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
        if ( event->modifiers() == Qt::NoModifier ||    // No Ctrl / Alt / Shift etc. pressed
             event->modifiers() == Qt::KeypadModifier )
        {
            if ( event->key() == Qt::Key_Return ||
                 event->key() == Qt::Key_Enter    )
            {
                _searchButton->animateClick();
                return;
            }
        }

    }

    QWidget::keyPressEvent( event );
}


void
YQPkgSearchFilterView::setFocus()
{
    _searchText->setFocus();
}


QSize
YQPkgSearchFilterView::minimumSizeHint() const
{
    return QSize( 0, 0 );
}


void
YQPkgSearchFilterView::filterIfVisible()
{
    if ( isVisible() )
        filter();
}


void
YQPkgSearchFilterView::filter()
{
    emit filterStart();
    _matchCount = 0;

    try
    {
        if ( ! _searchText->currentText().isEmpty() )
        {
            //
            // Build the query
            //

            SearchFilter searchFilter( buildSearchFilterFromWidgets() );

            // Use a zypp::PoolQuery for improved performance
            zypp::PoolQuery query;
            query.addKind( zypp::ResKind::package );
            string searchPattern = toUTF8( searchFilter.pattern() );
            query.setCaseSensitive( searchFilter.isCaseSensitive() );

            switch ( searchFilter.filterMode() )
            {
                case SearchFilter::Contains:
                    query.setMatchSubstring();
                    break;

                case SearchFilter::StartsWith:
                    query.setMatchRegex();
                    searchPattern = "^" + searchPattern;
                    break;

                case SearchFilter::ExactMatch:
                    query.setMatchExact();
                    break;

                case SearchFilter::Wildcard:
                    query.setMatchGlob();
                    break;

                case SearchFilter::RegExp:
                    query.setMatchRegex();
                    break;

                default:
                    logError() << "Unexpected search mode "
                               << SearchFilter::toString( searchFilter.filterMode() )
                               << " - falling back to 'Contains'"
                               << endl;
                    query.setMatchSubstring();
                    break;
            }

            query.addString( searchPattern );

            if ( _searchInName->isChecked()        ) query.addAttribute( zypp::sat::SolvAttr::name );
            if ( _searchInDescription->isChecked() ) query.addAttribute( zypp::sat::SolvAttr::description );
            if ( _searchInSummary->isChecked()     ) query.addAttribute( zypp::sat::SolvAttr::summary );
            if ( _searchInRequires->isChecked()    ) query.addAttribute( zypp::sat::SolvAttr( "solvable:requires" ) );
            if ( _searchInProvides->isChecked()    ) query.addAttribute( zypp::sat::SolvAttr( "solvable:provides" ) );
            if ( _searchInFileList->isChecked()    ) query.addAttribute( zypp::sat::SolvAttr::filelist );

            _searchText->setEnabled( false );   // Disable for the duration of the search
            _searchButton->setEnabled( false );

            QElapsedTimer queryTimer;
            queryTimer.start();

            //
            // Start the query and iterate over the results
            //

            for ( zypp::PoolQuery::Selectable_iterator it = query.selectableBegin();
                  it != query.selectableEnd();
                  ++it )
            {
                ZyppSel selectable = *it;
                ZyppPkg zyppPkg    = tryCastToZyppPkg( selectable->theObj() );

                if ( zyppPkg )
                {
                    _matchCount++;
                    emit filterMatch( selectable, zyppPkg );
                }


                if ( queryTimer.elapsed() > 300 ) // milisec
                {
                    // Process events only every 300 milliseconds - this is very
                    // expensive since both the progress dialog and the package
                    // list change all the time, thus display updates are necessary
                    // each time.

                    qApp->processEvents();
                    queryTimer.restart();
                }
            }

            if ( _matchCount == 0 )
                emit message( _( "No Results." ) );
        }
    }
    catch ( const std::exception & exception )
    {
        logWarning() << "CAUGHT zypp exception: " << exception.what() << endl;

        QMessageBox msgBox;

        // Translators: This is a (short) text indicating that something went
        // wrong while searching for packages. At this point, it is not clear
        // if it's a user error (e.g., syntax error in regular expression) or
        // an internal error. But there is a "Details" button that will return
        // the original (translated) error message.

        QString heading = _( "Query Error" );

        if ( heading.length() < 25 )    // Avoid very narrow message boxes
        {
            QString blanks;
            blanks.fill( ' ', 50 - heading.length() );
            heading += blanks;
        }

        msgBox.setText( heading );
        msgBox.setIcon( QMessageBox::Warning );
        msgBox.setInformativeText( fromUTF8( exception.what() ) );
        msgBox.exec();
    }

    _searchText->setEnabled(true);   // Re-enable the widgets we disabled earlier
    _searchButton->setEnabled(true);
    parentWidget()->parentWidget()->setCursor( Qt::ArrowCursor );

    emit filterFinished();
}


SearchFilter
YQPkgSearchFilterView::buildSearchFilterFromWidgets()
{
    SearchFilter::FilterMode filterMode = (SearchFilter::FilterMode) _searchMode->currentIndex();
    SearchFilter searchFilter( _searchText->currentText(), filterMode );
    searchFilter.setCaseSensitive( _caseSensitive->isChecked() );

    return searchFilter;
}


bool
YQPkgSearchFilterView::check( ZyppSel   selectable,
                              ZyppObj   zyppObj )
{
    if ( ! zyppObj )
        return false;

    SearchFilter searchFilter( buildSearchFilterFromWidgets() );

    bool match =
        ( _searchInName->isChecked()        && searchFilter.matches( zyppObj->name()         ) ) ||
        ( _searchInSummary->isChecked()     && searchFilter.matches( zyppObj->summary()      ) ) ||
        ( _searchInDescription->isChecked() && searchFilter.matches( zyppObj->description()  ) ) ||
        ( _searchInProvides->isChecked()    && checkCap( zyppObj->provides(), searchFilter   ) ) ||
        ( _searchInRequires->isChecked()    && checkCap( zyppObj->requires(), searchFilter   ) );

    return match;
}


bool
YQPkgSearchFilterView::checkCap( zypp::Capabilities   capSet,
                                 const SearchFilter & searchFilter )
{
    for ( zypp::Capabilities::const_iterator it = capSet.begin();
	  it != capSet.end();
	  ++it )
    {
        zypp::CapDetail cap( *it );

	if ( cap.isSimple() && searchFilter.matches( cap.name().asString() ) )
	{
	    return true;
	}
    }

    return false;
}


void YQPkgSearchFilterView::readSettings()
{
    QSettings settings;
    settings.beginGroup( "PkgSearchFilterView" );

    _searchInName->setChecked        ( settings.value( "searchInName",        true  ).toBool() );
    _searchInSummary->setChecked     ( settings.value( "searchInSummary",     false ).toBool() );
    _searchInDescription->setChecked ( settings.value( "searchInDescription", false ).toBool() );
    _searchInProvides->setChecked    ( settings.value( "searchInProvides",    false ).toBool() );
    _searchInRequires->setChecked    ( settings.value( "searchInRequires",    false ).toBool() );
    _searchInFileList->setChecked    ( settings.value( "searchInFileList",    false ).toBool() );

    _caseSensitive->setChecked       ( settings.value( "caseSensitive",       false ).toBool() );
    _searchMode->setCurrentIndex     ( settings.value( "searchMode",          0     ).toInt() );

    settings.endGroup();
}


void YQPkgSearchFilterView::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "PkgSearchFilterView" );

    settings.setValue( "searchInName",        _searchInName->isChecked()        );
    settings.setValue( "searchInSummary",     _searchInSummary->isChecked()     );
    settings.setValue( "searchInDescription", _searchInDescription->isChecked() );
    settings.setValue( "searchInProvides",    _searchInProvides->isChecked()    );
    settings.setValue( "searchInRequires",    _searchInRequires->isChecked()    );
    settings.setValue( "searchInFileList",    _searchInFileList->isChecked()    );

    settings.setValue( "caseSensitive",       _caseSensitive->isChecked()       );
    settings.setValue( "searchMode",          _searchMode->currentIndex()       );

    settings.endGroup();
}
