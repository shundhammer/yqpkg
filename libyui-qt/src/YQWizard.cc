/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2022 SUSE LLC
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

  File:		YQWizard.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

  Textdomain	"qt"

/-*/


#include "YQWizard.h"

#define YUILogComponent "qt-wizard"
#include <yui/YUILog.h>

#include <yui/YApplication.h>
#include <yui/YEvent.h>
#include <yui/YReplacePoint.h>
#include <yui/YWidgetFactory.h>

#include <QFileInfo>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QMenuBar>
#include <QPixmap>
#include <QStackedWidget>
#include <QToolButton>

#include "QY2HelpDialog.h"
#include "QY2ListView.h"
#include "QY2RelNotesDialog.h"
#include "QY2Styler.h"
#include "QY2StyleSheetSelector.h"

#include "utf8.h"
#include "YQAlignment.h"
#include "YQi18n.h"
#include "YQUI.h"
#include "YQWizardButton.h"
#include "YQSignalBlocker.h"
#include "YQMainWinDock.h"


using std::string;

#ifdef TEXTDOMAIN
#    undef TEXTDOMAIN
#endif

#define TEXTDOMAIN "qt"


YQWizard * YQWizard::main_wizard              = 0;
string     YQWizard::_releaseNotesButtonId    = "";
string     YQWizard::_releaseNotesButtonLabel = "";


YQWizard::YQWizard( YWidget *		parent,
		    const string & 	backButtonLabel,
		    const string & 	abortButtonLabel,
		    const string & 	nextButtonLabel,
		    YWizardMode 	wizardMode )
    : QSplitter( Qt::Horizontal, (QWidget *) parent->widgetRep() )

    , YWizard( parent,
	       backButtonLabel,
	       abortButtonLabel,
	       nextButtonLabel,
	       wizardMode )
    , _backButtonLabel( backButtonLabel )
    , _abortButtonLabel( abortButtonLabel )
    , _nextButtonLabel( nextButtonLabel )
    , _helpDialog( NULL )
    , _hotkeysDialog( NULL )
    , _relNotesDialog( NULL )
    , _styleButtonPos( NoStyleButton )
{
    setObjectName( "wizard" );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    setWidgetRep( this );

    // either main wizard with Opt(:stepsEnabled), or sub-wizard of steps-enabled wizard
    _stepsEnabled = (wizardMode == YWizardMode_Steps);
    _treeEnabled  = (wizardMode == YWizardMode_Tree);

    _stepsRegistered    = false;
    _stepsDirty		= false;
    _direction		= YQWizard::Forward;

    _sideBar		= 0;
    _stepsPanel		= 0;
    _helpButton		= 0;
    _stepsButton	= 0;
    _treeButton		= 0;
    _releaseNotesButton = 0;
    _treePanel		= 0;
    _tree		= 0;
    _styleButton        = 0;
    _styleButton2       = 0;
    _workArea		= 0;
    _clientArea		= 0;
    _menuBar		= 0;
    _dialogIcon		= 0;
    _dialogLogo         = 0;
    _dialogBanner       = 0;
    _dialogHeading	= 0;
    _contents		= 0;
    _backButton		= 0;
    _abortButton	= 0;
    _nextButton		= 0;
    _sendButtonEvents	= true;
    _forceBanner        = false;
    _forceTitleLeft     = false;
    _contentsReplacePoint = 0;

    _previousWindowIcon = topLevelWidget()->windowIcon();

    YQUI::setTextdomain( TEXTDOMAIN );

    if ( getenv( "Y2_FORCE_BANNER" ) )
        _forceBanner = true;    // for layout debugging

    if ( getenv( "Y2_FORCE_TITLE_LEFT" ) )
        _forceTitleLeft = true; // for layout debugging

    if ( topLevelWidget()->windowTitle().isEmpty() )
    {
        topLevelWidget()->setWindowTitle ( YQUI::ui()->applicationTitle() );
        QString icon_name = QFileInfo( YUI::app()->applicationIcon().c_str() ).baseName();
        setWindowIcon ( QIcon::fromTheme( icon_name, QIcon( YUI::app()->applicationIcon().c_str() ) ) );
    }

    layout->addLayout( layoutSideBar( this ) );
    layout->addWidget( layoutWorkArea( this ) );

    setStretchFactor( indexOf( _sideBar  ), 0 );
    setStretchFactor( indexOf( _workArea ), 1 );
    setCollapsible( indexOf( _sideBar ), false );

    // If steps are enabled, we want to delay registration until after we registered steps

    if ( !_stepsEnabled )
	QY2Styler::styler()->registerWidget( this );

    if ( !main_wizard )
    {
        main_wizard = this;
    }
    else if ( main_wizard )
    {
	copySteps( main_wizard );
        YQMainWinDock::mainWinDock()->resizeVisibleChild();
    }

    if ( YQUI::ui()->fullscreen() )
        topLevelWidget()->activateWindow();
}


YQWizard::~YQWizard()
{
    deleteSteps();

    if ( this == main_wizard )
    {
	main_wizard = 0;
    }
    else if ( main_wizard )
    {
        //transfer the widget ratio to the main wizard
	main_wizard->setSizes( sizes() );
    }

    delete _helpDialog;
    delete _hotkeysDialog;
    delete _relNotesDialog;

    QY2Styler::styler()->unregisterWidget( this );
    topLevelWidget()->setWindowIcon( _previousWindowIcon );
}


bool YQWizard::isSecondary() const
{
    return this != main_wizard;
}


void YQWizard::layoutTitleBar( QWidget * parent )
{
    QFrame * titleBar = new QFrame( parent );
    YUI_CHECK_NEW( titleBar );

    QHBoxLayout *layout = new QHBoxLayout( titleBar );
    titleBar->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert

    //
    // Left logo
    //

    QLabel * left = new QLabel( titleBar );
    layout->addWidget( left );
    left->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert
    left->setObjectName( "titleBar-left" );

    //
    // Center stretch space
    //

    layout->addStretch( 10 );


    //
    // Right logo
    //

    QLabel * right = new QLabel( titleBar );
    YUI_CHECK_NEW( right );

    layout->addWidget( right );
    right->setObjectName( "titleBar-right" );
}


