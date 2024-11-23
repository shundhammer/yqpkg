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


#include <vector>
#include <algorithm> // std::swap()

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolButton>

#include "Exception.h"
#include "Logger.h"
#include "YQIconPool.h"
#include "YQPkgDiskUsageList.h"
#include "YQSignalBlocker.h"
#include "YQi18n.h"
#include "utf8.h"

#include "YQPkgFilterTab.h"


using std::vector;
typedef vector<YQPkgFilterPage *> YQPkgFilterPageVector;

#define SHOW_ONLY_IMPORTANT_PAGES	1
#define VIEW_BUTTON_LEFT		1

#define SETTINGS_DIR			"YaST2"


#define MARGIN 5 		// inner margin between 3D borders and content
#define TOP_EXTRA_MARGIN		3
#define SPLITTER_HALF_SPACING		2


struct YQPkgFilterTabPrivate
{
    YQPkgFilterTabPrivate( const QString & name )
	: settingsName( name )
	, baseClassWidgetStack(0)
	, outerSplitter(0)
	, leftPaneSplitter(0)
	, filtersWidgetStack(0)
	, diskUsageList(0)
	, rightPane(0)
	, viewButton(0)
	, tabContextMenu(0)
	, tabContextMenuPage(0)
	{}

    QString			settingsName;
    QStackedWidget *		baseClassWidgetStack;
    QSplitter *			outerSplitter;
    QSplitter *			leftPaneSplitter;
    QStackedWidget *		filtersWidgetStack;
    YQPkgDiskUsageList *	diskUsageList;
    QWidget *			rightPane;
    QPushButton *		viewButton;
    QMenu *                     tabContextMenu;
    QAction *			actionMovePageLeft;
    QAction *			actionMovePageRight;
    QAction *			actionClosePage;
    YQPkgFilterPage *		tabContextMenuPage;
    YQPkgFilterPageVector	pages;
};




YQPkgFilterTab::YQPkgFilterTab( QWidget * parent, const QString & settingsName )
    : QTabWidget( parent )
    , priv( new YQPkgFilterTabPrivate( settingsName ) )
{
    CHECK_NEW( priv );

    // Nasty hack: Find the base class's QStackedWidget in its widget tree so
    // we have a place to put our own widgets. Unfortunately, this is private
    // in the base class, but Qt lets us search the widget hierarchy by widget
    // type.

    priv->baseClassWidgetStack = findChild<QStackedWidget*>();
    CHECK_PTR( priv->baseClassWidgetStack );

    // Nasty hack: Disconnect the base class from signals from its tab bar.
    // We will handle that signal on our own.

    disconnect( tabBar(), &QTabBar::currentChanged, 0, 0 );


    //
    // Splitter that divides this widget into a left and a right pane
    //

    priv->outerSplitter = new QSplitter( Qt::Horizontal, this );
    CHECK_NEW( priv->outerSplitter );

    priv->outerSplitter->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
						     QSizePolicy::Expanding ) );
    priv->baseClassWidgetStack->addWidget( priv->outerSplitter );


#if SHOW_ONLY_IMPORTANT_PAGES

    //
    // "View" and "Close" buttons
    //

    QWidget * buttonBox = new QWidget( this );
    CHECK_NEW( buttonBox );
    setCornerWidget( buttonBox, Qt::TopRightCorner );
    buttonBox->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );

    QHBoxLayout * buttonBoxLayout = new QHBoxLayout( buttonBox );
    CHECK_NEW( buttonBoxLayout );
    buttonBox->setLayout( buttonBoxLayout );
    buttonBoxLayout->setContentsMargins( 0, 0, 0, 0 );

#if VIEW_BUTTON_LEFT

    // Translators: Button with pop-up menu to open a new page (very much like
    // in a web browser) with another package filter view or to switch to an
    // existing one if it's open already

    priv->viewButton = new QPushButton( _( "&View" ), this );
    CHECK_NEW( priv->viewButton );
    setCornerWidget( priv->viewButton, Qt::TopLeftCorner );
#else
    priv->viewButton = new QPushButton( _( "&View" ), buttonBox );
    CHECK_NEW( priv->viewButton );
    buttonBoxLayout->addWidget( priv->viewButton );

#endif // VIEW_BUTTON_LEFT

    QMenu * menu = new QMenu( priv->viewButton );
    CHECK_NEW( menu );
    priv->viewButton->setMenu( menu );

    connect( menu, SIGNAL( triggered( QAction * ) ),
	     this, SLOT  ( showPage ( QAction * ) ) );

