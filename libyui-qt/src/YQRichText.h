/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2019 SUSE LLC
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

  File:	      YQRichText.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQRichText_h
#define YQRichText_h

#include <QFrame>
#include <QTextBrowser>
#include <yui/YRichText.h>

using std::string;


class YQTextBrowser;

class YQRichText : public QFrame, public YRichText
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQRichText( YWidget * 	        parent,
		const std::string &	text,
		bool		        plainTextMode = false );

    /**
     * Destructor.
     **/
    virtual ~YQRichText();

    /**
     * Change the text content of the RichText widget.
     *
     * Reimplemented from YRichText.
     **/
    virtual void setValue( const std::string & newValue ) override;

    /**
     * Set this RichText widget's "plain text" mode on or off.
     *
     * Reimplemented from YRichText.
     **/
    virtual void setPlainTextMode( bool on = true );

    /**
     * Set this RichText widget's "auto scroll down" mode on or off.
     *
     * Reimplemented from YRichText.
     **/
    virtual void setAutoScrollDown( bool on = true );

    /**
     * Preferred width of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredWidth();

    /**
     * Preferred height of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual int preferredHeight();

    /**
     * Set the new size of the widget.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setSize( int newWidth, int newHeight );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

    /**
     * Accept the keyboard focus.
     *
     * Reimplemented from YWidget.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Event filter.
     *
     * Reimplemented from QWidget.
     **/
    virtual bool eventFilter( QObject * obj, QEvent * ev );

    /**
     * Returns 'true' if the current text of this RichText widget contains
     * hyperlinks.
     **/
    bool haveHyperLinks();

    /**
     * Get the vertical scrollbar position.
     *
     * Reimplemented from YRichText.
     **/
    virtual std::string vScrollValue() const override;

    /**
     * Set the vertical scrollbar position.
     *
     * Reimplemented from YRichText.
     **/
    virtual void setVScrollValue( const std::string & newValue ) override;

    /**
     * Get the horizontal scrollbar position.
     *
     * Reimplemented from YRichText.
     **/
    virtual std::string hScrollValue() const override;

    /**
     * Set the horizontal scrollbar position.
     *
     * Reimplemented from YRichText.
     **/
    virtual void setHScrollValue( const std::string & newValue ) override;

    /**
     * Derived classes should implement this, method is used to trigger event
     * like user has clicked link in the RichText
     **/
    virtual void activateLink( const std::string & url );

    /**
     * Set the stylesheet for the RichText widget *content*.
     **/
    virtual void setRichTextStyleSheet( const QString &style );

protected slots:

    /**
     * Notification that a hyperlink is clicked.
     **/
    void linkClicked( const QUrl & url );

protected:

    YQTextBrowser *	_textBrowser;

private:

    /**
     * to avoid big suprises, we remember if the stylesheet specified a
     * color before we replace color= with class=
     **/
    bool *_colors_specified;

    /**
     * Helper function to get (vertical or horizontal) scrollbar position.
     */
    std::string scrollValue( QScrollBar* scrollBar ) const;

    /**
     * Helper function to set (vertical or horizontal) scrollbar position.
     */
    void setScrollValue( QScrollBar* scrollBar, const std::string & newValue );

};


/**
 * Helper class - needed to have the benefits of both QVBox as the base class
 * for YQRichText so uniform margins can be implemented (outside, not inside
 * the scroll window as QTextBrowser normally does it) and QTextBrowser's
 * feature (enable hyperlinks) without removing the text each time the user
 * clicks.
 *
 * This class is required only to overwrite setSource().
 **/
class YQTextBrowser: public QTextBrowser
{
    Q_OBJECT


public:

    /**
     * Constructor
     **/
    YQTextBrowser( QWidget * parent = 0 )
	: QTextBrowser( parent )
        {}

public slots:

    /**
     * Get the document pointed to by a hyperlink.
     *
     * Reimplemented from QTextBrowser to avoid having an empty text each time
     * the user clicks on a hyperlink.
     **/
    virtual void setSource( const QUrl & name );
};


#endif // YQRichText_h
