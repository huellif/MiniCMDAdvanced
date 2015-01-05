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
/*
 * MiniLog.cpp
 *
 *  Created on: 2011-5-12
 *      Author: Just Fancy
 */
//=================================================================================
#include "MiniLog.h"
//=================================================================================
_LIT(KWrap, "\r\n");
//=================================================================================
CMiniLog::~CMiniLog()
{
    iOutputStream.Close();
    iFile.Close();
}
//=================================================================================
CMiniLog* CMiniLog::NewLC(RFs &aFs, const TDesC &aFile)
{
    CMiniLog* self = new (ELeave)CMiniLog();

    CleanupStack::PushL(self);

    self->ConstructL(aFs, aFile);

    return self;
}
//=================================================================================
CMiniLog* CMiniLog::NewL(RFs &aFs, const TDesC &aFile)
{
    CMiniLog* self = CMiniLog::NewLC(aFs, aFile);

    CleanupStack::Pop();

    return self;
}
//=================================================================================
void CMiniLog::ConstructL(RFs &aFs, const TDesC &aFile)
{
    iFs = &aFs;
    
    aFs.MkDirAll(aFile);    //Fixed: when target dir does not exists, Create failed
                            //Jun. 22, 2011   
    if (iFile.Open(*iFs, aFile, EFileWrite) == KErrNotFound)
    {
        User::LeaveIfError(iFile.Create(*iFs, aFile, EFileWrite));
        
        iOutputStream.Attach(iFile);
        
        //write utf-16 bom
        //so that x-plore can recognize
        iOutputStream.WriteInt8L(0xFF);
        iOutputStream.WriteInt8L(0xFE);
        iOutputStream.CommitL();
    }
    else
    {
        iOutputStream.Attach(iFile);
    }

    iOutputStream.Sink()->SeekL(MStreamBuf::EWrite, EStreamEnd);
}
//=================================================================================
void CMiniLog::Log(TRefByValue<const TDesC16> aFmt, ...)
{
    VA_LIST ap;
    VA_START(ap, aFmt);

    TBuf16<512> iBuf;
    
    GetTimeStr(iBuf);
    
    iBuf.AppendFormatList(aFmt, ap);

    VA_END(ap);

    iBuf.Append(KWrap);
 
    iOutputStream.WriteL(iBuf);
    iOutputStream.CommitL();
}
//=================================================================================
void CMiniLog::GetTimeStr(TDes &aBuf)
{
    TTime currentTime;
    currentTime.HomeTime();
    currentTime.FormatL(aBuf, _L("[%H:%T:%S] "));
}
//////////////////////////////////////////////////////////////////////////////////
//END OF FILE
//////////////////////////////////////////////////////////////////////////////////
