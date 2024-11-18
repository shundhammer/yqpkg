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

  File:		YUIException.h

		Stolen from zypp/libzypp/base/Exception.h

  Author:     	Michael Andres    <ma@suse.de>
  Maintainer:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YUIException_h
#define YUIException_h


#include <cerrno>
#include <iostream>
#include <stdexcept>

#include "YProperty.h"


class YWidget;


//
// Macros for application use
//

/**
 * Usage summary:
 *
 * Use YUI_THROW to throw exceptions.
 * Use YUI_CAUGHT If you caught an exceptions in order to handle it.
 * Use YUI_RETHROW to rethrow a caught exception.
 *
 * The use of these macros is not mandatory. but YUI_THROW and YUI_RETHROW will
 * adjust the code location information stored in the exception. All three
 * macros will drop a line in the log file.
 *
 *  43   try
 *  44   {
 *  45       try
 *  46       {
 *  47           YUI_THROW( YUIException("Something bad happened.") );
 *  48       }
 *  49       catch( YUIException & exception )
 *  50       {
 *  51           YUI_RETHROW( exception );
 *  52       }
 *  53   }
 *  54   catch( YUIException & exception )
 *  55   {
 *  56       YUI_CAUGHT( exception );
 *  57   }
 *
 * The above produces the following log lines:
 *
 *  Main.cc(main):47 THROW:    Main.cc(main):47: Something bad happened.
 *  Main.cc(main):51 RETHROW:  Main.cc(main):47: Something bad happened.
 *  Main.cc(main):56 CAUGHT:   Main.cc(main):51: Something bad happened.
 **/


/**
 * Create YCodeLocation object storing the current location.
 **/
#define YUI_EXCEPTION_CODE_LOCATION YCodeLocation(__FILE__,__FUNCTION__,__LINE__)


/**
 * Drops a log line and throws the YUIException.
 **/
#define YUI_THROW( EXCEPTION ) \
    _YUI_THROW( ( EXCEPTION ), YUI_EXCEPTION_CODE_LOCATION )

/**
 * Drops a log line telling the YUIException was caught and handled.
 **/
#define YUI_CAUGHT( EXCEPTION ) \
    _YUI_CAUGHT( ( EXCEPTION ), YUI_EXCEPTION_CODE_LOCATION )


/**
 * Drops a log line and rethrows, updating the YCodeLocation.
 **/
#define YUI_RETHROW( EXCEPTION ) \
    _YUI_RETHROW( ( EXCEPTION ), YUI_EXCEPTION_CODE_LOCATION )


/**
 * Throw YUIException built from a message string.
 **/
#define YUI_THROW_MSG( EXCEPTION_TYPE, MSG ) \
    YUI_THROW( EXCEPTION_TYPE( MSG ) )


/**
 * Throw YUIException built from errno.
 **/
#define YUI_THROW_ERRNO( EXCEPTION_TYPE ) \
    YUI_THROW( EXCEPTION_TYPE( YUIException::strErrno( errno ) ) )

/**
 * Throw YUIException built from errno provided as argument.
 **/
#define YUI_THROW_ERRNO1( EXCEPTION_TYPE, ERRNO ) \
    YUI_THROW( EXCEPTION_TYPE( YUIException::strErrno( ERRNO ) ) )

/**
 * Throw YUIException built from errno and a message string.
 **/
#define YUI_THROW_ERRNO_MSG( EXCEPTION_TYPE, MSG) \
    YUI_THROW( EXCEPTION_TYPE( YUIException::strErrno( errno, MSG ) ) )

/**
 * Throw YUIException built from errno provided as argument and a message string.
 **/
#define YUI_THROW_ERRNO_MSG1( EXCEPTION_TYPE, ERRNO,MSG ) \
    YUI_THROW( EXCEPTION_TYPE( YUIException::strErrno( ERRNO, MSG ) ) )


//
// Higher-level (UI specific) exception macros
//

/**
 * Check if an instance returned by operator new is valid (nonzero).
 * Throws YUIOutOfMemoryException if it is 0.
 **/
#define YUI_CHECK_NEW( PTR )				\
    do							\
    {							\
	if ( ! (PTR) )					\
	{						\
	    YUI_THROW( YUIOutOfMemoryException() );	\
	}						\
    } while( 0 )



