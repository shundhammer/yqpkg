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
  File:	      YQPkgDescriptionView.h
  Author:     Stefan Hundhammer <shundhammer@suse.com>
*/


#ifndef YQPkgDescriptionView_h
#define YQPkgDescriptionView_h

#include <QUrl>
#include "YQPkgGenericDetailsView.h"

using std::list;
using std::string;


/**
 * @short Display the description of a ZyppObj derived object along with its
 * name and summary.
 **/
class YQPkgDescriptionView : public YQPkgGenericDetailsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     **/
    YQPkgDescriptionView( QWidget * parent, bool showSupportability = true );

    /**
     * Destructor
     **/
    virtual ~YQPkgDescriptionView();

    /**
     * Show details for the specified package:
     * In this case the package description.
     * Overwritten from YQPkgGenericDetailsView.
     **/
    virtual void showDetails( ZyppSel selectable );

    /**
     * Get the document pointed to by a hyperlink.
     *
     * Reimplemented from QTextBrowser to avoid having an empty text each time
     * the user clicks on a hyperlink.
     **/
    virtual void setSource( const QUrl & name );

protected:

    /**
     * Format a multi-line text into paragraphs
     **/
    QString simpleHtmlParagraphs( QString text );
    
    /**
     * Show information for a hyperlinked object,
     * e.g., a "pkg:somepkg" link to another package.
     **/
    void showLink( const QUrl & url );

    /**
     * Return html text that contains a list of application icons.
     **/
    QString applicationIconList( const list<string> & fileList ) const;

    /**
     * Find absolute file name (incl. path) for a icon.
     **/
    QString findDesktopIcon ( const QString& iconName ) const;

    /**
     * Extract name, icon and exec attributes from a desktop file.
     **/
    QMap<QString, QString> readDesktopFile( const QString & fileName ) const;

    /**
     * Search for all desktop files in a file list.
     **/
    QStringList findDesktopFiles( const list<string> & fileList ) const;

    /**
     * Initialize the language code (lang).
     **/
    void initLang();

private:
    QString langWithCountry;
    QString lang;
    bool _showSupportability;

};


#endif // ifndef YQPkgDescriptionView_h
