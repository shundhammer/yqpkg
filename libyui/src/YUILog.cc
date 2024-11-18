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

  File:		YUILog.cc

  Author:     	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#include <string.h>

#include <ostream>
#include <fstream>
#include <vector>
#include <pthread.h>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUIException.h"

using std::string;
using std::ostream;
using std::cerr;


static void stdLogger( YUILogLevel_t	logLevel,
		       const char *	logComponent,
		       const char *	sourceFileName,
		       int 	 	sourceLineNo,
		       const char * 	sourceFunctionName,
		       const char *	message );

static ostream * stdLogStream = &cerr;


/**
 * Stream buffer class that will use the YUILog's logger function.
 *
 * See also http://blogs.awesomeplay.com/elanthis/archives/2007/12/10/
 **/
class YUILogBuffer: public std::streambuf
{
    friend class YUILog;

public:

    /**
     * Constructor.
     **/
    YUILogBuffer()
	: logComponent( 0 )
	, sourceFileName( 0 )
	, lineNo( 0 )
	, functionName( 0 )
	{}

    /**
     * Destructor.
     **/
    virtual ~YUILogBuffer()
	{ flush(); }

    /**
     * Write (no more than maxLength characters of) a sequence of characters
     * and return the number of characters written.
     *
     * Reimplemented from streambuf.
     * This is called for all output operations on the associated ostream.
     **/
    virtual std::streamsize xsputn( const char * sequence, std::streamsize maxLength );

    /**
     * Write one character in case of buffer overflow.
     *
     * Reimplemented from streambuf.
     **/
    virtual int overflow( int ch = EOF );

    /**
     * Write (no more than maxLength characters of) a sequence of characters
     * and return the number of characters written.
     *
     * This is the actual worker function that uses the YUILog::loggerFunction to
     * actually write characters.
     **/
    std::streamsize writeBuffer( const char * sequence, std::streamsize seqLen );

    /**
     * Flush the output buffer: Write any data unwritten so far.
     **/
    void flush();


private:

    YUILogLevel_t	logLevel;
    const char *	logComponent;
    const char *	sourceFileName;
    int			lineNo;
    const char *	functionName;

    string		buffer;
};



std::streamsize
YUILogBuffer::writeBuffer( const char * sequence, std::streamsize seqLen )
{
    // Add new character sequence

    if ( seqLen > 0 )
	buffer += string( sequence, seqLen );

    //
    // Output buffer contents line by line
    //

    size_t start       = 0;
    size_t newline_pos = 0;

    while ( start < buffer.length() &&
	    ( newline_pos = buffer.find_first_of( '\n', start ) ) != string::npos )
    {
	YUILoggerFunction loggerFunction = YUILog::loggerFunction( true ); // never return 0

	string line = buffer.substr( start, newline_pos - start );

	loggerFunction( logLevel, logComponent,
			YUILog::basename( sourceFileName ).c_str(), lineNo, functionName,
			line.c_str() );

	start = newline_pos + 1;
    }

    if ( start < buffer.length() )
	buffer = buffer.substr( start, string::npos );
    else
	buffer.clear();

    return seqLen;
}


std::streamsize
YUILogBuffer::xsputn( const char * sequence, std::streamsize maxLength )
{
    return writeBuffer( sequence, maxLength );
}


int
YUILogBuffer::overflow( int ch )
{
    if ( ch != EOF )
    {
	char sequence = ch;
	writeBuffer( &sequence, 1 );
    }

    return 0;
}


void YUILogBuffer::flush()
{
    writeBuffer( "\n", 1 );
}





/**
 * Helper class: Per-thread logging information.
 *
 * Multiple threads can easily clobber each others' half-done logging.
 * A naive approach to prevent this would be to lock a mutex when a thread
 * starts logging and unlock it when it's done logging. But that "when it's
 * done" condition might never come true. endl or a newline in the output
 * stream would be one indication, but there is no way to make sure there
 * always is such a delimiter. If it is forgotten and that thread (that still
 * has the mutex locked) runs into a waiting condition itself (e.g., UI thread
 * synchronization with pipes), there would be a deadlock.
 *
 * So this much safer approach was chosen: Give each thread its own logging
 * infrastructure, i.e., its own log stream and its own log buffer.
 *
 * Sure, in bad cases the logger function might still be executed in parallel
 * and thus clobber a line or two of log output. But that's merely bad output
 * formatting, not writing another thread's data structures without control -
 * which can easily happen if multiple threads are working on the same output
 * buffer, i.e. manipulate the same string.
 **/
