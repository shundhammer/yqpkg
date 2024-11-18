/*
  Copyright (c) 2000-2012 Novell, Inc
  Copyright (c) 2022 SUSE LLC
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

  File:       QY2RelNotesDialog.h

  Author:     Jiri Srain <jsrain@suse.de>

  Textdomain    "qt"

/-*/

#ifndef QY2RelNotesDialog_h
#define QY2RelNotesDialog_h

#include <string>
#include <map>
#include <vector>

#include <QDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QTabBar>
#include <QUrl>

class QY2TextBrowser;

/**
 * Release notes dialog for the YQWizard widget.
 *
 * This one is used only in the Qt UI inside the YQWizard after
 * UI.WizardCommand(ShowReleaseNotesButton, ...).
 *
 * Outside of the YQWizard or in the NCurses UI, it uses
 * YDialog::showRelNotesText() (see ../../libyui/YDialogHelpers.cc) after being
 * triggered from the YRelNotesButtonHandler event filter on a very low level
 * when a button has the "release notes" button role
 * (YPushButton::isRelNotesButton()).
 **/
class QY2RelNotesDialog : public QDialog
{
    Q_OBJECT

public:
    QY2RelNotesDialog( QWidget *parent );
    ~QY2RelNotesDialog();
    void setRelNotes( const std::map<std::string,std::string>& relNotes );
    void retranslate();

public slots:
    void tabChanged( int index );

protected:

    QTabBar        * _tabBar;
    QY2TextBrowser * _textBrowser;
    QPushButton    * _closeButton;

    std::map<std::string,std::string> _relNotes;
    std::vector<std::string>          _tabKeys;
};


class QY2TextBrowser: public QTextBrowser
{
    Q_OBJECT

public:
    QY2TextBrowser( QWidget * parent ):
        QTextBrowser( parent )
        {}

public slots:
    /**
     * Overriding setSource to prevent clearing the content when the user
     * clicks on (external) hyperlinks.
     **/
    virtual void setSource( const QUrl & name );
};


#endif  // QY2RelNotesDialog_h