/**
 * Check for null pointer.
 * Throws YUINullPointerException if the pointer is 0.
 **/
#define YUI_CHECK_PTR( PTR )				\
    do							\
    {							\
	if ( ! (PTR) )					\
	{						\
	    YUI_THROW( YUINullPointerException() );	\
	}						\
    } while( 0 )


/**
 * Check if a widget pointer is valid.
 * Throws YUIInvalidWidgetException if it is 0 or invalid (already deleted).
 *
 * Explicitly casting the memory-address stored in the given pointer to
 * a boolean-type for null-pointer-checks is needed for GCC >= 6, because
 * it introduces new optimizations to remove null-pointer-checks for 'this'.
 *
 * Not explicitly casting the pointer's memory-address, will cause the
 * compilation to fail with an error, when using this macro in YDialog:
 *
 *     …/src/YDialog.cc: In member function 'bool YDialog::destroy(bool)':
 *     …/src/YDialog.cc:254:24: error:
 *         nonnull argument 'this' compared to NULL [-Werror=nonnull-compare]
 *     YUI_CHECK_WIDGET( this );
 *               ~~~~~~~~~^~~~~~
 *
 * See: https://gcc.gnu.org/gcc-6/porting_to.html
 **/
#define YUI_CHECK_WIDGET( WIDGET )			\
    do							\
    {							\
	if ( ! ( static_cast<bool> (WIDGET) ) ||	\
	     ! (WIDGET)->isValid() )			\
	{						\
	    YUI_THROW( YUIInvalidWidgetException() );	\
	}						\
    } while( 0 )


/**
 * Check if an index is in range:
 * VALID_MIN <= INDEX <= VALID_MAX
 *
 * Throws YUIInvalidWidgetException if out of range.
 **/
#define YUI_CHECK_INDEX_MSG( INDEX, VALID_MIN, VALID_MAX, MSG )	\
    do								\
    {								\
	if ( (INDEX) < (VALID_MIN) ||				\
	     (INDEX) > (VALID_MAX) )				\
	{							\
	    YUI_THROW( YUIIndexOutOfRangeException( (INDEX), (VALID_MIN), (VALID_MAX), (MSG) ) ); \
	}							\
    } while( 0 )


#define YUI_CHECK_INDEX( INDEX, VALID_MIN, VALID_MAX ) \
    YUI_CHECK_INDEX_MSG( (INDEX), (VALID_MIN), (VALID_MAX), "")




/**
 * Helper class for UI exceptions: Store _FILE_, _FUNCTION_ and _LINE_.
 * Construct this using the YUI_EXCEPTION_CODE_LOCATION macro.
 **/
class YCodeLocation
{
public:
    /**
     * Constructor.
     * Commonly called using the YUI_EXCEPTION_CODE_LOCATION macro.
     **/
    YCodeLocation( const std::string &	file_r,
		   const std::string &	func_r,
		   int			line_r )
	: _file( file_r )
	, _func( func_r )
	, _line( line_r )
	{}

    /**
     * Default constructor.
     ***/
    YCodeLocation()
	: _line( 0 )
	{}

    /**
     * Returns the source file name where the exception occured.
     **/
    std::string file() const { return _file; }

    /**
     * Returns the name of the function where the exception occured.
     **/
    std::string func() const { return _func; }

    /**
     * Returns the source line number where the exception occured.
     **/
    int line() const { return _line; }

    /**
     * Returns the location in normalized string format.
     **/
    std::string asString() const;

    /**
     * Stream output
     **/
    friend std::ostream & operator<<( std::ostream & str, const YCodeLocation & obj );

private:
    std::string	_file;
    std::string	_func;
    int		_line;

}; // YCodeLocation


/**
 * YCodeLocation stream output
 **/
std::ostream & operator<<( std::ostream & str, const YCodeLocation & obj );


/**
 * Base class for UI Exceptions.
 *
 * Exception offers to store a message string passed to the constructor.
 * Derived classes may provide additional information.
 * Overload dumpOn to provide a proper error text.
 **/
class YUIException : public std::exception
{
public:
    /**
     * Default constructor.
     * Use YUI_THROW to throw exceptions.
     **/
    YUIException();

    /**
     * Constructor taking a message.
     * Use YUI_THROW to throw exceptions.
     **/
    YUIException( const std::string & msg_r );