QLayout *YQWizard::layoutSideBar( QWidget * parent )
{
    _sideBar = new QStackedWidget( parent );
    YUI_CHECK_NEW( _sideBar );
    // _sideBar->setMinimumWidth( YQUI::ui()->defaultSize( YD_HORIZ ) / 5 );
    _sideBar->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred ) ); // hor/vert
    _sideBar->setObjectName( QString( "_sideBar-%1" ).arg( long( this ) ) );
    _sideBar->installEventFilter( this );

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget( _sideBar );

    if ( _treeEnabled )
    {
	layoutTreePanel();
	showTree();
    }
    else if ( _stepsEnabled )
    {
	layoutStepsPanel();
	showSteps();
    } else {
        _sideBar->hide();
    }

    return vbox;
}


void YQWizard::layoutStepsPanel()
{
    // Steps
    _stepsPanel = new QFrame( _sideBar );
    _sideBar->addWidget( _stepsPanel );
    _sideBar->setObjectName( "steps" );
    _sideBar->setProperty( "class", "steps" );
    QY2Styler::styler()->registerChildWidget( this, _stepsPanel );

    _stepsDirty = true; // no layout yet
}


void YQWizard::addStep( const string & text, const string & id )
{
    QString qId = fromUTF8( id );

    if ( _stepsIDs[ qId ] )
    {
	yuiError() << "Step ID \"" << id << "\" (\"" << text
		   <<"\") already used for \"" << _stepsIDs[ qId ]->name() <<"\""
		   << endl;
	return;
    }

    if ( !_stepsList.empty() && _stepsList.last()->name() == fromUTF8( text ) )
    {
	// Consecutive steps with the same name will be shown as one single step.
	//
	// Since steps are always added at the end of the list, it is
	// sufficient to check the last step of the list. If the texts are the
	// same, the other with the same text needs to get another (additional)
	// ID to make sure setCurrentStep() works as it should.
	_stepsList.last()->addID( qId );
    }
    else
    {
	_stepsList.append( new YQWizard::Step( fromUTF8( text ), qId ) );
	_stepsDirty = true;
    }

    _stepsIDs.insert( qId, _stepsList.last() );

    // make sure we always have a current step if we have steps
    if ( _currentStepID.isNull() )
        _currentStepID = qId;
}


void YQWizard::addStepHeading( const string & text )
{
    _stepsList.append( new YQWizard::StepHeading( fromUTF8( text ) ) );
    _stepsDirty = true;
}


void YQWizard::updateSteps()
{
    if ( ! _stepsPanel )
	return;

    yuiDebug() << "updateSteps" << endl;

    if ( !_stepsRegistered )
        setUpdatesEnabled(false);

    // Create a grid layout for the steps
    delete _stepsPanel->layout();
    _stepsPanel->setMaximumWidth( 65000 );

    QVBoxLayout *_stepsVBox = new QVBoxLayout( _stepsPanel );

    QGridLayout *_stepsGrid = new QGridLayout();
    _stepsGrid->setObjectName( QString( "_stepsGrid_%1" ).arg(  long( this ) ) );
    YUI_CHECK_NEW( _stepsGrid );
    _stepsVBox->addLayout( _stepsGrid );
    _stepsVBox->setMargin(0);
    _stepsGrid->setSpacing(0);

    const int statusCol = 1;
    const int nameCol	= 2;

    int row = 0;

    //
    // Create widgets for all steps and step headings in the internal list
    //

    for ( QList<Step*>::iterator i = _stepsList.begin(); i != _stepsList.end(); ++i)
    {
        YQWizard::Step * step = *i;

        step->deleteLabels();

	if ( step->isHeading() )
	{
	    //
	    // Heading
	    //

            yuiDebug() << "Adding StepHeading \"" << step->name() << "\"" << endl;
	    QLabel * label = new QLabel( step->name(), _stepsPanel );
	    YUI_CHECK_NEW( label );
            label->setObjectName( step->name() );
	    label->setAlignment( Qt::AlignLeft | Qt::AlignTop );
            label->setProperty( "class", "steps_heading" );

	    step->setNameLabel( label );
	    _stepsGrid->addWidget( label,
                                   row, statusCol,
                                   1, nameCol - statusCol + 1);
        }
	else	// No heading - ordinary step
	{
	    //
	    // Step status
	    //

            yuiDebug() << "Adding Step \"" << step->name() << "\"" << endl;

	    QLabel * statusLabel = new QLabel( _stepsPanel );
	    YUI_CHECK_NEW( statusLabel );

	    step->setStatusLabel( statusLabel );
            statusLabel->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	    _stepsGrid->addWidget( statusLabel, row, statusCol );

	    //
	    // Step name
	    //

	    QLabel * nameLabel = new QLabel( step->name(), _stepsPanel );
	    YUI_CHECK_NEW( nameLabel );
	    nameLabel->setAlignment( Qt::AlignLeft | Qt::AlignTop );
            nameLabel->setObjectName( step->name() );

	    step->setNameLabel( nameLabel );
            _stepsGrid->addWidget( nameLabel, row, nameCol );
        }

        step->setStatus( Step::Todo );
	row++;
    }

    _stepsVBox->addStretch( 99 );
    QVBoxLayout *rbl = new QVBoxLayout();
    rbl->addWidget( (QWidget *) _releaseNotesButton->widgetRep(), 0, Qt::AlignCenter );

    _stepsVBox->addLayout( rbl );
    _stepsVBox->addStretch( 1 );

    _stepsDirty = false;

    if ( !_stepsRegistered )
    {
	QY2Styler::styler()->registerWidget( this );
        setUpdatesEnabled( true );
	QY2Styler::styler()->updateRendering( this );
        _stepsRegistered = true;
    }
}


