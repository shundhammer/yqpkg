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

  File:		YDownloadProgress.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YDownloadProgress_h
#define YDownloadProgress_h

#include "YWidget.h"


class YDownloadProgressPrivate;

/**
 * DownloadProgress: A progress bar that monitors downloading a file by
 * repeatedly polling its size up to its expected size.
 **/
class YDownloadProgress : public YWidget
{
protected:
    /**
     * Constructor.
     *
     * 'label' is the label above the progress bar.
     *
     * 'filename' is the name (with path) of the file being monitored.
     *
     * 'expectedSize' is the expected size of the file in bytes.
     **/
    YDownloadProgress( YWidget *		parent,
		       const std::string &	label,
		       const std::string &	filename,
		       YFileSize_t		expectedSize );
public:
    /**
     * Destructor.
     **/
    virtual ~YDownloadProgress();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YDownloadProgress"; }

    /**
     * Get the label (the text above the progress bar).
     **/
    std::string label() const;

    /**
     * Set the label (the text above the progress bar).
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setLabel( const std::string & label );

    /**
     * Return the name of the file that is being monitored.
     **/
    std::string filename() const;

    /**
     * Set the name of a new file to monitor.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setFilename( const std::string & filename );

    /**
     * Return the expected file size.
     **/
    YFileSize_t expectedSize() const;

    /**
     * Set the expected file size.
     *
     * Derived classes are free to reimplement this, but they should call this
     * base class method at the end of the overloaded function.
     **/
    virtual void setExpectedSize( YFileSize_t newSize );

    /**
     * Return the current size of the file that is being downloaded
     * or 0 if this file doesn't exist (yet).
     *
     * This default implementation returns the 'st_size' field of a stat()
     * system call on the file. This should be useful for most implementations.
     **/
    virtual YFileSize_t currentFileSize() const;

    /**
     * Return the percentage (0..100) of the file being downloaded so far.
     **/
    int currentPercent() const;

    /**
     * Alias for currentPercent().
     **/
    int value() const { return currentPercent(); }

    /**
     * Set a property.
     * Reimplemented from YWidget.
     *
     * This function may throw YUIPropertyExceptions.
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string    & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property.
     * Reimplemented from YWidget.
     *
     * This method may throw YUIPropertyExceptions.
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Reimplemented from YWidget.
     **/
    virtual const YPropertySet & propertySet();


private:

    ImplPtr<YDownloadProgressPrivate> priv;
};


#endif // YDownloadProgress_h