    /**
     * Destructor.
     **/
    virtual ~YUIException() throw();

    /**
     * Return YCodeLocation.
     **/
    const YCodeLocation & where() const
	{ return _where; }

    /**
     * Exchange location on rethrow.
     **/
    void relocate( const YCodeLocation & newLocation ) const
	{ _where = newLocation; }

    /**
     * Return the message string provided to the constructor.
     * Note: This is not neccessarily the complete error message.
     * The whole error message is provided by asString or dumpOn.
     **/
    const std::string & msg() const
	{ return _msg; }

    /**
     * Set a new message string.
     **/
    void setMsg( const std::string & msg )
	{ _msg = msg; }

    /**
     * Error message provided by dumpOn as string.
     **/
    std::string asString() const;

    /**
     * Make a string from errno_r.
     **/
    static std::string strErrno( int errno_r );

    /**
     * Make a string from errno_r and msg_r.
     **/
    static std::string strErrno( int errno_r, const std::string & msg );

    /**
     * Drop a log line on throw, catch or rethrow.
     * Used by YUI_THROW macros.
     **/
    static void log( const YUIException  & 	exception,
		     const YCodeLocation & 	location,
                     const char * const		prefix );
    /**
     * Return message string.
     *
     * Reimplemented from std::exception.
     **/
    virtual const char * what() const throw()
	{ return _msg.c_str(); }

protected:

    /**
     * Overload this to print a proper error message.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;


private:
    friend std::ostream & operator<<( std::ostream & str, const YUIException & obj );


    mutable YCodeLocation	_where;
    std::string			_msg;

    /**
     * Called by std::ostream & operator<<() .
     * Prints YCodeLocation and the error message provided by dumpOn.
     **/
    std::ostream & dumpError( std::ostream & str ) const;

}; // class YUIException


/**
 * YUIException stream output
 **/
std::ostream & operator<<( std::ostream & str, const YUIException & obj );


/**
 * Exception class for generic null pointer exceptions.
 * When available, a more specialized exception class should be used.
 **/
class YUINullPointerException: public YUIException
{
public:
    YUINullPointerException()
	: YUIException( "Null pointer" )
	{}

    virtual ~YUINullPointerException() throw()
	{}
};


/**
 * Exception class for "out of memory".
 * Typically used if operator new returned 0.
 **/
class YUIOutOfMemoryException: public YUIException
{
public:
    YUIOutOfMemoryException()
	: YUIException( "Out of memory" )
	{}

    virtual ~YUIOutOfMemoryException() throw()
	{}

};

/**
 * Exception class for invalid widgets.
 * This is typically caused by widget pointers that continue living after the
 * corresponding widget has already been deleted.
 **/
class YUIInvalidWidgetException: public YUIException
{
public:
    YUIInvalidWidgetException()
	: YUIException( "Invalid widget" )
	{}

    virtual ~YUIInvalidWidgetException() throw()
	{}
};


/**
 * Exception class for "No widget found with that ID".
 **/
class YUIWidgetNotFoundException: public YUIException
{
public:
    YUIWidgetNotFoundException( const std::string & idString )
	: YUIException( std::string( "No widget with ID " ) + idString )
	{}

    virtual ~YUIWidgetNotFoundException() throw()
	{}
};


class YUINoDialogException: public YUIException
{
public:
    YUINoDialogException()
	: YUIException( "No dialog existing" )
	{}

    virtual ~YUINoDialogException() throw()
	{}
};


class YUIDialogStackingOrderException: public YUIException
{
public:
    YUIDialogStackingOrderException()
	: YUIException( "Dialog stacking order violated" )
	{}

    virtual ~YUIDialogStackingOrderException() throw()
	{}
};


class YUISyntaxErrorException: public YUIException
{
public:
    YUISyntaxErrorException( const std::string & msg )
	: YUIException( msg )
	{}

    virtual ~YUISyntaxErrorException() throw()
	{}
};


/**
 * Abstract base class for widget property exceptions.
 **/
class YUIPropertyException: public YUIException
{
protected:
    YUIPropertyException( const YProperty & 	prop,
			  YWidget * 		widget = 0 )
	: YUIException()
	, _property( prop )
	, _widget( widget )
	{}

