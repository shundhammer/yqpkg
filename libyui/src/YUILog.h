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

  File:		YUILog.h

  Author:     	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YUILog_h

#ifndef YUILogComponent
#error Missing #define YUILogComponent "myComponent" before #include "YUILog.h"
#endif

#include <iostream>
#include <string>

#include "ImplPtr.h"

using std::endl;


//
// UI Logging: Macros for Application use.
//
// They all return a std::ostream &  for use with operator<<().
// #define YUILogComponent before including this header file
// to identify what subsystem ("my-ui" etc.) this log line belongs to.
//
//    #define YUILogComponent "myComponent"
//    #include <YUILog.h>
//
//    ...
//    yuiDebug() << "Creating widget" << widget << endl;
//    yuiError() << "No widget with ID " << id << endl;
//
// Unless the underlying logger function handles this differently,
// Milestone, Warning and Error are always logged, Debug only when enabled.
//

#define yuiDebug()	YUILog::debug    ( YUILogComponent, __FILE__, __LINE__, __FUNCTION__ )
#define yuiMilestone()	YUILog::milestone( YUILogComponent, __FILE__, __LINE__, __FUNCTION__ )
#define yuiWarning()	YUILog::warning  ( YUILogComponent, __FILE__, __LINE__, __FUNCTION__ )
#define yuiError()	YUILog::error    ( YUILogComponent, __FILE__, __LINE__, __FUNCTION__ )


//
// ------ End of user relevant part ------
//



class YUILogPrivate;

enum YUILogLevel_t
{
    YUI_LOG_DEBUG     = 0,
    YUI_LOG_MILESTONE,
    YUI_LOG_WARNING,
    YUI_LOG_ERROR
};


/**
 * Logger function.
 *
 * All const char pointer parameters might be 0.
 **/
typedef void (*YUILoggerFunction)( YUILogLevel_t,	// logLevel
				   const char *,	// logComponent
				   const char *,	// sourceFileName
				   int, 	 	// sourceLineNo
				   const char *, 	// sourceFunctionName
				   const char * );	// message

typedef void (*YUIEnableDebugLoggingFunction)( bool );
typedef bool (*YUIDebugLoggingEnabledFunction)();


/**
 * UI logging.
 **/
class YUILog
{
public:

    /**
     * Logging functions for each log level. They all access the singleton object for this class.
     * This means that the first call to any of those functions will create the singleton YUILog object.
     **/
    static std::ostream & debug    ( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName );
    static std::ostream & milestone( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName );
    static std::ostream & warning  ( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName );
    static std::ostream & error    ( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName );

    /**
     * Generic log function. debug(), milestone() etc. ultimately all call this function.
     **/
    std::ostream & log( YUILogLevel_t	logLevel,
			const char *	logComponent,
			const char *	sourceFileName,
			int 		lineNo,
			const char * 	functionName );

    /**
     * Return the singleton object for this class.
     * This will create the singleton if it doesn't exist yet.
     **/
    static YUILog * instance();

    /**
     * Enable or disable debug logging.
     **/
    static void enableDebugLogging( bool debugLogging = true );

    /**
     * Return 'true' if debug logging is enabled, 'false' if not.
     **/
    static bool debugLoggingEnabled();

    /**
     * Set the log file name to be used with the standard logger function.
     * Output will be appended to this file.
     *
     * Until this file name is set, the standard logger function logs to stderr.
     * Set the log file name to an empty string to log to stderr again.
     *
     * This returns 'true' upon success (opening the file was successful),
     *'false' upon error.
     *
     *
     * Notice:
     *
     * (1) This file name is only relevant as long as the standard logger
     *     function is used. Custom logger functions may or may not use this
     *     file name.
     *
     * (2) No attempt is made to do anything fancy with the log file like log
     *     file rotation when a certain file size is reached. Applications that
     *     need this should use a custom logger function.
     *     See also setLoggerFunction().
     **/
    static bool setLogFileName( const std::string & logFileName );

    /**
     * Return the current log file name or an empty string if stderr is used.
     * Notice that this information is only relevant as long as the standard
     * logger function is used.
     **/
    static std::string logFileName();

    /**
     * Set the UI logger function. This is the function that will ultimately
     * receive all UI log output (except debug logging if debug logging is
     * disabled).
     *
     * By default, all logging is output to stderr. This behaviour can be
     * restored if 0 is passed as a function pointer here.
     **/
    static void setLoggerFunction( YUILoggerFunction loggerFunction );

    /**
     * Return the UI logger function.
     *
     * If stderr is used for logging (i.e. no logger function set), 0 is
     * returned (unless 'returnStdLogger' is 'true', in which case the
     * internally used stderr-logger is returned).
     **/
    static YUILoggerFunction loggerFunction( bool returnStdLogger = false );

    /**
     * Set the hook functions to enable/disable debug logging and to query if
     * debug logging is enabled:
     *
     *     void enableDebugLogging( bool enable );
     *     bool debugLoggingEnabled();
     *
     * If those functions are set, they will be used instead of the internal
     * "debugLogging" flag.
     **/
    static void setEnableDebugLoggingHooks( YUIEnableDebugLoggingFunction  enableFunction,
					    YUIDebugLoggingEnabledFunction isEnabledFunction );

    /**
     * Return the hook function that enables or disables debug logging
     * or 0 if no such hook function is set.
     **/
    static YUIEnableDebugLoggingFunction  enableDebugLoggingHook();

    /**
     * Return the hook function that checks if debug logging is enabled
     * or 0 if no such hook function is set.
     **/
    static YUIDebugLoggingEnabledFunction debugLoggingEnabledHook();

    /**
     * Return the base name without path from a file name with path.
     **/
    static std::string basename( const std::string & fileNameWithPath );


private:
    /**
     * Constructor.
     *
     * Not for application use - use one of the static functions above instead.
     * They all access the singleton object for this class.
     **/
    YUILog();

    /**
     * Destructor.
     **/
    ~YUILog();

    //
    // Data
    //

    ImplPtr<YUILogPrivate> priv;
};


#define YUILog_h

#endif // YUILog_h