struct YPerThreadLogInfo
{
    /**
     * Constructor
     **/
    YPerThreadLogInfo()
	: threadHandle( pthread_self() )
	, logBuffer()
	, logStream( &logBuffer )
        {
            // cerr << "New thread with ID " << hex << threadHandle << dec << endl;
        }

    /**
     * Destructor
     **/
    ~YPerThreadLogInfo()
        {
            logBuffer.flush();
        }

    /**
     * Check if this per-thread logging information belongs to the specified thread.
     **/
    bool isThread( pthread_t otherThreadHandle )
        {
            return pthread_equal( otherThreadHandle, this->threadHandle );
        }


    //
    // Data members
    //

    pthread_t		threadHandle;
    YUILogBuffer	logBuffer;
    ostream             logStream;
};




struct YUILogPrivate
{
    /**
     * Constructor
     **/
    YUILogPrivate()
	: loggerFunction( stdLogger )
	, enableDebugLoggingHook( 0 )
	, debugLoggingEnabledHook( 0 )
	, enableDebugLogging( false )
	{}

    /**
     * Destructor
     **/
    ~YUILogPrivate()
        {
            for ( unsigned i=0; i < threadLogInfo.size(); i++ )
                delete threadLogInfo[i];
        }

    /**
     * Find the per-thread logging information for the current thread.
     * Create a new one if it doesn't exist yet.
     **/
    YPerThreadLogInfo * findCurrentThread()
        {
            pthread_t thisThread = pthread_self();

            // Search backwards: Slight optimization for the UI.
            // The UI thread does the most logging, but it is created after the
            // main thread.

            for ( std::vector<YPerThreadLogInfo *>::reverse_iterator it = threadLogInfo.rbegin();
                  it != threadLogInfo.rend();
                  ++it )
            {
                if ( (*it)->isThread( thisThread ) )
                    return (*it);
            }

            YPerThreadLogInfo * newThreadLogInfo = new YPerThreadLogInfo();
            threadLogInfo.push_back( newThreadLogInfo );

            return newThreadLogInfo;
        }

    //
    // Data members
    //

    string				logFileName;
    std::ofstream			stdLogStream;
    YUILoggerFunction			loggerFunction;
    YUIEnableDebugLoggingFunction	enableDebugLoggingHook;
    YUIDebugLoggingEnabledFunction	debugLoggingEnabledHook;
    bool				enableDebugLogging;

    std::vector<YPerThreadLogInfo *>    threadLogInfo;
};




YUILog::YUILog()
    : priv( new YUILogPrivate() )
{
    YUI_CHECK_NEW( priv );
}


YUILog::~YUILog()
{
    if ( priv->stdLogStream.is_open() )
	priv->stdLogStream.close();
}


YUILog *
YUILog::instance()
{
    static YUILog * instance = 0;

    if ( ! instance )
    {
	instance = new YUILog();
	YUI_CHECK_NEW( instance );
    }

    return instance;
}


bool
YUILog::setLogFileName( const string & logFileName )
{
    instance()->priv->logFileName = logFileName;

    std::ofstream & logStream = instance()->priv->stdLogStream;

    if ( logStream.is_open() )
	logStream.close();

    bool success = true;

    if ( logFileName.empty() ) // log to stderr again
    {
	stdLogStream = &cerr;
    }
    else
    {
	logStream.open( logFileName.c_str(), std::ios_base::app );
	success = logStream.good();

	if ( success )
	{
	    stdLogStream = &( instance()->priv->stdLogStream );
	}
	else
	{
	    cerr << "ERROR: Can't open log file " << logFileName << endl;
	    stdLogStream = &cerr;
	}
    }

    return success;
}


string
YUILog::logFileName()
{
    return instance()->priv->logFileName;
}


void
YUILog::enableDebugLogging( bool debugLogging )
{
    instance()->priv->enableDebugLogging = debugLogging;

    if ( instance()->priv->enableDebugLoggingHook )
	instance()->priv->enableDebugLoggingHook( debugLogging );
}


bool
YUILog::debugLoggingEnabled()
{
    if ( instance()->priv->debugLoggingEnabledHook )
	return instance()->priv->debugLoggingEnabledHook();
    else
	return instance()->priv->enableDebugLogging;
}


void
YUILog::setLoggerFunction( YUILoggerFunction loggerFunction )
{
    if ( ! loggerFunction )
	loggerFunction = stdLogger;

    instance()->priv->loggerFunction = loggerFunction;
}