void YQWizard::updateStepStates()
{
    yuiDebug() << "steps dirty: " << _stepsDirty << endl;

    if ( _stepsDirty )
        updateSteps();

    YQWizard::Step * currentStep = findStep( _currentStepID );
    QList<YQWizard::Step*>::iterator step = _stepsList.begin();

    if ( currentStep )
    {
	// Set status icon and color for the current step
	currentStep->setStatus( Step::Current );

	//
	// Set all steps before the current to "done"
	//

	while ( step != _stepsList.end() && *step != currentStep )
	{
	    ( *step )->setStatus( Step::Done );
	    step++;
	}

	// Skip the current step - continue with the step after it

	if ( step != _stepsList.end() )
	    step++;
    }

    //
    // Set all steps after the current to "to do"
    //

    while ( step != _stepsList.end() )
    {
	( *step )->setStatus( Step::Todo );
	step++;
    }
}


void YQWizard::setCurrentStep( const string & id )
{
    yuiDebug() << "Setting current step to \"" << id << "\"" << endl;

    _currentStepID = fromUTF8( id );
    updateStepStates();
}


void YQWizard::copySteps( YQWizard *wizard)
{
    QList<Step*> _oldSteps = wizard->stepsList();

    if (_oldSteps.empty())
	return;

    foreach ( Step *oldStep, _oldSteps)
    {
        Step *newStep;

        if ( !oldStep->isHeading() )
            newStep = new Step( oldStep->name());
        else
  	    newStep = new StepHeading( oldStep->name());

	foreach ( QString oneId, oldStep->id())
	{
            newStep->addID( oneId);
	    _stepsIDs.insert( oneId, newStep );
   	}

   	newStep->setEnabled( oldStep->isEnabled());
	_stepsList.append(newStep);

    }

    setCurrentStep(  wizard->currentStep().toStdString() );
    setSizes( main_wizard->sizes());
}


void YQWizard::deleteSteps()
{
    yuiDebug() << "Deleting steps" << endl;

    if ( _stepsPanel )
        _stepsPanel->setFixedWidth( _stepsPanel->width() );

    qDeleteAll(_stepsList);
    _stepsList.clear();
    _stepsIDs.clear();
    _currentStepID = QString();
    _stepsDirty = true;
}


YQWizard::Step * YQWizard::findStep( const QString & id )
{
    if ( id.isEmpty() )
	return 0;

    return _stepsIDs[ id ];
}


void YQWizard::layoutTreePanel()
{
    _treePanel = new QFrame( _sideBar );
    YUI_CHECK_NEW( _treePanel );
    QHBoxLayout *layout = new QHBoxLayout( _treePanel );
    _sideBar->addWidget( _treePanel );

    QVBoxLayout * vbox = new QVBoxLayout();
    YUI_CHECK_NEW( vbox );
    layout->addLayout( vbox );

    // Selection tree

    _tree = new QY2ListView( _treePanel );
    YUI_CHECK_NEW( _tree );
    vbox->addWidget( _tree );

    _tree->header()->hide();
    _tree->header()->setSectionResizeMode( 0, QHeaderView::Stretch );

    _tree->setRootIsDecorated( true );
    _tree->setSortByInsertionSequence( true );

    connect( _tree,	&pclass(_tree)::itemSelectionChanged,
	     this,	&pclass(this)::treeSelectionChanged );

    connect( _tree,	&pclass(_tree)::itemDoubleClicked,
	     this,	&pclass(this)::sendTreeEvent );

}


void YQWizard::addTreeItem( const string & parentID, const string & text, const string & id )
{
    QString qId = fromUTF8( id );

    if ( ! _tree )
    {
	yuiError() << "YQWizard widget not created with Opt(:treeEnabled) !" << endl;
	return;
    }

    YQWizard::TreeItem * item	= 0;
    YQWizard::TreeItem * parent = 0;

    if ( ! parentID.empty() )
    {
	parent = findTreeItem( parentID );
    }

    if ( parent )
    {
	item = new YQWizard::TreeItem( parent, fromUTF8( text ), qId );
	YUI_CHECK_NEW( item );
    }
    else
    {
	item = new YQWizard::TreeItem( _tree, fromUTF8( text ), qId );
	YUI_CHECK_NEW( item );
    }

    if ( ! qId.isEmpty() )
	_treeIDs.insert( qId, item );
}



void YQWizard::deleteTreeItems()
{
    if ( _tree )
	_tree->clear();

    _treeIDs.clear();
}



YQWizard::TreeItem * YQWizard::findTreeItem( const string & id )
{
    if ( id.empty() )
	return 0;

    return _treeIDs[ fromUTF8( id ) ];
}


void YQWizard::selectTreeItem( const string & id )
{
    if ( _tree )
    {
	YQWizard::TreeItem * item = findTreeItem( id );

	if ( item )
	{
	    YQSignalBlocker sigBlocker( _tree );

            _tree->setCurrentItem(item);
	    _tree->scrollToItem(item);
	}
    }
}


void YQWizard::sendTreeEvent( QTreeWidgetItem * listViewItem )
{
    if ( listViewItem )
    {
	YQWizard::TreeItem * item = dynamic_cast<YQWizard::TreeItem *> ( listViewItem );

	if ( item && ! item->id().isEmpty() )
	    sendEvent( toUTF8( item->id() ) );
    }
}


void YQWizard::treeSelectionChanged()
{
    if ( _tree )
	sendTreeEvent( _tree->currentItem() );
}


string YQWizard::currentTreeSelection()
{
    if ( _tree )
    {
	QTreeWidgetItem * sel = _tree->currentItem();

	if ( sel )
	{
	    YQWizard::TreeItem * item = dynamic_cast<YQWizard::TreeItem *> (sel);

	    if ( item && ! item->id().isEmpty() )
		return toUTF8( item->id() );
	}
    }

    return string();
}



