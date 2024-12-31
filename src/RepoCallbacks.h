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


#ifndef RepoCallbacks_h
#define RepoCallbacks_h

#include <QObject>
#include <zypp/ZYppCallbacks.h>

#include "Exception.h"
#include "Logger.h"
#include "utf8.h"


/**
 * Signal forwarder for the callbacks so they can send Qt signals to a receiver
 * QObject without being subclasses of QObject themselves.
 *
 * See also PkgCommitCallbacks.h
 *
 * This is a singleton class.
 **/
class RepoSignalForwarder: public QObject
{
   Q_OBJECT

protected:

    /**
     * Constructor. Use instance() instead.
     **/
    RepoSignalForwarder()
        : QObject()
        , _doAbort( false )
        {}

public:

    /**
     * Destructor.
     **/
    virtual ~RepoSignalForwarder() { _instance = 0; }

    /**
     * Return the singleton of this class. Create it if it doesn't exist yet.
     **/
    static RepoSignalForwarder * instance();

    /**
     * Connect all refresh signals to slots with the same name in 'receiver'.
     **/
    void connectRefreshSignals( QObject * receiver );

    /**
     * Return 'true' if 'abortCommit()' has been received since the last
     * 'reset()'
     **/
    bool doAbort() const { return _doAbort; }

    /**
     * Reset the internal status, including the _doAbort flag.
     **/
    void reset();


signals:

    //
    // The receiver needs to have a slot with the same name for each of these
    // signals:
    //

    void repoRefreshStart   ( const QString & repoName );
    void repoRefreshProgress( int percent );
    void repoRefreshEnd();



public slots:

    void abortCommit() { _doAbort = true; }


public:

    // Use each one with  RepoSignalForwarder::instance()->sendRepo...()

    void sendRepoRefreshStart   ( const QString & repoName )  { emit repoRefreshStart   ( repoName ); }
    void sendRepoRefreshProgress( int percent )               { emit repoRefreshProgress( percent  ); }
    void sendRepoRefreshEnd     ()                            { emit repoRefreshEnd();                }


    //
    // Data members
    //

    bool _doAbort;

    static RepoSignalForwarder * _instance;
};


//
//----------------------------------------------------------------------
//
// Libzypp callbacks; see /usr/include/zypp/ZYppCallbacks.h


typedef zypp::repo::RepoReport::Action  ZyppRepoAction;
typedef zypp::repo::RepoReport::Error   ZyppRepoError;


struct ZyppRepoRefreshCallback:
    public zypp::callback::ReceiveReport<zypp::repo::RepoReport>
{
    virtual void start( const zypp::ProgressData & progressData,
                        const zypp::RepoInfo       repo )
        {
            Q_UNUSED( progressData );

            RepoSignalForwarder::instance()->sendRepoRefreshStart( fromUTF8( repo.name() ) );
        }


    virtual bool progress( const zypp::ProgressData & progressData )
        {
            int percent = progressData.reportValue();
            RepoSignalForwarder::instance()->sendRepoRefreshProgress( percent );

            return true;  // Don't abort
        }


    virtual void finish( zypp::Repository    repo,
                         const std::string & task,
                         ZyppRepoError       error,
                         const std::string & reason )
        {
            Q_UNUSED( repo   );
            Q_UNUSED( task   );
            Q_UNUSED( error  );
            Q_UNUSED( reason );

            RepoSignalForwarder::instance()->sendRepoRefreshEnd();
        }


    virtual ZyppRepoAction problem( zypp::Repository    repo,
                                    ZyppRepoError       error,  // NO_ERROR, NOT_FOUND, IO, INVALID
                                    const std::string & errorDescription )
        {
            Q_UNUSED( repo  );
            Q_UNUSED( error );

            logError() << errorDescription << endl;

            return zypp::repo::RepoReport::IGNORE;
        }

};


/**
 * Class to bundle the zypp callbacks needed while refreshing repos.
 *
 * The constructor instantiates and connects the callbacks, the destructor
 * disconnects and deletes them; so the instance of this object needs to live
 * until all repos are refreshed.
 **/
class RepoRefreshCallbacks
{
public:

    /**
     * Constructor: Create the needed callbacks and connect them (register them
     * with libzypp).
     *
     * Remember to call RepoSignalForwarder::connectAll( receiver )
     * once to get the signals that the callbacks send via the signal forwarder.
     **/
    RepoRefreshCallbacks();

    /**
     * Destructor: Disconnect (unregister them with libzypp) and delete the
     * callbacks.
     **/
    virtual ~RepoRefreshCallbacks();


protected:

    ZyppRepoRefreshCallback _zyppRepoRefreshCallback;
};


#endif // RepoCallbacks_h
