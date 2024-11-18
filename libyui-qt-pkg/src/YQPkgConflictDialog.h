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
  File:	      YQPkgConflictDialog.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgConflictDialog_h
#define YQPkgConflictDialog_h

#include <QDialog>
#include <QMenu>
#include <QLabel>

class YQPkgConflictList;
class PMManager;
class QPushButton;
class QMenu;
class QLabel;


/**
 * @short Dialog that takes care of dependency checking and resolving
 * conflicts.
 **/
class YQPkgConflictDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor. Does not pop up a dialog yet.
     * Use solveAndShowConflicts().
     **/
    YQPkgConflictDialog( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflictDialog();

    /**
     * Reimplemented from QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    /**
     * Returns the average time in seconds used for solving or 0 if solving
     * hasn't taken place yet.
     **/
    double averageSolveTime() const;

    /**
     * Returns the total amount of time in seconds used for solving.
     **/
    double totalSolveTime() const { return _totalSolveTime; }

    /**
     * Returns the times solving has taken place (with this dialog).
     **/
    int solveCount() const { return _solveCount; }

    /**
     * Reset all previously ignored dependency problems.
     **/
    static void resetIgnoredDependencyProblems();


public slots:

    /**
     * Run the package dependency solver for the current package set and open
     * the conflict dialog if there are any conflicts.
     * Returns only when the conflict dialog is closed.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int solveAndShowConflicts();

    /**
     * Run the package dependency solver for the currently installed system
     * plus the packages that are marked for installation (or update or...) and
     * open the conflict dialog if there are any conflicts.
     * Returns only when the conflict dialog is closed.
     *
     * Returns QDialog::Accepted or QDialog::Rejected.
     **/
    int verifySystem();

    /**
     * Mini-wizard to generate solver test case:
     *
     *   - Inform user what this is all about
     *   - Tell user where the files will go
     *   - Generate test case
     *   - Ask user if he would like to pack it all into y2logs.tgz
     *   - Prompt user for path for y2logs.tgz
     *   - Save y2logs.tgz
     **/
    void askCreateSolverTestCase();

    
signals:

    /**
     * Update package states - they may have changed.
     **/
    void updatePackages();


protected:

    /**
     * Initialize solving: Post "busy" popup etc.
     **/ 
    void prepareSolving();

    /**
     * Process the result of solving: Post conflict dialog, if neccessary.
     * 'success' is the return value of the preceding solver call.
     * Returns either QDialog::Accepted or QDialog::Rejected.
     **/
    int  processSolverResult( bool success );
    
    /**
     * Event handler for keyboard input.
     * Only very special keys are processed here.
     *
     * Inherited from QWidget.
     **/
    void keyPressEvent( QKeyEvent * e );



    //
    // Data members
    //

    YQPkgConflictList *	_conflictList;
    QMenu *	_expertMenu;
    QLabel *		_busyPopup;

    double		_totalSolveTime;
    int			_solveCount;
};



#endif // ifndef YQPkgConflictDialog_h