QWidget * YQWizard::layoutWorkArea( QWidget * parent )
{
    _workArea = new QFrame( parent );
    YUI_CHECK_NEW( _workArea );
    _workArea->setObjectName( "WorkArea" );

    QVBoxLayout * workAreaVBox = new QVBoxLayout( _workArea );
    YUI_CHECK_NEW( workAreaVBox );

    if ( useBanner() )
    {
        //
        // Banner area at the top
        // with a product logo on the left and an optional banner text on the right
        // (the machine name/type for s/390)
        //

        QWidget * logoWidget = new QWidget;
        logoWidget->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) ); // hor/vert
        logoWidget->setObjectName( "LogoHBox" );
        workAreaVBox->addWidget( logoWidget );

	QHBoxLayout * logoHBox = new QHBoxLayout( logoWidget );
        YUI_CHECK_NEW( logoHBox );

        //
        // Add the logo at the top
        //

        _dialogLogo = new QLabel( _workArea );
        YUI_CHECK_NEW( _dialogLogo );

        logoHBox->addWidget( _dialogLogo );
        _dialogLogo->setObjectName( "DialogLogo" );
	_dialogLogo->setAlignment( Qt::AlignLeft );
        QY2Styler::styler()->registerChildWidget( this, _dialogLogo );
        _dialogLogo->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) ); // hor/vert
	_dialogLogo->setMinimumHeight( 59 ); // FIXME: control size via stylesheet, did not find how
	_dialogLogo->setMinimumWidth( 100 );

        logoHBox->addStretch();

        _dialogBanner = new QLabel( _workArea );
        _dialogBanner->setText( QString( getenv( "YAST_BANNER" ) ) );
        YUI_CHECK_NEW( _dialogBanner );
        logoHBox->addWidget( _dialogBanner );
        _dialogBanner->setObjectName( "DialogBanner" );
        _dialogBanner->setAlignment( Qt::AlignCenter );
        QY2Styler::styler()->registerChildWidget( this, _dialogBanner );

        if ( titleIsOnTheLeft() && ! _styleButton )
        {
            // "Change Widget Style" button
            //
            // If the wizard title is on the left, e.g. in the SLE installation
            // theme, we need a different place for the "Change Widget Style"
            // button.
            //
            // If there is a banner at the top, let's put it at the very right
            // of that banner; but beware that in some scenarios (e.g. s/390)
            // there is the machine type also in the top right of the banner
            // area, so we need to maintain some distance from it.
            //
            // https://documentation.suse.com/sles/15-SP3/html/SLES-all/cha-install.html#sec-yast-install-s390-part

            logoHBox->addSpacing( 10 );

            _styleButton = addStyleButton( _workArea );
            logoHBox->addWidget( _styleButton );
            _styleButtonPos = StyleButtonInLogoBanner;
        }
    }


    //
    // Wizard menu bar
    // (not to be confused with the newer YMenuBar used e.g. in the partitioner)
    //
    // This is rarely used these days; only (?) in the AutoYaST config module.
    //

    _menuBar = new QMenuBar( _workArea );
    YUI_CHECK_NEW( _menuBar );

    _menuBar->hide(); // will be made visible when menus are added
    workAreaVBox->addWidget( _menuBar );


    //
    // Inner dialog area / client area
    //

    QWidget * dialogInnerArea = new QWidget( _workArea );
    dialogInnerArea->setObjectName( "work_area" );

    QY2Styler::styler()->registerChildWidget( this, dialogInnerArea );
    QVBoxLayout * innerAreaVBox = new QVBoxLayout( dialogInnerArea );
    YUI_CHECK_NEW( innerAreaVBox );
    workAreaVBox->addWidget( dialogInnerArea );

    QVBoxLayout * innerVBox = new QVBoxLayout();
    YUI_CHECK_NEW( innerVBox );

    QVBoxLayout * leftInnerVBox  = innerVBox;
    QVBoxLayout * rightInnerVBox = innerVBox;

    innerVBox->setMargin( YQWidgetMargin  );
    innerAreaVBox->addLayout( innerVBox );
    workAreaVBox->setMargin( 0 );


    //
    // Dialog icon and heading
    // at the left or at the top
    //

    if ( titleIsOnTheLeft() )
    {
        QHBoxLayout * bigHBox = new QHBoxLayout();
        innerVBox->addLayout( bigHBox );

        leftInnerVBox = new QVBoxLayout();
        leftInnerVBox->setObjectName( "LeftInnerBox" );
        bigHBox->addLayout( leftInnerVBox );
        bigHBox->setStretchFactor( leftInnerVBox, 1 );

        rightInnerVBox = new QVBoxLayout();
        rightInnerVBox->setObjectName( "RightInnerBox" );
        bigHBox->addLayout( rightInnerVBox );
        bigHBox->setStretchFactor( rightInnerVBox, 2 );
    }

    QHBoxLayout * headingHBox = new QHBoxLayout();
    YUI_CHECK_NEW( headingHBox );
    leftInnerVBox->addLayout( headingHBox );

    _dialogIcon = new QLabel( _workArea );
    YUI_CHECK_NEW( _dialogIcon );
    headingHBox->addWidget( _dialogIcon );
    _dialogIcon->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) ); // hor/vert
    _dialogIcon->setObjectName( "DialogIcon" );
    _dialogIcon->hide();

    _dialogHeading = new QLabel( _workArea );
    YUI_CHECK_NEW( _dialogHeading );
    headingHBox->addWidget( _dialogHeading );
    _dialogHeading->setWordWrap( true );
    _dialogHeading->setTextFormat( Qt::PlainText );
    _dialogHeading->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) ); // hor/vert
    _dialogHeading->setObjectName( titleIsOnTheLeft() ? "DialogHeadingLeft" : "DialogHeadingTop" ) ;

    if ( ! titleIsOnTheLeft() && ! _styleButton )
    {
        // "Change Widget Style" button
        //
        // In most cases we want the "Change Style" button in the top right
        // corner of the wizard. If we don't already have one, let's use the
        // rightmost part of that row where the dialog (wizard) icon and title
        // are; there is empty space anyway.
        //
        // Exception: The SLE installation theme where the dialog title is in
        // very large font in the left third of the wizard dialog. That would
        // put the "Change Style" button in the center of the screen where it
        // would be very much misplaced.
        //
        // Since the SLE installation theme also has a big banner at the top
        // with a SUSE logo and name on the left and optionally (on s/390) a
        // machine type on the right, that is the preferred location for this
        // button. In that case, it's already created and _styleButton is
        // non-null.
        //
        // If that also didn't work out because there is no banner at the top,
        // the button will go to the right of the [Help] button in the button
        // box at the bottom of the wizard.

        _styleButton = addStyleButton( _workArea );
        headingHBox->addWidget( _styleButton );
        _styleButtonPos = StyleButtonRightOfDialogHeading;
    }


    //
    // Client area (the part that belongs to the YCP application)
    //

    layoutClientArea( _workArea );
    rightInnerVBox->addWidget( _clientArea );


    //
    // Button box
    //

    QLayout * buttonBox = layoutButtonBox( _workArea );
    innerVBox->addLayout( buttonBox );

    return _workArea;
}