#endif // SHOW_ONLY_IMPORTANT_PAGES


    //
    // Splitter that divides the left pane into upper filters area and disk usage area
    //

    priv->leftPaneSplitter = new QSplitter( Qt::Vertical, priv->outerSplitter );
    CHECK_NEW( priv->leftPaneSplitter );


    //
    // Left pane content
    //

    priv->filtersWidgetStack = new QStackedWidget( priv->leftPaneSplitter );
    CHECK_NEW( priv->filtersWidgetStack );

    priv->diskUsageList = new YQPkgDiskUsageList( priv->leftPaneSplitter );
    CHECK_NEW( priv->diskUsageList );

    {
	QSplitter * sp = priv->leftPaneSplitter;
	sp->setStretchFactor( sp->indexOf( priv->filtersWidgetStack ), 1 );
	sp->setStretchFactor( sp->indexOf( priv->diskUsageList      ), 2 );


	// FIXME: Don't always hide the disk usage list
	QList<int> sizes;
	sizes << priv->leftPaneSplitter->height();
	sizes << 0;
	sp->setSizes( sizes );
    }


    //
    // Right pane
    //

    priv->rightPane = new QWidget( priv->outerSplitter );
    CHECK_NEW( priv->rightPane );


    //
    // Stretch factors for left and right pane
    //
    {
	QSplitter * sp = priv->outerSplitter;
	sp->setStretchFactor( sp->indexOf( priv->leftPaneSplitter ), 0 );
	sp->setStretchFactor( sp->indexOf( priv->rightPane        ), 1 );
    }


    // Set up connections

    connect( tabBar(), &QTabBar::currentChanged,
             this,     static_cast<void (YQPkgFilterTab::*)(int)>(&YQPkgFilterTab::showPage) );

    tabBar()->installEventFilter( this ); // for tab context menu


    //
    // Cosmetics
    //

    priv->baseClassWidgetStack->setContentsMargins( MARGIN,			// left
						    MARGIN + TOP_EXTRA_MARGIN,	// top
						    MARGIN,			// right
						    MARGIN );			// bottom

    priv->leftPaneSplitter->setContentsMargins	( 0,				// left
						  0,				// top
						  SPLITTER_HALF_SPACING,	// right
						  0 );				// bottom

    // priv->rightPane->setContentsMargins() is set when widgets are added to the right pane
}


YQPkgFilterTab::~YQPkgFilterTab()
{
    saveSettings();

    for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	  it != priv->pages.end();
	  ++it )
    {
	delete (*it);
    }

    priv->pages.clear();
}


QWidget *
YQPkgFilterTab::rightPane() const
{
    return priv->rightPane;
}


YQPkgDiskUsageList *
YQPkgFilterTab::diskUsageList() const
{
    return priv->diskUsageList;
}


void
YQPkgFilterTab::addPage( const QString &	pageLabel,
			 QWidget *		pageContent,
			 const QString &	internalName )
{
    YQPkgFilterPage * page = new YQPkgFilterPage( pageLabel,
						  pageContent,
						  internalName );
    CHECK_NEW( page );

    priv->pages.push_back( page );
    priv->filtersWidgetStack->addWidget( pageContent );


    if ( priv->viewButton && priv->viewButton->menu() )
    {
	QAction * action = new QAction( pageLabel, this );
	CHECK_NEW( action );
	action->setData( QVariant::fromValue( pageContent ) );

	priv->viewButton->menu()->addAction( action );
    }

#if ! SHOW_ONLY_IMPORTANT_PAGES
    page->tabIndex = tabBar()->addTab( pageLabel );
#endif
}


void
YQPkgFilterTab::showPage( QWidget * pageContent )
{
    YQPkgFilterPage * page = findPage( pageContent );
    CHECK_PTR( page );

    showPage( page );
}


void
YQPkgFilterTab::showPage( const QString & internalName )
{
    YQPkgFilterPage * page = findPage( internalName );

    if ( page )
        showPage( page );
    else
        logWarning() << "No page with ID \"" << internalName << "\"" << endl;
}


void
YQPkgFilterTab::showPage( int tabIndex )
{
    YQPkgFilterPage * page = findPage( tabIndex );

    if ( page )
	showPage( page );
}


void
YQPkgFilterTab::showPage( QAction * action )
{
    if ( ! action )
	return;

    QWidget * pageContent = action->data().value<QWidget *>();
    showPage( pageContent );
}


void
YQPkgFilterTab::showPage( YQPkgFilterPage * page )
{
    CHECK_PTR( page );
    YQSignalBlocker sigBlocker( tabBar() );

    if ( page->tabIndex < 0 ) // No corresponding tab yet?
    {
	// Add a tab for that page
	page->tabIndex = tabBar()->addTab( page->label );
    }

    priv->filtersWidgetStack->setCurrentWidget( page->content );
    tabBar()->setCurrentIndex( page->tabIndex );
    priv->tabContextMenuPage = page;

    emit currentChanged( page->content );
}


