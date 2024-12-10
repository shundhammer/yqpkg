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


#ifndef PkgCommitCallbacks_h
#define PkgCommitCallbacks_h


#include <QObject>

#include <zypp/Resolvable.h>
#include <zypp/Url.h>
#include <zypp/ZYppCallbacks.h>


class PkgCommitSignalForwarder;

using zypp::Resolvable;
using zypp::Pathname;
using zypp::Url;


/**
 * Signal forwarder for the callbacks so they can send Qt signals to a receiver
 * QObject without being subclasses of QObject themselves.
 *
 * This is a singleton class.
 **/
class PkgCommitSignalForwarder: public QObject
{
   Q_OBJECT

protected:

    /**
     * Constructor. Use instance() instead.
     **/
    PkgCommitSignalForwarder()
        : QObject()
        {}

public:

    /**
     * Destructor.
     **/
    virtual ~PkgCommitSignalForwarder() { _instance = 0; }

    /**
     * Return the singleton of this class. Create it if it doesn't exist yet.
     **/
    static PkgCommitSignalForwarder * instance();

    /**
     * Connect all signals to slots with the same name in 'receiver'.
     **/
    void connectAll( QObject * receiver );


signals:

    //
    // The receiver needs to have a slot with the same name for each of these
    // signals:
    //

    void pkgDownloadStart();
    void pkgDownloadProgress();
    void pkgDownloadEnd();

    void pkgInstallStart();
    void pkgInstallProgress();
    void pkgInstallEnd();

    void pkgRemoveStart();
    void pkgRemoveProgress();
    void pkgRemoveEnd();


public:

    // Use each one with  PkgCommitSignalForwarder::instance()->sendPkg...()

    void sendPkgDownloadStart()     { emit pkgDownloadStart();    }
    void sendPkgDownloadProgress()  { emit pkgDownloadProgress(); }
    void sendPkgDownloadEnd()       { emit pkgDownloadEnd();      }

    void sendPkgInstallStart()      { emit pkgInstallStart();     }
    void sendPkgInstallProgress()   { emit pkgInstallProgress();  }
    void sendPkgInstallEnd()        { emit pkgInstallEnd();       }

    void sendPkgRemoveStart()       { emit pkgRemoveStart();      }
    void sendPkgRemoveProgress()    { emit pkgRemoveProgress();   }
    void sendPkgRemoveEnd()         { emit pkgRemoveEnd();        }


    static PkgCommitSignalForwarder * _instance;
};


//
//----------------------------------------------------------------------
//
// Libzypp callbacks; see /usr/include/zypp/ZYppCallbacks.h

struct PkgDownloadCallback:
    public zypp::callback::ReceiveReport<zypp::repo::DownloadResolvableReport>
{
    virtual void start( Resolvable::constPtr /*resolvable_ptr*/,
                        const Url & /*url*/ )
        {
            PkgCommitSignalForwarder::instance()->sendPkgDownloadStart();
        }


    virtual bool progress( int /*value*/,
                           Resolvable::constPtr /*resolvable_ptr*/)
        {
            PkgCommitSignalForwarder::instance()->sendPkgDownloadProgress();

            return true; // Don't abort
        }


    virtual void finish( Resolvable::constPtr resolvable,
                         zypp::repo::DownloadResolvableReport::Error error,
                         const std::string & reason )
        {
            PkgCommitSignalForwarder::instance()->sendPkgDownloadEnd();
        }


    virtual zypp::repo::DownloadResolvableReport::Action
    problem( Resolvable::constPtr /*resolvable_ptr*/,
             zypp::repo::DownloadResolvableReport::Error /*error*/,
             const std::string & /*description*/ )
        {
            return zypp::repo::DownloadResolvableReport::ABORT;
        }


    /**
     * Hint that package is available in the local cache (no download needed).
     * This will be the only trigger for an already cached package.
     **/
    virtual void infoInCache( Resolvable::constPtr res,
                              const Pathname & /*localfile*/ )
        {}

#if 0
    // FIXME: TO DO later (much later...)

    virtual void pkgGpgCheck( const UserData & userData_r = UserData() )
        {}

    virtual void startDeltaDownload( const Pathname &  /*filename*/,
                                     const ByteCount & /*downloadSize*/ )
        {}

    virtual bool progressDeltaDownload( int /*value*/ )
        {
            return true; // Don't abort
        }

    virtual void problemDeltaDownload( const std::string & /*description*/ )
        {}

    virtual void finishDeltaDownload()
        {}

    virtual void startDeltaApply( const Pathname & /*filename*/ )
        {}

    virtual void progressDeltaApply( int /*value*/ )
        {}

    virtual void problemDeltaApply( const std::string & /*description*/ )
        {}

    virtual void finishDeltaApply()
        {}
#endif

}; // PkgDownloadCallback