void YQWizard::layoutClientArea( QWidget * parent )
{
    _clientArea = new QFrame( parent );
    YUI_CHECK_NEW( _clientArea );
    _clientArea->setObjectName( "_clientArea" );
    QVBoxLayout *layout = new QVBoxLayout( _clientArea );
    layout->setMargin( 0 );

    //
    // HVCenter for wizard contents
    //

    _contents = new YQAlignment( this, _clientArea, YAlignCenter, YAlignCenter );
    YUI_CHECK_NEW( _contents );
    layout->addWidget( _contents );
    _contents->QObject::setProperty( "class", "Contents" );

    _contents->setStretchable( YD_HORIZ, true );
    _contents->setStretchable( YD_VERT,	 true );
    _contents->installEventFilter( this );
    _contents->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) ); // hor/vert

    //
    // Replace point for wizard contents
    //

    _contentsReplacePoint = YUI::widgetFactory()->createReplacePoint( _contents );

    //
    // Initial YEmpty widget contents of replace point
    //

    YUI::widgetFactory()->createEmpty( _contentsReplacePoint );
    _contentsReplacePoint->showChild();
}


QLayout *YQWizard::layoutButtonBox( QWidget * parent )
{
    //
    // QHBoxLayout for the buttons
    //

    QHBoxLayout * hbox = new QHBoxLayout();
    YUI_CHECK_NEW( hbox );

    hbox->setSpacing( 0 );
    hbox->setMargin( 0 );

    // Help button
    // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
    _helpButton = new YQWizardButton( this, parent, _( "&Help" ).toStdString());
    YUI_CHECK_NEW( _helpButton );

    connect( _helpButton, &pclass(_helpButton)::clicked,
	     this,	  &pclass(this)::showHelp );

    hbox->addWidget( (QWidget *) _helpButton->widgetRep() );

    // Help action to be able to react to F1 and Alt-H (bnc#973389)
    _helpAction = new QAction( this );
    _helpAction->setShortcut( Qt::Key_F1 );
    addAction( _helpAction );

    connect( _helpAction, &pclass( _helpAction )::triggered,
             this,        &pclass( this )::showHelp );

    // Help action to be able to react to Shift-F1 to show hotkeys
    _hotkeysAction = new QAction( this );
    _hotkeysAction->setShortcut( Qt::ShiftModifier + Qt::Key_F1 );
    addAction( _hotkeysAction );

    connect( _hotkeysAction, &pclass( _hotkeysAction )::triggered,
             this,           &pclass( this )::showHotkeys );

    hbox->addSpacing( 10 );

    //
    // "Release Notes" button
    //

    // Release Notes button
    // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
    _releaseNotesButton = new YQWizardButton( this, parent, _( "&Release Notes" ).toStdString ());
    YUI_CHECK_NEW( _releaseNotesButton );
    hbox->addWidget( (QWidget *) _releaseNotesButton->widgetRep() );
    connect( _releaseNotesButton, &pclass( _releaseNotesButton )::clicked,
             this,                &pclass( this )::showReleaseNotes );


    if (_releaseNotesButtonId == "")
    {
	_releaseNotesButton->hide();    // hidden until showReleaseNotesButton() is called
    }
    else
    {
	showReleaseNotesButton( _releaseNotesButtonLabel, _releaseNotesButtonId );
    }


    if ( ! _styleButton )       // if not already created
    {
        // "Change Widget Style" button
        //
        // This is the last-ditch effort to place the "Change Widget Style"
        // button somewhere: There was no digalog title (wizard title) at the
        // top, and despite a dialog title on the left (i.e. the SLE
        // installation theme), there was no banner at the top. So let's put it
        // here, next to the [Help] and (if present) [Release Notes] button.
        //
        // While this place is not ideal, here it doesn't get in the way of dialog
        // content.

        hbox->addSpacing( 10 );
        _styleButton = addStyleButton( parent );
        hbox->addWidget( _styleButton );
        _styleButtonPos = StyleButtonRightOfHelpButton;
    }
    else if ( _styleButtonPos == StyleButtonRightOfDialogHeading )
    {
        // If we already have a "Change Widget Style" button, but it is to the
        // right of the dialog heading, create an alternate (but hidden) one to
        // the right of the [Help] button: If during the wizard steps the one
        // at the top is hidden because the dialog title is hidden, this one
        // will be shown instead.
        //
        // As soon as there is a dialog title again, the primary one is shown
        // again and this alternate one is hidden again, so there is always
        // exactly one "Change Widget Style" button visible.
        //
        // This is relevant for menu-driven wizard dialogs like the partitioner
        // and others with large table widgets that need a lot of screen space.

        hbox->addSpacing( 10 );
        _styleButton2 = addStyleButton( parent );
        hbox->addWidget( _styleButton2 );
        _styleButton2->hide();
    }

    hbox->addStretch( 10 );



    //
    // "Abort" button
    //

    _abortButton = new YQWizardButton( this, parent, _abortButtonLabel );
    YUI_CHECK_NEW( _abortButton );

    hbox->addWidget( (QWidget *) _abortButton->widgetRep() );
    connect( _abortButton,	&pclass(_abortButton)::clicked,
	     this,		&pclass(this)::slotAbortClicked );

    hbox->addSpacing( 10 );

    //
    // "Back" button
    //

    _backButton	 = new YQWizardButton( this, parent, _backButtonLabel );
    YUI_CHECK_NEW( _backButton );

    hbox->addWidget( (QWidget *) _backButton->widgetRep() );
    connect( _backButton,	&pclass(_backButton)::clicked,
	     this,		&pclass(this)::slotBackClicked );

    if ( _backButton->text().isEmpty() )
	_backButton->hide();

    //
    // "Next" button
    //

    hbox->addSpacing( 5 );

    _nextButton	 = new YQWizardButton( this, parent, _nextButtonLabel );
    YUI_CHECK_NEW( _nextButton );

    hbox->addWidget( (QWidget *) _nextButton->widgetRep() );
    connect( _nextButton,	&pclass(_nextButton)::clicked,
	     this,		&pclass(this)::slotNextClicked );

    return hbox;
}


