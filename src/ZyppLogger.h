/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef ZyppLogger_h
#define ZyppLogger_h


#include <string>

#include <zypp/base/Logger.h>
#include <zypp/base/LogControl.h>



/**
 * Class that writes a preformatted zypp log line.
 **/
struct ZyppLogLineWriter: public zypp::base::LogControl::LineWriter
{
    virtual void writeOut( const std::string & formated_msg );
};


/**
 * Class that formats a zypp log line.
 **/
struct ZyppLogLineFormatter: public zypp::base::LogControl::LineFormater
{
    virtual std::string
    format( const std::string &          log_group,
            zypp::base::logger::LogLevel log_level,
            const char *                 src_file,
            const char *                 src_func,
            int                          src_line,
            const std::string &          message );
};


/**
 * Class to redirect the Zypp log to the application's log for the life time of
 * this object.
 **/
class ZyppLogger
{
public:

    ZyppLogger();
    ~ZyppLogger();

private:

    boost::shared_ptr<ZyppLogLineWriter>    _lineWriter;
    boost::shared_ptr<ZyppLogLineFormatter> _lineFormatter;
};

#endif // ZyppLogger_h
