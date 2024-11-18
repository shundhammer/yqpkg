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

  File:	      YQDownloadProgress.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQDownloadProgress_h
#define YQDownloadProgress_h

#include <QFrame>
#include <yui/YDownloadProgress.h>

class YQWidgetCaption;
class QProgressBar;


class YQDownloadProgress : public QFrame, public YDownloadProgress
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQDownloadProgress( YWidget *	        parent,
			const std::string & 	label,
			const std::string &	filename,
			YFileSize_t	        expectedSize );

    /**
     * Destructor.
     **/
    virtual ~YQDownloadProgress();

    /**
     * Set the label (the text above the progress bar).
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Set the name of a new file to monitor.
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setFilename( const std::string & filename );

    /**
     * Change the expected file size.
     *
     * Reimplemented from YDownloadProgress.
     **/
    virtual void setExpectedSize( YFileSize_t expectedSize );

    /**
     * Set enabled/disabled state.
     *
     * Reimplemented from YWidget.
     **/
    virtual void setEnabled( bool enabled );

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


public slots:
    /**
     * Slot for polling and displaying the file size.
     **/
    void pollFileSize( void );


protected:

    YQWidgetCaption *	_caption;
    QProgressBar *	_qt_progressBar;
    QTimer *		_timer;
};


#endif // YQDownloadProgress_h