YUILoggerFunction
YUILog::loggerFunction( bool returnStdLogger )
{
    YUILoggerFunction logger = instance()->priv->loggerFunction;

    if ( logger == stdLogger && ! returnStdLogger )
	logger = 0;

    return logger;
}


void
YUILog::setEnableDebugLoggingHooks( YUIEnableDebugLoggingFunction  enableFunction,
				    YUIDebugLoggingEnabledFunction isEnabledFunction )
{
    instance()->priv->enableDebugLoggingHook  = enableFunction;
    instance()->priv->debugLoggingEnabledHook = isEnabledFunction;
}


YUIEnableDebugLoggingFunction
YUILog::enableDebugLoggingHook()
{
    return instance()->priv->enableDebugLoggingHook;
}


YUIDebugLoggingEnabledFunction
YUILog::debugLoggingEnabledHook()
{
    return instance()->priv->debugLoggingEnabledHook;
}


ostream &
YUILog::log( YUILogLevel_t	logLevel,
	     const char *	logComponent,
	     const char *	sourceFileName,
	     int 		lineNo,
	     const char * 	functionName )
{
    YPerThreadLogInfo * threadLogInfo = priv->findCurrentThread();

    if ( ! threadLogInfo->logBuffer.buffer.empty() )	// Leftovers from previous logging?
    {
	if ( threadLogInfo->logBuffer.logLevel != logLevel ||
	     threadLogInfo->logBuffer.lineNo   != lineNo   ||
	     strcmp( threadLogInfo->logBuffer.logComponent,   logComponent   ) != 0 ||
	     strcmp( threadLogInfo->logBuffer.sourceFileName, sourceFileName ) != 0 ||
	     strcmp( threadLogInfo->logBuffer.functionName,   functionName   ) != 0   )
	{
	    threadLogInfo->logBuffer.flush();
	}
    }

    threadLogInfo->logBuffer.logLevel		= logLevel;
    threadLogInfo->logBuffer.logComponent	= logComponent;
    threadLogInfo->logBuffer.sourceFileName	= sourceFileName;
    threadLogInfo->logBuffer.lineNo		= lineNo;
    threadLogInfo->logBuffer.functionName	= functionName;

    return threadLogInfo->logStream;
}


ostream &
YUILog::debug( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName )
{
    return instance()->log( YUI_LOG_DEBUG, logComponent, sourceFileName, lineNo, functionName );
}


ostream &
YUILog::milestone( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName )
{
    return instance()->log( YUI_LOG_MILESTONE, logComponent, sourceFileName, lineNo, functionName );
}


ostream &
YUILog::warning( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName )
{
    return instance()->log( YUI_LOG_WARNING, logComponent, sourceFileName, lineNo, functionName );
}


ostream &
YUILog::error( const char * logComponent, const char * sourceFileName, int lineNo, const char * functionName )
{
    return instance()->log( YUI_LOG_ERROR, logComponent, sourceFileName, lineNo, functionName );
}



string
YUILog::basename( const string & fileNameWithPath )
{
    size_t lastSlashPos = fileNameWithPath.find_last_of( '/' );

    string fileName =
	( lastSlashPos == string::npos ) ?
	fileNameWithPath :
	fileNameWithPath.substr( lastSlashPos+1 );

    return fileName;
}



static void
stdLogger( YUILogLevel_t	logLevel,
	   const char *		logComponent,
	   const char *		sourceFileName,
	   int 	 		sourceLineNo,
	   const char * 	sourceFunctionName,
	   const char *		message )
{
    const char * logLevelStr = "";

    switch ( logLevel )
    {
	case YUI_LOG_DEBUG:
	    if ( ! YUILog::debugLoggingEnabled() )
		return;

	    logLevelStr = "dbg";
	    break;

	case YUI_LOG_MILESTONE:	logLevelStr = "_M_";	break;
	case YUI_LOG_WARNING:	logLevelStr = "WRN";	break;
	case YUI_LOG_ERROR:	logLevelStr = "ERR";	break;
    }

    if ( ! logComponent )
	logComponent = "??";

    if ( ! sourceFileName )
	sourceFileName = "??";

    if ( ! sourceFunctionName )
	sourceFunctionName = "??";

    if ( ! message )
	message = "";

    (*stdLogStream) << "<" << logLevelStr  << "> "
		    << "[" << logComponent << "] "
		    << sourceFileName	   << ":" << sourceLineNo << " "
		    << sourceFunctionName  << "(): "
		    << message
		    << endl;
}