    virtual ~YUIPropertyException() throw()
	{}

public:
    /**
     * Returns the property that caused this exception.
     **/
    YProperty property() const { return _property; }

    /**
     * Returns the corresponding widget or 0 if there was none.
     **/
    YWidget * widget() const { return _widget; }

    /**
     * Set the corresponding widget.
     **/
    void setWidget( YWidget * w ) { _widget = w; }

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const = 0;

private:
    YProperty	_property;
    YWidget *	_widget;
};


/**
 * Exception class for "unknown property name":
 * The application tried to set (or query) a property that doesn't exist.
 **/
class YUIUnknownPropertyException: public YUIPropertyException
{
public:
    YUIUnknownPropertyException( const std::string &	propertyName,
				 YWidget *		widget = 0 )
	: YUIPropertyException( YProperty( propertyName, YUnknownPropertyType ), widget )
	{}

    virtual ~YUIUnknownPropertyException() throw()
	{}

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};


/**
 * Exception class for "property type mismatch":
 * The application tried to set a property with a wrong type.
 **/
class YUIPropertyTypeMismatchException: public YUIPropertyException
{
public:

    YUIPropertyTypeMismatchException( const YProperty & property,
				      YPropertyType	type,
				      YWidget *		widget = 0 )
	: YUIPropertyException( property, widget )
	, _type( type )
	{}

    virtual ~YUIPropertyTypeMismatchException() throw()
	{}

    /**
     * Return the property type the application tried to set.
     **/
    YPropertyType type() const { return _type; }

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;

private:
    YPropertyType _type;
};


/**
 * Exception class for attempt to set a read-only property.
 **/
class YUISetReadOnlyPropertyException: public YUIPropertyException
{
public:

    YUISetReadOnlyPropertyException( const YProperty &	property,
				     YWidget *		widget = 0 )
	: YUIPropertyException( property, widget )
	{}

    virtual ~YUISetReadOnlyPropertyException() throw()
	{}

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};


class YUIBadPropertyArgException: public YUIPropertyException
{
public:

    YUIBadPropertyArgException( const YProperty & 	property,
				YWidget *		widget,
				const std::string &	message = "" )
	: YUIPropertyException( property, widget )
	{ setMsg( message ); }

    virtual ~YUIBadPropertyArgException() throw()
	{}

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};


/**
 * Exception class for "too many children":
 * Attempt to add a child to a widget class that can't handle children
 * (YPushButton etc.) or just one child (YFrame, YDialog).
 **/
template<class YWidget> class YUITooManyChildrenException: public YUIException
{
public:

    YUITooManyChildrenException( YWidget * container )
	: YUIException( "Too many children" )
	, _container( container )
	{}

    virtual ~YUITooManyChildrenException() throw()
	{}

    /**
     * Returns the container widget that can't handle that many children.
     **/
    YWidget * container() const { return _container; }

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const
    {
	std::string widgetClass =
	    container() ? container()->widgetClass() :
	    "widget";

	return str << "Too many children for "
		   << widgetClass
		   << std::endl;
    }

private:

    YWidget * _container;
};


/**
 * Exception class for "invalid child". One of:
 *
 * - Attempt to remove a child from a children manager that is not in that
 *   manager's children list.
 *
 * - Child widget of wrong type added to a container widget, e.g., anything
 *   other than a YPushButton added to a YButtonBox.
 **/
template<class YWidget> class YUIInvalidChildException: public YUIException
{
public:

    YUIInvalidChildException( YWidget * container,
			      YWidget * child = 0 )
	: YUIException( "Invalid child" )
	, _container( container )
	, _child( child )
	{}

    virtual ~YUIInvalidChildException() throw()
	{}

    /**
     * Returns the container widget whose child should be removed etc.
     **/
    YWidget * container() const { return _container; }

    /**
     * Returns the child widget.
     **/
    YWidget * child() const { return _child; }

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const
    {
	std::string containerWidgetClass =
	    container() ? container()->widgetClass() :
	    "widget";

	std::string childWidgetClass =
	    child() ? child()->widgetClass() :
	    "<Null>";

	return str << childWidgetClass
		   << " is not a child of "
		   << containerWidgetClass
		   << std::endl;
    }

private:

    YWidget * _container;
    YWidget * _child;
};



