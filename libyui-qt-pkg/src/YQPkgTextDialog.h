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
  File:	      YQPkgTextDialog.h
  Author:     Stefan Hundhammer <shundhammer.de>
*/


#ifndef YQPkgTextDialog_h
#define YQPkgTextDialog_h

#include "YQZypp.h"
#include <QDialog>
#include <QEvent>
#include <zypp/ResObject.h>

class QPushButton;
class QTextBrowser;

using std::string;


/**
 * @short Dialog that shows a scrolled (HTML) text.
 **/
class YQPkgTextDialog: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Creates a dialog with a text browser and two buttons.
     * This does not pop up a dialog yet.
     * Use 'exec()' (inherited from QDialog).
     * Or, better yet, use the static confirmText() method.
     *
     **/
    YQPkgTextDialog( const QString & 	text,
		     QWidget * 		parent,
		     const QString & 	acceptButtonLabel,
		     const QString & 	rejectButtonLabel );
    /**
     * Constructor.
     *
     * Creates a dialog with a text browser and an "OK" button.
     * This does not pop up a dialog yet.
     * Use 'exec()' (inherited from QDialog).
     * Or, better yet, use the static showText() method.
     **/
    YQPkgTextDialog( const QString & text, QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgTextDialog();

    /**
     * Reimplemented from QWidget:
     * Reserve a reasonable amount of space.
     **/
    virtual QSize sizeHint() const;

    /**
     * Show a text and wait until the user confirmed with 'OK'.
     **/
    static void showText( QWidget * parent, const QString & text );

    /**
     * Show a text with a headline identifying a selectable (name+summary).
     **/
    static void showText( QWidget * 		parent,
			  ZyppSel 		selectable,
			  const string &	text );

    /**
     * Let the user confirm a text. Returns "true" if the user clicked the
     * accept button, false if he clicked the reject button.
     **/
    static bool confirmText( QWidget * 	parent,
			     const QString & 	text,
			     const QString & 	acceptButtonLabel,
			     const QString & 	rejectButtonLabel );

    /**
     * Let the use confirm a text with buttons "Accept" and "Cancel".
     * Returns "true" if the user clicked "Accept", "false" on "Cancel".
     **/
    static bool confirmText( QWidget * parent, const QString & text );
    static bool confirmText( QWidget * parent, const char * text );

    /**
     * Let the use confirm a text with a headline identifying a selectable
     * (name+summary) with buttons "Accept" and "Cancel".
     * Returns "true" if the user clicked "Accept", "false" on "Cancel".
     **/
    static bool confirmText( QWidget * 			parent,
			     ZyppSel 	selectable,
			     const string & 		text );

    /**
     * Simple HTML formatting: Wrap paragraphs in <p>...</p>
     * Paragraphs are delimited by empty lines.
     * Return unformatted text if it contains "<!-- DT:Rich -->".
     **/
    static QString htmlParagraphs( const string & rawText );

    /**
     * Returns a uniform heading in HTML format.
     **/
    static QString htmlHeading( const QString & text );

    /**
     * Returns a uniform heading in HTML format for the specified selectable:
     * name and summary
     **/
    static QString htmlHeading( ZyppSel selectable );

    /**
     * Escapes characters special to HTML in a ( plain text ) string, such as:
     * '<'   ->   '&lt;'
     * '>'   ->   '&gt;'
     * '&'   ->   '&amp;'
     *
     * Returns the escaped string.
     **/
    static QString htmlEscape( const QString & plainText );


public slots:

    /**
     * Set the text contents.
     **/
    void setText( const QString & text );
    void setText( const string & text );

    /**
     * Show a text with a headline identifying a zypp::ResObject ( name+summary ).
     **/
    void setText( ZyppSel selectable,
		  const string & text );

protected:

    /**
     * Create the dialog. Called from all constructors.
     **/
    void buildDialog( const QString & 	text,
		      QWidget * 	parent,
		      const QString & 	acceptButtonLabel,
		      const QString & 	rejectButtonLabel = "" );
    /**
     * Grab [Return] press events and close dialog.
     * Inherited from QObject.
     **/
    bool eventFilter( QObject * obj, QEvent * ev );


    // Data members

    QPushButton *	_acceptButton;
    QPushButton * 	_rejectButton;
    QTextBrowser *	_textBrowser;
};



#endif // ifndef YQPkgTextDialog_h