QToolButton * YQWizard::addStyleButton( QWidget * parent )
{
    QString styleSheet( "QToolButton#styleButton { border: 0px }" );

    QToolButton * button = new QToolButton( parent );
    YUI_CHECK_NEW( button );

    button->setObjectName( "styleButton" );
    button->setIcon( QIcon::fromTheme( ":day-night-mode" ) );
    button->setIconSize( QSize( 28, 28 ) );
    button->setAutoRaise( true );
    button->setStyleSheet( styleSheet );
    button->setToolTip( _( "Change the widget theme (Shift-F3)" ) );

    connect( button, &pclass( button )::clicked,
             this,   &pclass( this )::askForWidgetStyle );

    return button;
}


void YQWizard::askForWidgetStyle()
{
    // not using YQUI::yqApp()->askForWidgetStyle() to avoid busy cursor issues

    QY2StyleSheetSelector dialog( this );
    dialog.exec();
    // The return code doesn't matter because the dialog applies any changes instantly.
}


bool YQWizard::titleIsOnTheLeft() const
{
    if ( _forceTitleLeft )  // env Y2_FORCE_TITLE_LEFT
        return true;

    return wizardMode() == YWizardMode_TitleOnLeft;
}


bool YQWizard::useBanner() const
{
    if ( _forceBanner )  // env Y2_FORCE_BANNER
        return true;

    return YUI::application()->showProductLogo();
}


void YQWizard::destroyButtons()
{
    delete _backButton;
    _backButton = 0;

    delete _abortButton;
    _abortButton = 0;

    delete _nextButton;
    _nextButton = 0;
}


void YQWizard::connectNotify ( const char * signal )
{
    if ( QString( signal ).contains( "nextClicked()" ) )
    {
	yuiDebug() << "nextClicked connected, no longer directly sending button events" << endl;
	_sendButtonEvents = false;
    }
}


void YQWizard::disconnectNotify ( const char * signal )
{
    if ( QString( signal ).contains( "nextClicked()" ) )
    {
	yuiDebug() << "nextClicked disconnected, directly sending button events again" << endl;
	_sendButtonEvents = true;
    }
}


void YQWizard::setDialogIcon( const string & iconName )
{
    if ( _dialogIcon )
    {
	if ( ! iconName.empty() )
	{
	    QPixmap icon( iconName.c_str() );

	    if ( icon.isNull() )
	    {
	        _dialogIcon->hide();
		yuiWarning() << "Couldn't load dialog icon \"" << iconName << "\"" << endl;
	    }
	    else
	    {
		_dialogIcon->show();
		_dialogIcon->setPixmap( icon );
		topLevelWidget()->setWindowIcon( icon );
	    }
	}
	else
	{
	    _dialogIcon->hide();
	    _dialogIcon->clear();
	    topLevelWidget()->setWindowIcon( QIcon() );
	}
    }
}


void YQWizard::setDialogTitle( const string & titleText )
{
    QString title = fromUTF8( titleText.c_str() );

    if ( !title.isEmpty() )
	topLevelWidget()->setWindowTitle( YQUI::ui()->applicationTitle() + QString(" - ") + title  );
    else
	topLevelWidget()->setWindowTitle( YQUI::ui()->applicationTitle() );
}


string YQWizard::getDialogTitle()
{
    return toUTF8( topLevelWidget()->windowTitle() );
}


void YQWizard::setDialogHeading( const string & headingText )
{
    if ( _dialogHeading )
    {
	if ( ! headingText.empty() )
	{
	    _dialogHeading->show();
	    _dialogHeading->setText( fromUTF8( headingText ) );
	}
	else
	{
	    _dialogHeading->hide();
	    _dialogHeading->clear();
	}

        if ( _styleButton && _styleButtonPos == StyleButtonRightOfDialogHeading )
        {
            // If we have a "Change Widget Style" button and it's to the right
            // of the dialog heading, make sure it's shown or hidden along with
            // the dialog heading. It's better to not have a style button in
            // one or two wizard steps than artificially adding a lot of empty
            // space at the top of the wizard content; for example in
            // menu-driven wizard steps (e.g. the partitioner) that have a menu
            // bar at the top, but no wizard heading to save space.

            _styleButton->setVisible( _dialogHeading->isVisible() );

            // Make sure we have exactly one "Change Widget Style" button
            // visible. If we just made the one to the right of the dialog
            // heading invisible, make the alternate one to the right of the
            // "Help" button visible instead; and vice versa.

            if ( _styleButton2 )
                _styleButton2->setVisible( ! _styleButton->isVisible() );
        }
    }
}