void
YQPkgFilterTab::closeAllPages()
{
    while ( tabBar()->count() > 0 )
    {
	tabBar()->removeTab( 0 );
    }

    for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	  it != priv->pages.end();
	  ++it )
    {
	(*it)->tabIndex = -1;
    }
}


void
YQPkgFilterTab::closeCurrentPage()
{
    if ( tabBar()->count() > 1 )
    {
	int currentIndex = tabBar()->currentIndex();
	YQPkgFilterPage * currentPage = findPage( currentIndex );

	if ( currentPage )
	    currentPage->tabIndex = -1;

	tabBar()->removeTab( currentIndex );

	//
	// Adjust tab index of the active pages to the right of that page
	//

	for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	      it != priv->pages.end();
	      ++it )
	{
	    YQPkgFilterPage * page = *it;

	    if ( page->tabIndex >= currentIndex )
		page->tabIndex--;
	}

	showPage( tabBar()->currentIndex() );
    }
}


YQPkgFilterPage *
YQPkgFilterTab::findPage( QWidget * pageContent )
{
    for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	  it != priv->pages.end();
	  ++it )
    {
	if ( (*it)->content == pageContent )
	    return *it;
    }

    return 0;
}


YQPkgFilterPage *
YQPkgFilterTab::findPage( const QString & internalName )
{
    for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	  it != priv->pages.end();
	  ++it )
    {
	if ( (*it)->id == internalName )
	    return *it;
    }

    return 0;
}


YQPkgFilterPage *
YQPkgFilterTab::findPage( int tabIndex )
{
    if ( tabIndex < 0 )
	return 0;

    for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	  it != priv->pages.end();
	  ++it )
    {
	if ( (*it)->tabIndex == tabIndex )
	    return *it;
    }

    return 0;
}


int
YQPkgFilterTab::tabCount() const
{
    return tabBar()->count();
}


bool
YQPkgFilterTab::eventFilter ( QObject * watchedObj, QEvent * event )
{
    if ( watchedObj == tabBar() &&
	 event && event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent * mouseEvent = dynamic_cast<QMouseEvent *> (event);

        if ( mouseEvent && mouseEvent->button() == Qt::RightButton )
	{
	    return postTabContextMenu( mouseEvent->pos() );
	}
    }

    return QTabWidget::eventFilter( watchedObj, event );
}


bool
YQPkgFilterTab::postTabContextMenu( const QPoint & pos )
{
    int tabIndex = tabBar()->tabAt( pos );

    if ( tabIndex >= 0 ) // -1 means "no tab at that position"
    {
	priv->tabContextMenuPage = findPage( tabIndex );

	if ( priv->tabContextMenuPage )
	{
	    if ( ! priv->tabContextMenu )
	    {
		// On-demand menu creation

		priv->tabContextMenu = new QMenu( this );
		CHECK_NEW( priv->tabContextMenu );

                // bool reverseLayout = YUI::yApp()->reverseLayout();
                // FIXME
                bool reverseLayout = false;


		// Translators: Change this to "right" for Arabic and Hebrew
		priv->actionMovePageLeft  = new QAction( reverseLayout ?
							 YQIconPool::arrowRight() : YQIconPool::arrowLeft(),
							 _( "Move page &left"  ), this );
		CHECK_NEW( priv->actionMovePageLeft );

		connect( priv->actionMovePageLeft, 	SIGNAL( triggered() ),
			 this,				SLOT  ( contextMovePageLeft() ) );


		// Translators: Change this to "left" for Arabic and Hebrew
		priv->actionMovePageRight = new QAction(  reverseLayout ?
							  YQIconPool::arrowLeft() : YQIconPool::arrowRight(),
							 _( "Move page &right" ), this );
		CHECK_NEW( priv->actionMovePageRight );

		connect( priv->actionMovePageRight, 	SIGNAL( triggered()   ),
			 this,				SLOT  ( contextMovePageRight() ) );


		priv->actionClosePage = new QAction( YQIconPool::tabRemove(), _( "&Close page" ), this );
		CHECK_NEW( priv->actionClosePage );

		connect( priv->actionClosePage, 	SIGNAL( triggered()   	),
			 this,				SLOT  ( contextClosePage() ) );


		priv->tabContextMenu->addAction( priv->actionMovePageLeft  );
		priv->tabContextMenu->addAction( priv->actionMovePageRight );
		priv->tabContextMenu->addAction( priv->actionClosePage     );
	    }

	    // Enable / disable actions

	    priv->actionMovePageLeft->setEnabled( tabIndex > 0 );
	    priv->actionMovePageRight->setEnabled( tabIndex < ( tabBar()->count() - 1 ) );
	    priv->actionClosePage->setEnabled( tabBar()->count() > 1 && priv->tabContextMenuPage->closeEnabled );

	    priv->tabContextMenu->popup( tabBar()->mapToGlobal( pos ) );

	    return true; // event consumed - no further processing
	}
    }

    return false; // no tab at that position
}