/**
 * Exception class for "optional widget not supported".
 *
 * Note that applications are supposed to check with
 * YUI::optionalWidgetFactory()->hasXYWidget() before trying to create such a
 * widget. This exception is thrown if that check wasn't done, the application
 * tried to create that kind of widget anyway, but the UI doesn't support that
 * widget.
 **/
class YUIUnsupportedWidgetException: public YUIException
{
public:

    YUIUnsupportedWidgetException( const std::string & widgetType )
	: YUIException( std::string( "Unsupported optional widget type: " ) + widgetType )
	{}

    virtual ~YUIUnsupportedWidgetException() throw()
	{}
};


/**
 * Exception class for "value other than YD_HORIZ or YD_VERT used for
 * dimension".
 **/
class YUIInvalidDimensionException: public YUIException
{
public:
    YUIInvalidDimensionException()
	: YUIException( "Invalid dimension (neither YD_HORIZ nor YD_VERT)" )
	{}

    virtual ~YUIInvalidDimensionException() throw()
	{}
};


/**
 * Exception class for "index out of range"
 **/
class YUIIndexOutOfRangeException: public YUIException
{
public:
    /**
     * Constructor.
     *
     * 'invalidIndex' is the offending index value. It should be between
     *'validMin' and 'validMax':
     *
     *     validMin <= index <= validMax
     **/
    YUIIndexOutOfRangeException( int			invalidIndex,
				 int			validMin,
				 int			validMax,
				 const std::string &	msg = "" )
	: YUIException( msg )
	, _invalidIndex( invalidIndex )
	, _validMin( validMin )
	, _validMax( validMax )
	{}

    virtual ~YUIIndexOutOfRangeException() throw()
	{}

    /**
     * Return the offending index value.
     **/
    int invalidIndex() const	{ return _invalidIndex; }

    /**
     * Return the valid minimum index.
     **/
    int validMin() const	{ return _validMin; }

    /**
     * Return the valid maximum index.
     **/
    int validMax() const	{ return _validMax; }

protected:

    /**
     * Write proper error message with all relevant data.
     * Reimplemented from YUIException.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const
    {
	std::string prefix = msg();

	if ( prefix.empty() )
	    prefix = "Index out of range";

	return str << prefix << ": " << _invalidIndex
		   << " valid: " << _validMin << " .. " << _validMax
		   << std::endl;
    }

private:

    int _invalidIndex;
    int _validMin;
    int _validMax;
};


/**
 * Exception class for plugin load failure
 **/
class YUIPluginException: public YUIException
{
public:
    YUIPluginException( const std::string & pluginName )
	: YUIException( std::string( "Couldn't load plug-in " ) + pluginName )
	{}

    virtual ~YUIPluginException() throw()
	{}
};


/**
 * Exception class for UI plugin load failure
 **/
class YUICantLoadAnyUIException: public YUIException
{
public:
    YUICantLoadAnyUIException()
	: YUIException( "No $DISPLAY and stdout is not a tty" )
	{}

    virtual ~YUICantLoadAnyUIException() throw()
	{}
};


/**
 * Exception class for "wrong button roles in YButtonBox"
 **/
class YUIButtonRoleMismatchException: public YUIException
{
public:

    YUIButtonRoleMismatchException( const std::string & msg )
	: YUIException( msg )
	{}

    virtual ~YUIButtonRoleMismatchException() throw()
	{}
};


//
// Helper templates
//


/**
 * Helper for YUI_THROW()
 **/
template<class _Exception>
void _YUI_THROW( const _Exception & exception_r, const YCodeLocation & where_r )
{
    exception_r.relocate( where_r );
    YUIException::log( exception_r, where_r, "THROW:   " );
    throw( exception_r );
}


/**
 * Helper for YUI_CAUGHT()
 **/
template<class _Exception>
void _YUI_CAUGHT( const _Exception & exception_r, const YCodeLocation & where_r )
{
    YUIException::log( exception_r, where_r, "CAUGHT:  " );
}


/**
 * Helper for YUI_RETHROW()
 **/
template<class _Exception>
void _YUI_RETHROW( const _Exception & exception_r, const YCodeLocation & where_r )
{
    YUIException::log( exception_r, where_r, "RETHROW: " );
    exception_r.relocate( where_r );
    throw;
}


#endif // YUIException_h