string YQWizard::getDialogHeading()
{
    if ( _dialogHeading )
        return toUTF8(_dialogHeading->text());
    else
        return "";
}

string YQWizard::debugLabel() const
{
    if ( _dialogHeading )
    {
	QString label = _dialogHeading->text();
	label = label.simplified(); // Replace any embedded newline with a single blank

	if ( ! label.isEmpty() )
	    return toUTF8( label );
    }

    return "untitled YQWizard";
}


void YQWizard::setHelpText( const string & helpText )
{
    _qHelpText = fromUTF8( helpText );
    _qHelpText.replace( "&product;", fromUTF8( YUI::app()->productName() ) );
}


void YQWizard::slotBackClicked()
{
    emit backClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _backButton, YEvent::Activated ) );

    _direction = YQWizard::Backward;
}


void YQWizard::slotAbortClicked()
{
    emit abortClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _abortButton, YEvent::Activated ) );
}


void YQWizard::slotNextClicked()
{
    emit nextClicked();

    if ( _sendButtonEvents )
	YQUI::ui()->sendEvent( new YWidgetEvent( _nextButton, YEvent::Activated ) );

    _direction = YQWizard::Forward;
}


void YQWizard::showHelp()
{
    if ( !_helpDialog )
	_helpDialog = new QY2HelpDialog ( _qHelpText, NULL );
    else
    {
	_helpDialog->setHelpText( _qHelpText );
	_helpDialog->hide(); // workaround for icewm (see: bnc #397083)
    }

    _helpDialog->show();
    _helpDialog->raise();
    _helpDialog->activateWindow();
}


void YQWizard::showHotkeys()
{
    /**
     * Help text to be shown after pressing Shift-F1 listing the advanced
     * keyboard shortcuts available in the Qt-UI
     **/
    _qHotkeysText = _(
        "<h1>Advanced Hotkeys</h1>"
        "<dl>"
        "<dt>Print Screen</dt>"
        "<dd>Take and save a screenshot. May not be available when YaST is running under "
        "some desktop environments.</dd>"
        "<dt>Shift-F3</dt>"
        "<dd>Select a widget style (color theme).</dd>"
        "<dt>Shift-F4</dt>"
        "<dd>Enable/disable the color palette optimized for vision impaired users.</dd>"
        "<dt>Shift-F7</dt>"
        "<dd>Enable/disable logging of debug messages.</dd>"
        "<dt>Shift-F8</dt>"
        "<dd>Open a file dialog to save log files to a non-standard location.</dd>"
        "<dt>Ctrl-Shift-Alt-D</dt>"
        "<dd>Send a DebugEvent. YaST modules can react on this by executing "
        "special debugging actions. Result depends on the specific YaST-module.</dd>"
        "<dt>Ctrl-Shift-Alt-M</dt>"
        "<dd>Start/Stop macro recorder.</dd>"
        "<dt>Ctrl-Shift-Alt-P</dt>"
        "<dd>Replay macro.</dd>"
        "<dt>Ctrl-Shift-Alt-S</dt>"
        "<dd>Show style sheet editor.</dd>"
        "<dt>Ctrl-Shift-Alt-T</dt>"
        "<dd>Dump widget tree to the log file.</dd>"
        "<dt>Ctrl-Alt-Shift-X</dt>"
        "<dd>Open a terminal window (xterm). Useful for VNC installations.</dd>"
        "<dt>Ctrl-Shift-Alt-Y</dt>"
        "<dd>Show widget tree browser.</dd>"
        "</dl>"
        );

    if (!_hotkeysDialog)
	_hotkeysDialog = new QY2HelpDialog ( _qHotkeysText , NULL );

    _hotkeysDialog->show();
    _hotkeysDialog->raise();
    _hotkeysDialog->activateWindow();
}


void YQWizard::showReleaseNotes()
{
    if (!_relNotesDialog)
	_relNotesDialog = new QY2RelNotesDialog ( NULL );
    else
    {
	_relNotesDialog->hide(); // workaround for icewm (see: bnc #397083)
    }

    std::map<string,string> relnotes = YUI::application()->releaseNotes();

    if ( relnotes.size() == 0)
    {
        return;
    }
    _relNotesDialog->setRelNotes( relnotes );
    _relNotesDialog->show();
    _relNotesDialog->raise();
    _relNotesDialog->activateWindow();
}


void YQWizard::showSteps()
{
    if ( _sideBar && _stepsPanel )
    {
	_sideBar->setCurrentWidget( _stepsPanel );
    }
}


void YQWizard::showTree()
{
    if ( _sideBar && _treePanel )
    {
	_sideBar->setCurrentWidget( _treePanel );
    }
}


void YQWizard::addMenu( const string & text,
			const string & id )
{
    if ( _menuBar )
    {
	QMenu * menu = new QMenu( _menuBar );
	YUI_CHECK_NEW( menu );

	_menuIDs.insert( fromUTF8( id ), menu );
        _menuBar->addMenu( menu );
        menu->setTitle( fromUTF8( text ) );

	connect( menu, &pclass(menu)::triggered,
		 this, &pclass(this)::sendMenuEvent );

        _menuBar->show();
    }
}


void YQWizard::addSubMenu( const string & parentMenuID,
			   const string & text,
			   const string & id )
{
    QMenu* parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	QMenu * menu = new QMenu( _menuBar );
	YUI_CHECK_NEW( menu );

	_menuIDs.insert( fromUTF8( id ), menu );
	//FIXME parentMenu->insertItem( fromUTF8( text ), menu );

	connect( menu, &pclass(menu)::triggered,
		 this, &pclass(this)::sendMenuEvent );
    }
    else
    {
	yuiError() << "Can't find menu with ID " << parentMenuID << endl;
    }
}


