/*  ---------------------------------------------------------
               __  __            _
              |  \/  |_   _ _ __| |_   _ _ __
              | |\/| | | | | '__| | | | | '_ \
              | |  | | |_| | |  | | |_| | | | |
              |_|  |_|\__, |_|  |_|\__, |_| |_|
                      |___/        |___/
    ---------------------------------------------------------

    Project:  Myrlyn Package Manager GUI
    Copyright (c) 2024-25 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#include "Exception.h"
#include "Logger.h"
#include "KeyRingCallbacks.h"


KeyRingCallbacks::KeyRingCallbacks()
{
    _keyRingReceiveCallback.connect();
}


KeyRingCallbacks::~KeyRingCallbacks()
{
    // Each zypp::Callback disconnects automatically in the dtor.
}