void
YQPkgFilterTab::contextMovePageLeft()
{
    if ( priv->tabContextMenuPage )
    {
	int contextPageIndex = priv->tabContextMenuPage->tabIndex;
	int otherPageIndex   = contextPageIndex-1;

	if ( otherPageIndex >= 0 )
	{
	    swapTabs( priv->tabContextMenuPage, findPage( otherPageIndex ) );
	}
    }
}


void
YQPkgFilterTab::contextMovePageRight()
{
    if ( priv->tabContextMenuPage )
    {
	int contextPageIndex = priv->tabContextMenuPage->tabIndex;
	int otherPageIndex   = contextPageIndex+1;

	if ( otherPageIndex < tabBar()->count() )
	{
	    swapTabs( priv->tabContextMenuPage, findPage( otherPageIndex ) );
	}
    }
}


void
YQPkgFilterTab::swapTabs( YQPkgFilterPage * page1, YQPkgFilterPage * page2 )
{
    if ( ! page1 or ! page2 )
	return;

    int oldCurrentIndex = tabBar()->currentIndex();
    std::swap( page1->tabIndex, page2->tabIndex );
    tabBar()->setTabText( page1->tabIndex, page1->label );
    tabBar()->setTabText( page2->tabIndex, page2->label );


    // If one of the two pages was the currently displayed one,
    // make sure the same page is still displayed.

    if ( oldCurrentIndex == page1->tabIndex )
    {
	YQSignalBlocker sigBlocker( tabBar() );
	tabBar()->setCurrentIndex( page2->tabIndex );
    }
    else if ( oldCurrentIndex == page2->tabIndex )
    {
	YQSignalBlocker sigBlocker( tabBar() );
	tabBar()->setCurrentIndex( page1->tabIndex );
    }
}


void
YQPkgFilterTab::contextClosePage()
{
    if ( priv->tabContextMenuPage )
    {
	int pageIndex = priv->tabContextMenuPage->tabIndex;
	priv->tabContextMenuPage->tabIndex = -1;
	tabBar()->removeTab( pageIndex );


	//
	// Adjust tab index of the active pages to the right of that page
	//

	for ( YQPkgFilterPageVector::iterator it = priv->pages.begin();
	      it != priv->pages.end();
	      ++it )
	{
	    YQPkgFilterPage * page = *it;

	    if ( page->tabIndex >= pageIndex )
		page->tabIndex--;
	}

	showPage( tabBar()->currentIndex() ); // display the new current page
    }
}


void
YQPkgFilterTab::loadSettings()
{
    closeAllPages();

    QSettings settings;
    settings.beginGroup( priv->settingsName );

    int size = settings.beginReadArray( "Tab_Pages" );

    for ( int i=0; i < size; i++ )
    {
	settings.setArrayIndex(i);
	QString id = settings.value( "Page_ID" ).toString();
	YQPkgFilterPage * page = findPage( id );

	if ( page )
	{
	    logVerbose() << "Restoring page \"" << toUTF8( id ) << "\"" << endl;
	    showPage( page );
	}
	else
	    logWarning() << "No page with ID \"" << toUTF8( id ) << "\"" << endl;
    }

    settings.endArray();

    QString id = settings.value( "Current_Page" ).toString();

    if ( ! id.isEmpty() )
	showPage( id );

    settings.endGroup();
}


void
YQPkgFilterTab::saveSettings()
{
    QSettings settings;
    settings.beginGroup( priv->settingsName );

    settings.beginWriteArray( "Tab_Pages" );

    for ( int i=0; i < tabBar()->count(); i++ )
    {
	YQPkgFilterPage * page = findPage( i );

	if ( page )
	{
	    settings.setArrayIndex(i);

	    if ( page->id.isEmpty() )
		logWarning() << "No ID for tab page \"" << page->label << "\"" << endl;
	    else
	    {
		// logDebug() << "Saving page #" << i << ": \"" << toUTF8( page->id ) << "\"" << endl;
		settings.setValue( "Page_ID", page->id );
	    }
	}
    }

    settings.endArray();

    YQPkgFilterPage * currentPage = findPage( tabBar()->currentIndex() );

    if ( currentPage )
	settings.setValue( "Current_Page", currentPage->id );

    settings.endGroup();
}