struct PkgInstallCallback:
    public zypp::callback::ReceiveReport<zypp::target::rpm::InstallResolvableReport>
{
    virtual void start( Resolvable::constPtr /*resolvable*/ )
        {
            PkgCommitSignalForwarder::instance()->sendPkgInstallStart();
        }


    virtual bool progress( int /*value*/,
                           Resolvable::constPtr /*resolvable*/)
        {
            PkgCommitSignalForwarder::instance()->sendPkgInstallProgress();

            return true; // Don't abort
        }


    virtual void finish( Resolvable::constPtr /*resolvable*/,
                         zypp::target::rpm::InstallResolvableReport::Error /*error*/,
                         const std::string & /*reason*/,
                         RpmLevel /*level*/ )
        {
            PkgCommitSignalForwarder::instance()->sendPkgInstallProgress();
        }


    virtual zypp::target::rpm::InstallResolvableReport::Action
    problem( Resolvable::constPtr /*resolvable*/,
             Error /*error*/,
             const std::string & /*description*/,
             RpmLevel /*level*/ )
        {
            return zypp::target::rpm::InstallResolvableReport::ABORT;
        }

}; // PkgInstallCallback


struct PkgRemoveCallback:
    public zypp::callback::ReceiveReport<zypp::target::rpm::RemoveResolvableReport>
{
    virtual void start( Resolvable::constPtr /*resolvable*/ )
        {
            PkgCommitSignalForwarder::instance()->sendPkgRemoveStart();
        }


    virtual bool progress( int /*value*/,
                           Resolvable::constPtr /*resolvable*/)
        {
            PkgCommitSignalForwarder::instance()->sendPkgRemoveProgress();

            return true; // Don't abort
        }


    virtual void finish( Resolvable::constPtr /*resolvable*/,
                         zypp::target::rpm::RemoveResolvableReport::Error /*error*/,
                         const std::string & /*reason*/ )
        {
            PkgCommitSignalForwarder::instance()->sendPkgRemoveEnd();
        }


    virtual zypp::target::rpm::RemoveResolvableReport::Action
    problem( Resolvable::constPtr /*resolvable*/,
             zypp::target::rpm::RemoveResolvableReport::Error /*error*/,
             const std::string & /*description*/ )
        {
            return zypp::target::rpm::RemoveResolvableReport::ABORT;
        }

}; // PkgRemoveCallback


//
//----------------------------------------------------------------------
//


/**
 * Class to bundle the zypp callbacks needed during a zypp package commit and
 * to translate each libzypp event ("report" in libzypp lingo) into a Qt signal.
 *
 * The constructor instantiates and connects the callbacks, the destructor
 * disconnects and deletes them; so the instance of this object needs to live
 * until the commit is finished.
 **/
class PkgCommitCallbacks
{
public:

    /**
     * Constructor: Create the needed callbacks and connect them (register them
     * with libzypp).
     *
     * Remember to call PkgCommitSignalForwarder::connectAll( receiver )
     * once to get the signals that the callbacks send via the signal forwarder.
     **/
    PkgCommitCallbacks();

    /**
     * Destructor: Disconnect (unregister them with libzypp) and delete the
     * callbacks.
     **/
    virtual ~PkgCommitCallbacks();


protected:

    PkgDownloadCallback _pkgDownloadCallback;
    PkgInstallCallback  _pkgInstallCallback;
    PkgRemoveCallback   _pkgRemoveCallback;
};


#endif // PkgCommitCallbacks_h