void YQWizard::addMenuEntry( const string & parentMenuID,
			     const string & text,
			     const string & idString )
{
    QMenu * parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	QAction *action;
        action = parentMenu->addAction( fromUTF8( text ) );
	_menuEntryIDs[ action ] = idString ;

    }
    else
    {
	yuiError() << "Can't find menu with ID " << parentMenuID << endl;
    }
}


void YQWizard::addMenuSeparator( const string & parentMenuID )
{
    QMenu * parentMenu = _menuIDs[ fromUTF8( parentMenuID ) ];

    if ( parentMenu )
    {
	parentMenu->addSeparator();
    }
    else
    {
	yuiError() << "Can't find menu with ID " << parentMenuID << endl;
    }
}


void YQWizard::deleteMenus()
{
    if ( _menuBar )
    {
	_menuBar->hide();
	_menuBar->clear();
	_menuIDs.clear();
	_menuEntryIDs.clear();
    }
}


void YQWizard::sendMenuEvent( QAction *action )
{
    if (  _menuEntryIDs.contains( action ) )
    {
	sendEvent( _menuEntryIDs[ action ] );
    }
    else
    {
	yuiError() << "Invalid menu ID " <<  endl;
    }
}


void YQWizard::sendEvent( const string & id )
{
    YQUI::ui()->sendEvent( new YMenuEvent( id ) );
}


int YQWizard::preferredWidth()
{
    return sizeHint().width();
}


int YQWizard::preferredHeight()
{
    return sizeHint().height();
}


void YQWizard::setSize( int newWidth, int newHeight )
{
    resize( newWidth, newHeight );
    resizeClientArea();
}


void YQWizard::resizeClientArea()
{
    QSize contentsRect = _clientArea->contentsRect().size();
    _contents->setSize( contentsRect.width(), contentsRect.height() );
}


bool YQWizard::eventFilter( QObject * obj, QEvent * ev )
{
    if ( ev->type() == QEvent::Resize && obj == _contents )
    {
        resizeClientArea();
        return true;		// Event handled
    }

    if ( ev->type() == QEvent::Resize && obj == _sideBar && main_wizard == this && _stepsPanel )
    {
        YQMainWinDock::mainWinDock()->setSideBarWidth( _sideBar->width() );
	return true;		// Event handled
    }

    return QWidget::eventFilter( obj, ev );
}


void YQWizard::setButtonLabel( YPushButton * button, const string & newLabel )
{
    button->setLabel( newLabel );
    YDialog::currentDialog()->checkShortcuts();

    YQWizardButton * wizardButton = dynamic_cast<YQWizardButton *> (button);

    if ( wizardButton )
    {
        // QWizardButton only implements hide and show, not setVisible
        if ( newLabel.empty() )
            wizardButton->hide();
        else
            wizardButton->show();
    }
}


void YQWizard::showReleaseNotesButton( const string & label, const string & id )
{
    if ( ! _releaseNotesButton )
    {
	yuiError() << "NULL Release Notes button" << endl;

	if ( ! _stepsPanel )
	    yuiError() << "This works only if there is a \"steps\" panel!" << endl;

	return;
    }

    // Qt handles duplicate shortcuts, it can be kept
    _releaseNotesButton->setLabel( fromUTF8( label ) );
    _releaseNotesButtonId = id;
    _releaseNotesButtonLabel = label;

    _releaseNotesButton->show();
}


void YQWizard::hideReleaseNotesButton()
{
    if ( _releaseNotesButton && !_releaseNotesButton->isHidden() )
    {
	_releaseNotesButton->hide();
	_releaseNotesButtonId = "";
	_releaseNotesButtonLabel = "";
    }
}


void YQWizard::retranslateInternalButtons()
{
    YQUI::setTextdomain( TEXTDOMAIN );

    if ( _helpButton )
	// "Help" button
        // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
	_helpButton->setLabel( _( "&Help" ) );

    if ( _stepsButton )
	// "Steps" button
        // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
	_stepsButton->setText( _( "&Steps" ) );

    if ( _treeButton )
	// "Tree" button
        // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
	_treeButton->setText( _( "&Tree" ) );

    if ( _releaseNotesButton )
	// "Release Notes" button
        // Qt handles duplicate shortcuts, it can be kept (bnc#880983)
	_releaseNotesButton->setLabel( _( "&Release Notes" ) );

    if ( _helpDialog )
	_helpDialog->retranslate();

    if ( _hotkeysDialog )
	_hotkeysDialog->retranslate();

    if ( _relNotesDialog )
	_relNotesDialog->retranslate();

}


void YQWizard::Step::deleteLabels()
{
    delete _statusLabel;
    _statusLabel = 0;
    delete _nameLabel;
    _nameLabel = 0;
}


YQWizard::Step::~Step()
{
    deleteLabels();
}


void YQWizard::Step::setStatus( Status s )
{
    if ( !_statusLabel || !_nameLabel || _status == s )
        return;

    _status = s;

    if ( s == Todo )
    {
        _statusLabel->setProperty( "class", "todo-step-status QLabel" );
        _nameLabel->setProperty  ( "class", "todo-step-name QLabel"   );
    }

    if ( s == Done )
    {
        _statusLabel->setProperty( "class", "done-step-status QLabel" );
        _nameLabel->setProperty  ( "class", "done-step-name QLabel"   );
    }

    if ( s == Current )
    {
        _statusLabel->setProperty( "class", "current-step-status QLabel" );
        _nameLabel->setProperty  ( "class", "current-step-name QLabel"   );
    }

    _statusLabel->style()->unpolish( _statusLabel );
    _statusLabel->style()->polish( _statusLabel );
    _nameLabel->style()->unpolish( _nameLabel );
    _nameLabel->style()->polish( _nameLabel );
}
