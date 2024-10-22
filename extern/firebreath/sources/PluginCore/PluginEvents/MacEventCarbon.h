/**********************************************************\
Original Author: Richard Bateman (taxilian)

Created:    Oct 19, 2009
License:    Dual license model; choose one of two:
            New BSD License
            http://www.opensource.org/licenses/bsd-license.php
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 PacketPass, Inc and the Firebreath development team
\**********************************************************/

#ifndef H_FB_EVENTS_MACEVENT
#define H_FB_EVENTS_MACEVENT

#ifndef XP_MACOSX
#define EventRecord NPEvent
#else
#include <Carbon/Carbon.h>
#endif

#include "PluginEvent.h"

namespace FB {

    class MacEventCarbon : public PluginEvent
    {
    public:
        MacEventCarbon(EventRecord *src)
        {
            memcpy(&msg, src, sizeof(EventRecord));
        }

    public:
        EventRecord msg;
    };
};

#endif
