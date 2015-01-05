/*
 ============================================================================
 Name           : MiniCMD
 Author         : Just Fancy
 Project Home   : http://code.google.com/p/minicmd/
 ============================================================================
    MiniCMD, mini commander for Symbian OS phone
    Copyright (C) 2011  Just Fancy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ============================================================================
 */
#ifndef __SERVER_SESSION_H__
#define __SERVER_SESSION_H__

#include <e32base.h>

#include "RequestCmd.h"

static const TUint KDefaultMessageSlots = 0;

// Server UID
const TUid KServerUid3 = { 0xE132DCB4 };

_LIT(KServerFilename, "MiniCMD_Server.EXE");

class RServerSession : public RSessionBase
{
public:
    RServerSession();

public: 
    TInt Connect();
    TVersion Version() const;
    void RequestCommand(TRequestCommand &aCmd) const;

};

#endif  //__SERVER_SESSION_H__
