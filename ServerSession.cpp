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
// -----------------------------------------------------------------------------
#include <e32math.h>

#include "Common.h"
#include "ServerSession.h"
// -----------------------------------------------------------------------------
static TInt StartServer();
static TInt CreateServerProcess();
// -----------------------------------------------------------------------------
RServerSession::RServerSession()
    : RSessionBase()
{
}
// -----------------------------------------------------------------------------
TInt RServerSession::Connect()
{
    TInt error = ::StartServer();

    if (KErrNone == error)
        error = CreateSession(KMCServerName, Version(), KDefaultMessageSlots);
    
    return error;
}
// -----------------------------------------------------------------------------
TVersion RServerSession::Version() const
{
    return TVersion(KMCServMajorVersionNumber,
        KMCServMinorVersionNumber,
        KMCServBuildVersionNumber);
}
// -----------------------------------------------------------------------------
void RServerSession::RequestCommand(TRequestCommand &aCmd) const
{
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aCmd), sizeof(aCmd), sizeof(aCmd));
    TIpcArgs args(&descriptor);
    SendReceive(EMCServRequestOp, args);
}
// -----------------------------------------------------------------------------
static TInt StartServer()
{
    TInt result;

    TFindServer findServer(KMCServerName);
    TFullName name;

    result = findServer.Next(name);
    
    if (result == KErrNone)     // Server already running
        return KErrNone;

    RSemaphore semaphore;
    
    result = semaphore.CreateGlobal(KMCServerSemaphoreName, 0);
    if (result != KErrNone)
        return  result;
    
    result = CreateServerProcess();
    if (result != KErrNone)
        return  result;

    semaphore.Wait();
    semaphore.Close();

    return KErrNone;
}
// -----------------------------------------------------------------------------
static TInt CreateServerProcess()
{
    TInt result;

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

    RProcess server;

    result = server.Create(KServerFilename, KNullDesC, serverUid);

    if (result != KErrNone)
        return  result;

    server.Resume();
    server.Close();

    return  KErrNone;
}
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//END OF FILE
////////////////////////////////////////////////////////////////////////////////
