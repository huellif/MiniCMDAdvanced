/*
 ============================================================================
 Name		    : MiniCMD.cpp
 Author	        : Just Fancy
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
#include <e32math.h>
#include <APGCLI.H>
#include <APACMDLN.H>
#include <W32STD.H>
#include <aknglobalmsgquery.h>
#include <aknglobalnote.h>
#include <BAFINDF.H>
#include <BADESCA.H>
#include <SWInstApi.h>

#include "MiniCMD.h"
#include "Constant.h"
#include "MiniLog.h"
#include "ServerSession.h"
#include "Config.h"
//=================================================================================
RFs iFs;
CMiniLog *miniLog = NULL;
RServerSession iServer;

TBool iLogAll   = EFalse;   // whether log all conditions
TInt  iLastErr  = KErrNone;
TBool iStop     = EFalse;   // stop minicmd

Config iConfig;

//=================================================================================
#define _LOG_(what) \
    if(miniLog)     \
        miniLog->Log(what)

#define _LOG_FMT_(fmt, what)\
    if(miniLog)             \
        miniLog->Log(fmt, what)
//=================================================================================
LOCAL_C void MainL()
{
    CArrayFixFlat<TCommand> *iCmdSet = new(ELeave) CArrayFixFlat<TCommand>(10);
    if (!iCmdSet)
    {
//        CleanupStack::PopAndDestroy();
        return ;
    }
    CleanupStack::PushL(iCmdSet);

    _LOG_(_L("[Starting MiniCMD]"));

    TInt ret = iServer.Connect();
    if (ret != KErrNone && miniLog != NULL)
        miniLog->Log(_L("[Start Server Failed] [%d]"), ret);

    if (!IsCmdDisabled())
    {
        TInt ret = KErrNotFound;

        TInt count = iConfig.GetDriveLetters().Length();
        if (iConfig.GetCMDFile().Length() == 0)
            count = 0;
        for (TInt i=0; i<count; i++)
        {
            TFileName cmdFile;

            if (iConfig.GetCmdFileX('C').Length() > 0
                    && iConfig.GetDriveLetters()[i]
                            == iConfig.GetCmdFileX('C')[0])
            {
                if (IsPathFileExists(iConfig.GetCmdFileX('C')))
                {
                    ret = LoadCmdFileL(iConfig.GetCmdFileX('C'), iCmdSet);
                    break;
                }
            }
            else if (iConfig.GetCmdFileX('D').Length() > 0
                    && iConfig.GetDriveLetters()[i]
                            == iConfig.GetCmdFileX('D')[0])
            {
                if (IsPathFileExists(iConfig.GetCmdFileX('D')))
                {
                    ret = LoadCmdFileL(iConfig.GetCmdFileX('D'), iCmdSet);
                    break;
                }
            }
            if (iConfig.GetCmdFileX('E').Length() > 0
                    && iConfig.GetDriveLetters()[i]
                            == iConfig.GetCmdFileX('E')[0])
            {
                if (IsPathFileExists(iConfig.GetCmdFileX('E')))
                {
                    ret = LoadCmdFileL(iConfig.GetCmdFileX('E'), iCmdSet);
                    break;
                }
            }
            if (iConfig.GetCmdFileX('Y').Length() > 0
                    && iConfig.GetDriveLetters()[i]
                            == iConfig.GetCmdFileX('Y')[0])
            {
                if (IsPathFileExists(iConfig.GetCmdFileX('Y')))
                {
                    ret = LoadCmdFileL(iConfig.GetCmdFileX('Y'), iCmdSet);
                    break;
                }
            }
            if (iConfig.GetCmdFileX('Z').Length() > 0
                    && iConfig.GetDriveLetters()[i]
                            == iConfig.GetCmdFileX('Z')[0])
            {
                if (IsPathFileExists(iConfig.GetCmdFileX('Z')))
                {
                    ret = LoadCmdFileL(iConfig.GetCmdFileX('Z'), iCmdSet);
                    break;
                }
            }

            cmdFile.Append(iConfig.GetDriveLetters()[i]);
            cmdFile.Append(':');
            if (iConfig.GetCMDFile()[0] != '\\')
                cmdFile.Append('\\');
            cmdFile.Append(iConfig.GetCMDFile());

            if (IsPathFileExists(cmdFile))
            {
                ret = LoadCmdFileL(cmdFile, iCmdSet);
                break;
            }
        }

        if (ret == KErrNotFound)
        {
            _LOG_(_L("[Not Found CMD file]"));
        }
        else if (iCmdSet->Count() == 0)
        {
            _LOG_(_L("[No Valid CMD set]"));
        }
        else
        {
            Run(*iCmdSet);
        }
    }
    else {
        _LOG_(_L("[MiniCMD disabled]"));
    }

    _LOG_(_L("[Exiting MiniCMD normally]\r\n"));

    CleanupStack::PopAndDestroy(iCmdSet);

    iServer.Close();
}
//=================================================================================
LOCAL_C void DoStartL()
{
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    CleanupStack::PopAndDestroy(scheduler);
}
//=================================================================================
GLDEF_C TInt E32Main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    User::LeaveIfError(iFs.Connect());

    TInt iErr = KErrNone;
    TBool loadOK = EFalse;

    TFileName iniFile;
    iniFile.Copy(_L("D:\\System\\Apps\\MiniCMD\\"));

    iniFile.Append(RProcess().Name());
    iErr = iniFile.Locate('.');

    // 应该 不会没有.的情况吧
    if (iErr != KErrNotFound)
    {
        iniFile.Replace(iErr, iniFile.Length() - iErr, _L(".ini"));
    }

    // 先D、E后C再Z
    if (!IsPathFileExists(iniFile))
    {
        iniFile[0] = 'E';
        if (!IsPathFileExists(iniFile))
            iniFile[0] = 'C';

        if (!IsPathFileExists(iniFile))
            iniFile[0] = 'Z';
    }

    TRAP(iErr, loadOK = iConfig.LoadL(iFs, iniFile));

    if (iErr == KErrNone && loadOK)
    {
        if (iConfig.GetLogLevel() != 0 && iConfig.GetLogFile().Length() != 0)
        {
            TRAP(iErr, miniLog = CMiniLog::NewL(iFs, iConfig.GetLogFile()));
            if (iErr != KErrNone || !IsPathFileExists(iConfig.GetLogFile()))
            {
                if (miniLog)
                    delete miniLog;
                miniLog = NULL;
            }
            else if(iConfig.GetLogLevel() == 2)
                iLogAll = ETrue;
        }
    }

    TRAP(iErr, DoStartL());

    if (miniLog)
    {
        if (iErr != KErrNone)
            miniLog->Log(_L("[Main::TRAP] [%d]\r\n"), iErr);

        delete miniLog;
    }

    iFs.Close();
    delete cleanup;
    __UHEAP_MARKEND;

    return KErrNone;
}
//=================================================================================
TInt LoadCmdFileL(const TDesC &aFileName, CArrayFixFlat<TCommand> *aCmdSet)
{
    RFile file;
    TInt errCode = KErrNone;

    errCode = file.Open(iFs, aFileName, EFileShareReadersOnly | EFileRead);

    if (miniLog)
    {
        HBufC *logBuf = HBufC::NewLC(260);
        TPtr bufPtr = logBuf->Des();

        bufPtr.Append(_L("[Load "));
        bufPtr.Append(aFileName);
        bufPtr.Append(KLogFmt);

        miniLog->Log(bufPtr, errCode);

        CleanupStack::PopAndDestroy(logBuf);
    }

    if (errCode != KErrNone)
        return errCode;

    CleanupClosePushL(file);

    TFileText fileText;
    fileText.Set(file);

    //one line should not exceed 512 bytes
    HBufC* heapBuf = HBufC::NewLC(512);
    TPtr lineBuf = heapBuf->Des();

    while (true)
    {
        if ((errCode = fileText.Read(lineBuf)) == KErrEof)
            break;

        ParseLineL(lineBuf, aCmdSet);
    }

    CleanupStack::PopAndDestroy(heapBuf);
    CleanupStack::PopAndDestroy(&file);

    if (KErrEof == errCode)
        errCode = KErrNone;

    return errCode;
}
//=================================================================================
void ParseLineL(TDes &aLine, CArrayFixFlat<TCommand> *aCmdSet)
{
    TBuf<24> cmd;

    if (!GetCMD(aLine, cmd))
        return ;

    cmd.Trim();
    cmd.LowerCase();    //case insensitive

    aLine.Delete(0, cmd.Length());
    aLine.TrimLeft();

    Parameter iParam;
    TFileName iSrc, iDest;

    ParseCMD(aLine, iSrc, &iDest, &iParam);
    iSrc.Trim();
    iDest.Trim();

    if (cmd == KCmdAtt)
    {
        if (iParam.NeedToSetAtt() && iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EAtt, &iParam, &iSrc));
    }
    else if(cmd == KCmdCp || cmd == KCmdCopy)
    {
        if (iDest.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ECp, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdMv)
    {
        if (iDest.Length() > 0  && iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EMv, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdMd || cmd == KCmdMkDir)
    {
        if (iSrc.Length() > 0 && iSrc[iSrc.Length() - 1] != '\\')
            iSrc.Append('\\');

        aCmdSet->AppendL(TCommand(TCommand::EMd, NULL, &iSrc));
    }
    else if(cmd == KCmdRn || cmd == KCmdRename)
    {
        if (iSrc.Length() > 0 && iDest.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ERn, NULL, &iSrc, &iDest));
    }
    else if(cmd == KCmdRm || cmd == KCmdRmDir)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ERm, &iParam, &iSrc));
    }
    else if (cmd == KCmdDel)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EDel, &iParam, &iSrc));
    }
    else if (cmd == KCmdRun)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ERun, &iParam, &iSrc));
    }
    else if(cmd == KCmdKill)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EKill, NULL, &iSrc));
    }
    else if(cmd == KCmdSleep)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ESleep, NULL, &iSrc));
    }
    else if(cmd == KCmdAbort)
    {
        aCmdSet->AppendL(TCommand(TCommand::EAbort, NULL, NULL));
    }
    else if(cmd == KCmdAbes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EAbes, &iParam, &iSrc));
    }
    else if(cmd == KCmdAbnes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EAbnes, &iParam, &iSrc));
    }
    else if(cmd == KCmdSkes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ESkes, &iParam, &iSrc));
    }
    else if(cmd == KCmdSknes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ESknes, &iParam, &iSrc));
    }
    else if(cmd == KCmdBkes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EBkes, &iParam, &iSrc));
    }
    else if(cmd == KCmdBknes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EBknes, &iParam, &iSrc));
    }
    else if(cmd == KCmdRses)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ERses, &iParam, &iSrc));
    }
    else if(cmd == KCmdRsnes)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ERsnes, &iParam, &iSrc));
    }
    else if (cmd == KCmdCmd)
    {
        // LoadCmdFileL(iSrc);
        // Jul. 19, 2011
        // Change to dynamic load

        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ECmd, NULL, &iSrc));
    }
    else if(cmd == KCmdIf)
    {
        aCmdSet->AppendL(TCommand(TCommand::EIf, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdIfn)
    {
        aCmdSet->AppendL(TCommand(TCommand::EIfn, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdElse)
    {
        aCmdSet->AppendL(TCommand(TCommand::EElse, NULL, NULL));
    }
    else if(cmd == KCmdElseIf)
    {
        aCmdSet->AppendL(TCommand(TCommand::EElseIf, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdElseIfn)
    {
        aCmdSet->AppendL(TCommand(TCommand::EElseIfn, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdEndIf)
    {
        aCmdSet->AppendL(TCommand(TCommand::EEndIf, NULL, NULL));
    }
    else if(cmd == KCmdKey)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EKey, &iParam, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
    }
    else if(cmd == KCmdSendKey)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ESendKey, &iParam, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
    }
    else if(cmd == KCmdFile)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EFile, &iParam, &iSrc));
    }
    else if(cmd == KCmdNote)
    {
        if (iSrc.Length() > 0)
        {
            if (iParam.c == rm_it)
                aCmdSet->AppendL(TCommand(TCommand::ENote, &iParam, &iSrc, &iDest));
            else
                aCmdSet->AppendL(TCommand(TCommand::ENote, NULL, &iSrc));
        }
    }
    else if(cmd == KCmdLog)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::ELog, NULL, &iSrc));
    }
    else if(cmd == KCmdStop)
    {
        aCmdSet->AppendL(TCommand(TCommand::EStop, NULL, NULL));
    }
    else if (cmd == KCmdFind)
    {
        if (iSrc.Length() > 0 && iDest.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EFind, &iParam, &iSrc, &iDest));
    }
    else if (cmd == KCmdInstall)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EInstall, &iParam, &iSrc));
    }
    else if(cmd == KCmdUninstall)
    {
        if (iSrc.Length() > 0)
            aCmdSet->AppendL(TCommand(TCommand::EUninstall, NULL, &iSrc));
    }
    else if(cmd == KCmdLogPs)
    {
        aCmdSet->AppendL(TCommand(TCommand::ELogPs, NULL, &iSrc));
    }
}
//=================================================================================
TInt FindPath(TDes &aLine, TDes &aPath)
{
    aLine.TrimLeft();

    if (aLine.Length() == 0)
        return 0;

    if(aLine[0] == '"') //以引号开始的
    {
        int i=0;
        while(++i<aLine.Length() && aLine[i] != '"')
        {
            // \" ==> "
            // Added - Jul. 19, 2011
            // Removed - Jul. 27, 2011
            /*if (aLine[i] == '\\' && i+1 < aLine.Length() &&
                aLine[i + 1] == '"')
            {
                i++;
                aPath.Append(aLine[i]);
            }
            else*/
            {
                aPath.Append(aLine[i]);
            }
        }

        return i;
    }
    else
    {
        int i=-1;

        while(++i<aLine.Length() && aLine[i] != ' ')    //以空� �结束
        {
            /*if (aLine[i] == '\\' && i+1 < aLine.Length() &&
                 aLine[i + 1] == '"')
             {
                 i++;
                 aPath.Append(aLine[i]);
             }
             else*/
             {
                 aPath.Append(aLine[i]);
             }
        }

        return i;
    }
}
//=================================================================================
TInt GetParams(TDes &aLine, Parameter &aParam)
{
    aLine.TrimLeft();
    if (aLine.Length() < 2)
        return 0;

    int i = -1;

    AttStatus status = not_set;
    TBool bSlash = EFalse;  //is '/'

    while(++i < aLine.Length())
    {
        if (aLine[i] == ' ')    //skip the space
            continue;

        if ((aLine[i] == '-' || aLine[i] == '+' || aLine[i] == '/') && i != aLine.Length() - 1)
        {
            if (i + 2 < aLine.Length())
                if (aLine[i+2] != ' ')  //参数后必须有空� �
                    return i;

            bSlash = EFalse;

            if (aLine[i] == '+')
                status = add_to;
            else if (aLine[i] == '-')
                status = rm_it;
            else
                bSlash = ETrue;

            if (++i >= aLine.Length())
                break;

            //case insensitive
            if (aLine[i] == 's' || aLine[i] == 'S')
            {
                if (bSlash)
                    aParam.is = ETrue;
                else
                    aParam.s = status;
            }
            else if (aLine[i] == 'h' || aLine[i] == 'H')
                aParam.h = status;
            else if (aLine[i] == 'r' || aLine[i] == 'R')
                aParam.r = status;
            else if(aLine[i] == 'o' || aLine[i] == 'O')
                aParam.ow = ETrue;  // /o or -o is OK
            else if(aLine[i] == 'c' || aLine[i] == 'C')
                aParam.c = status;
            else if(aLine[i] == 'a' || aLine[i] == 'A')
                aParam.a = status;
            else if(aLine[i] == 'e' || aLine[i] == 'E')
                aParam.e = status;
            else if(aLine[i] == 'w' || aLine[i] == 'W')
                aParam.w = status;
            else if(aLine[i] == 't' || aLine[i] == 'T')
                aParam.t = status;
        }
        else    //规定参数紧跟命令后
            return i;
    }

    return 0;
}
//=================================================================================
TBool IsDir(const TDesC &aPath)
{
    TParse iParse;
    iFs.Parse(aPath, iParse);

    if (iParse.IsWild())    //有通配符的当文件处理
        return EFalse;

    if (aPath[aPath.Length() - 1] == '\\')
        return ETrue;

    TEntry entry;
    if (iFs.Entry(aPath, entry) != KErrNone)
        return entry.IsDir();

    return EFalse;
}
//=================================================================================
TBool GetCMD(TDes &aLine, TDes &cmd)
{
    aLine.Trim();

    //empty line
    if (aLine.Length() == 0)
        return EFalse;

    //comment line
    if (aLine[0] == ';' || aLine[0] == '#' || aLine[0] == '[')
        return EFalse;

    TInt posSpace = aLine.Locate(' ');

    if (posSpace == KErrNotFound && aLine.Length() <= 24)   //
    {
        cmd.Copy(aLine);
        return ETrue;
    }

    //invalid command
    if (posSpace == KErrNotFound ||
        posSpace > 24 || posSpace == 0 ||
        posSpace == aLine.Length() - 1
    )
        return EFalse;

    cmd.Copy(aLine.Mid(0, posSpace));

    return ETrue;
}
//=================================================================================
void ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest/* = NULL*/, Parameter *aParam/* = NULL*/)
{
    //parse first
    if (aParam != NULL)
    {
        TInt len = GetParams(aCMD, *aParam);
        if (len > 0)
        {
            aCMD.Delete(0, len);
            aCMD.TrimLeft();
        }
    }

    //then source path
    TInt iLen = FindPath(aCMD, aSrc);
    if (iLen == 0)
        return ;

    if (aDest != NULL)
    {
//        if (aSrc.Length() < 0)
//            return ;
        //Important to +2 when the first path enclosed by quote
        if (aCMD[0] == '"')
            aCMD.Delete(0, /*aSrc.Length()*/iLen + 2);
        else
            aCMD.Delete(0, /*aSrc.Length()*/iLen);

        aCMD.TrimLeft();
        FindPath(aCMD, *aDest);
    }
}
//=================================================================================
TBool IsCmdDisabled()
{
/*    TFindFile findFile(iFs);
    //Search in drive from Y to A, and the last is Z
    if (KErrNone == findFile.FindByDir(KDisableFile, _L("\\")))
        return ETrue;

    return EFalse;
*/
    return !iConfig.IsMiniCMDEnabled();
}
//=================================================================================
TInt DoCommand(const TCommand &aCmd)
{
    switch(aCmd.GetCommand())
    {
    case TCommand::ERun:
        return DoRunApp(aCmd.GetSrc(), aCmd.GetParam().c == rm_it, aCmd.GetParam().w == rm_it);

    case TCommand::EKill:
        return DoKillApp(aCmd.GetSrc());

    case TCommand::ESleep:
        Sleep(DecStr2Int32(aCmd.GetSrc()));
        return KErrNone;

    case TCommand::ENone:
        return  KErrNotSupported;

    case TCommand::EKey:
        DoPressKey(aCmd);
        return KErrNone;

    case TCommand::ESendKey:
        return DoSendKey(aCmd);

    case TCommand::EFile:
        return DoFile(aCmd);

    case TCommand::ENote:
        if (aCmd.GetParam().c == rm_it)
            return Note(aCmd.GetSrc(), aCmd.GetDst());
        else
            return Note(aCmd.GetSrc());

    case TCommand::ELog:
        _LOG_(aCmd.GetSrc());
        return KErrNone;

    case TCommand::ECmd:
    {
        // to make log cmd first
        LogToFile(0, aCmd);
        TRAPD(iErr, DoLoadAndRunCmdL(aCmd.GetSrc()));
        return iErr;
    }

    case TCommand::EFind:
        DoFind(aCmd);
        return KErrNone;

    case TCommand::EInstall:
        return DoInstall(aCmd.GetParam().e == rm_it ? 'E' : 'C', aCmd.GetSrc());

    case TCommand::EUninstall:
        return DoUninstall(HexStr2Int32(aCmd.GetSrc()));

    case TCommand::ELogPs:
        return DoLogPs(aCmd.GetSrc());

    default:    //the rest turn to the server (possible need tcb)
        return DobyServer(aCmd);
    }
}
//=================================================================================
TBool IsPathFileExists(const TDesC &aPath)
{
    TEntry entry;

    if (iFs.Entry(aPath, entry)!= KErrNone)
        return EFalse;

    return ETrue;
}
//=================================================================================
void Run(const CArrayFixFlat<TCommand> &aCmdSet)
{
    for(TInt i=0; i<aCmdSet.Count(); i++)
    {
        if (iStop)
            break;
        // 这个'&'害死人了
        const TCommand &cmd = aCmdSet[i];

        TCommand::TCommandSet cs = cmd.GetCommand();

        if (cmd.AbortIfExists() || cmd.AbortIfNotExists())  //abort_if_(not_)exists
        {
            TBool fExists = IsCondition(cmd);
            if (cmd.AbortIfExists() && fExists)
                break;

            if (cmd.AbortIfNotExists() && !fExists)
                break;
        }
        else if (cmd.SkipIfExists() || cmd.SkipIfNotExists())   //skip_if_(not_)exists
        {
            TBool fExists = IsCondition(cmd);

            if ((cmd.SkipIfExists() && fExists) || (cmd.SkipIfNotExists() && !fExists))
            {
                i++;

                // if the next cmd is if(n)
                // then should skip this part
                if (i < aCmdSet.Count() && (
                    aCmdSet[i].GetCommand() == TCommand::EIf ||
                    aCmdSet[i].GetCommand() == TCommand::EIfn))
                {
                    for(; i<aCmdSet.Count(); i++)
                    {
                        if (aCmdSet[i].GetCommand() == TCommand::EEndIf)
                            break;
                    }
                }
            }

            continue;
        }
        else if(cmd.BackIfExists() || cmd.BackIfNotExists())    //back_if_(not_)exists
        {
            TBool fExists = IsCondition(cmd);

            if ((cmd.BackIfExists() && fExists) || (cmd.BackIfNotExists() && !fExists))
            {
                if (i > 0)  //should not be the first cmd,
                {
                    //i -= 2; // bug fixed Feb. 3, 2012
                    i --;

                    if (aCmdSet[i].GetCommand() == TCommand::EEndIf)
                    {
                        for(; i >= 0; i--)
                        {
                             if (aCmdSet[i].GetCommand() == TCommand::EIf ||
                                 aCmdSet[i].GetCommand() == TCommand::EIfn)
                             {
                                 //i--;
                                 break;
                             }
                        }
                    }

                    i--;
                }
            }
            continue;
        }
        else if (cmd.RestartIfExists() || cmd.RestartIfNotExists()) //restart_if_(not_)exists
        {
            TBool fExists = IsCondition(cmd);
            if ((cmd.RestartIfExists() && fExists) || (cmd.RestartIfNotExists() && !fExists))
            {
                if (i > 0)
                    i = -1;
            }
            continue;
        }
        else if(cs == TCommand::EIf || cs == TCommand::EIfn)
        {
            DoIF(aCmdSet, i, 0);
            continue;
        }
        else if(cs == TCommand::EAbort)
        {
            _LOG_(_L("[Abort]"));
            break;
        }
        else if (cs == TCommand::EStop)
        {
            _LOG_(_L("[Stop]"));
            iStop = ETrue;
            break;
        }

        iLastErr = DoCommand(cmd);

        // do not log cmd here
        if (miniLog && cmd.GetCommand() != TCommand::ECmd)
            LogToFile(iLastErr, cmd);
    }
}
//=================================================================================
void SkipCmd(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i)
{
    for(++i; i<aCmdSet.Count(); i++)
    {
        TCommand iCmd = aCmdSet.operator [](i);
        if (iCmd.GetCommand() == TCommand::EElse ||
            iCmd.GetCommand() == TCommand::EElseIf ||
            iCmd.GetCommand() == TCommand::EElseIfn ||
            iCmd.GetCommand() == TCommand::EEndIf)
        {
            i--;    //back to current found cmd
            break;
        }
    }
}
//=================================================================================
TInt RunApp(const TDesC &aName)
{
    RApaLsSession apaLsSession;
    TInt ret = apaLsSession.Connect();
    if (ret != KErrNone)
        return ret;

    CleanupClosePushL(apaLsSession);
    if(ret == KErrNone)
    {
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(aName);
        cmdLine->SetCommandL(EApaCommandRun);
        ret = apaLsSession.StartApp(*cmdLine);

        CleanupStack::PopAndDestroy(cmdLine);
    }

    CleanupStack::PopAndDestroy(&apaLsSession);

    return ret;
}
//=================================================================================
TInt RunApp(TInt aUid)
{
    RApaLsSession apaLsSession;
    TInt ret = apaLsSession.Connect();
    if (ret != KErrNone)
        return ret;

    CleanupClosePushL(apaLsSession);

    TApaAppInfo appInfo;
    TUid uid;
    uid.iUid = aUid;

    ret = apaLsSession.GetAppInfo(appInfo, uid);

    if(ret == KErrNone)
    {
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo.iFullName);
        cmdLine->SetCommandL(EApaCommandRun);
        ret = apaLsSession.StartApp(*cmdLine);

        CleanupStack::PopAndDestroy(cmdLine);
    }

    CleanupStack::PopAndDestroy(&apaLsSession);

    return ret;
}
//=================================================================================
TInt DoRunApp(const TDesC &aStr, TBool aIsConsole/* = EFalse*/, TBool aWaitForFinish/* = EFalse */)
{
    if (aIsConsole)
        return RunConsole(aStr, aWaitForFinish);

    if (IsHexString(aStr))
        return RunApp(HexStr2Int32(aStr));

    return RunApp(aStr);
}
//=================================================================================
TInt RunConsole(const TDesC &aName, TBool aWaitForFinish)
{
    RProcess process;

    TInt ret = process.Create(aName, KNullDesC);

    if (ret == KErrNone)
    {
        if (aWaitForFinish)
        {
            TRequestStatus istat;

            process.Logon(istat);
            process.Resume();
            User::WaitForRequest(istat);
        }
        else
        {
            process.Resume();
        }

        process.Close();
    }

    return ret;
}
//=================================================================================
TInt KillApp(TInt aUid)
{
    TInt ret = KErrNone;

    TFullName pName;
    TFindProcess finder(_L("*"));

    TUid tUid = {aUid};

    while((ret = finder.Next(pName)) == KErrNone)
    {
        if (pName == KNullDesC)
            break;

        RProcess process;
        ret = process.Open(pName);
        if (ret != KErrNone)
            return ret;

        if (tUid == process.Type()[2])
        {
            process.Kill(0);
            process.Close();
            break;
        }

        process.Close();
    }

    return ret;
}
//=================================================================================
TInt KillApp(const TDesC &aName)
{
    TInt ret = KErrNone;

    TFullName pName;
    pName.Copy(aName);
    pName.Append('*');

    TFindProcess finder(pName);

    while((ret = finder.Next(pName)) == KErrNone)
    {
        if (pName == KNullDesC)
            break;

        RProcess process;
        ret = process.Open(pName);
        if (ret != KErrNone)
            return ret;

        process.Kill(0);
        process.Close();
        break;
    }

    return ret;
}
//=================================================================================
TInt DoKillApp(const TDesC &aStr)
{
    if (IsHexString(aStr))
        return KillApp(HexStr2Int32(aStr));

    return KillApp(aStr);
}
//=================================================================================
void Sleep(TInt aMs)
{
    User::After(aMs * 1000);
}
//=================================================================================
TInt GetHex(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';

    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;

    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;

    return 0;
}
//=================================================================================
TInt32 HexStr2Int32(const TDesC & aHexStr)
{
    TInt len = aHexStr.Length();
    //防止溢出
    if(len > 10) return 0;

    TInt32 res = 0;
    TInt32 tmp = 0;
    const TUint16 * hexString = aHexStr.Ptr();

    TInt i = 0;

    if (aHexStr.Length() > 1 && aHexStr[0] == '0' &&
        (aHexStr[1] == 'x' || aHexStr[1] == 'X'))
        i = 2;

    for (; i < len; i++)
    {
        tmp = GetHex(hexString[i]);

        tmp <<= ((len-i-1)<<2);
        res |= tmp;
    }

    return res;
}
//=================================================================================
TInt32 DecStr2Int32(const TDesC &aDecStr)
{
    TInt len = aDecStr.Length();
    TInt i = 0;
    TReal res = 0;

    for (TInt j=len-1; j>=0; j--)
    {
        TReal pow = 0;
        Math::Pow10(pow, i++);

        res = (aDecStr[j] - '0') * pow + res;
    }

    return (TInt32)res;
}
//=================================================================================
TInt DobyServer(const TCommand &aCmd)
{
    TRequestCommand iReqstCmd(aCmd);

    iServer.RequestCommand(iReqstCmd);

    return iReqstCmd.iRet;
}
//=================================================================================
void PressKey(TInt aKey, TInt aTime/* = 50000*/)
{
    TRawEvent lEventDown;
    lEventDown.Set(TRawEvent::EKeyDown, aKey);
    UserSvr::AddEvent(lEventDown);

    User::After(aTime);

    TRawEvent lEventUp;
    lEventUp.Set(TRawEvent::EKeyUp, aKey);
    UserSvr::AddEvent(lEventUp);
}
//=================================================================================
void PressKey(const TDesC &aKey, TInt aTime/* = 50000*/)
{
    TInt len = aKey.Length();
    for(TInt i=0; i<len; i++)
        PressKey(aKey[i], aTime);
}
//=================================================================================
void DoPressKey(const TCommand &aCmd)
{
    if (aCmd.GetSrc().Length() == 0) //no key defined
        return ;

    TInt time = 50000;  //default is 50ms

    if (aCmd.GetDst().Length() > 0)
    {
        time = DecStr2Int32(aCmd.GetDst());

        if (time < 10)
            time = 10;

        time *= 1000;
    }

    if (aCmd.GetParam().c == rm_it) //use key code instead ascii
        PressKey(DecStr2Int32(aCmd.GetSrc()), time);
    else if (aCmd.GetParam().c == not_set)
        PressKey(aCmd.GetSrc(), time);
}
//=================================================================================
void SendKey(TApaTask &app, TInt aKey)
{
    TKeyEvent key = {0};

    key.iCode = aKey;
    key.iScanCode = aKey;

    app.SendKey(key);
}
//=================================================================================
TInt SendKey(TInt aUid, const TDesC &aKey)
{
    RWsSession wsSession;
    TInt ret = wsSession.Connect();
    if (ret != KErrNone)
        return ret;

    TApaTaskList tlist(wsSession);

    TUid uid;
    uid.iUid = aUid;

    TApaTask app(tlist.FindApp(uid));

    if (!app.Exists())
    {
        wsSession.Close();
        return KErrNotFound;
    }

    TInt len = aKey.Length();
    for(TInt i=0; i<len; i++)
        SendKey(app, aKey[i]);

    wsSession.Close();

    return KErrNone;
}
//=================================================================================
TInt SendKey(TInt aUid, TInt aKey)
{
    RWsSession wsSession;
    TInt ret = wsSession.Connect();
    if (ret != KErrNone)
        return ret;

    TApaTaskList tlist(wsSession);

    TUid uid;
    uid.iUid = aUid;

    TApaTask app(tlist.FindApp(uid));

    if (!app.Exists())
    {
        wsSession.Close();
        return KErrNotFound;
    }

    SendKey(app, aKey);

    wsSession.Close();

    return KErrNone;
}
//=================================================================================
TInt SendKey(const TDesC &aAppName, const TDesC &aKey)
{
    RWsSession wsSession;
    TInt ret = wsSession.Connect();
    if (ret != KErrNone)
        return ret;

    TApaTaskList tlist(wsSession);
    TApaTask app(tlist.FindApp(aAppName));

    if (!app.Exists())
    {
        wsSession.Close();
        return KErrNotFound;
    }

    TInt len = aKey.Length();
    for(TInt i=0; i<len; i++)
        SendKey(app, aKey[i]);

    wsSession.Close();

    return KErrNone;
}
//=================================================================================
TInt SendKey(const TDesC &aAppName, TInt aKey)
{
    RWsSession wsSession;
    TInt ret = wsSession.Connect();
    if (ret != KErrNone)
        return ret;

    TApaTaskList tlist(wsSession);
    TApaTask app(tlist.FindApp(aAppName));

    if (!app.Exists())
    {
        wsSession.Close();
        return KErrNotFound;
    }

    SendKey(app, aKey);

    wsSession.Close();

    return KErrNone;
}
//=================================================================================
TInt DoSendKey(const TCommand &aCmd)
{
    if (aCmd.GetSrc().Length() == 0 || aCmd.GetDst().Length() == 0)
        return KErrGeneral;

    if (IsHexString(aCmd.GetDst())) //UID
    {
        TBuf<16> iUid;
        iUid.Copy(aCmd.GetDst());
        iUid.Delete(0, 2);

        if (aCmd.GetParam().c == rm_it) //use key code instead ascii
            return SendKey(HexStr2Int32(iUid), DecStr2Int32(aCmd.GetSrc()));
        else if (aCmd.GetParam().c == not_set)
            return SendKey(HexStr2Int32(iUid), aCmd.GetSrc());
    }

    if (aCmd.GetParam().c == rm_it)
        return SendKey(aCmd.GetDst(), DecStr2Int32(aCmd.GetSrc()));
    else if (aCmd.GetParam().c == not_set)
        return SendKey(aCmd.GetDst(), aCmd.GetSrc());

    return KErrNone;
}
//=================================================================================
TInt OpenFile(const TDesC &aFileName)
{
    RApaLsSession apaLsSession;
    TInt ret = apaLsSession.Connect();
    if (ret != KErrNone)
        return ret;

    CleanupClosePushL(apaLsSession);

    TThreadId iThreadId;

    ret = apaLsSession.StartDocument(aFileName, iThreadId);

    CleanupStack::PopAndDestroy(&apaLsSession);

    return ret;
}
//=================================================================================
TInt DoFile(const TCommand &aCmd)
{
    if (aCmd.GetSrc().Length() == 0)
        return KErrGeneral;

    if (aCmd.GetParam().c == rm_it)
        return DobyServer(aCmd);    //due to when create file in sys\ need tcb cap
    else if(aCmd.GetParam().c == not_set)
        return  OpenFile(aCmd.GetSrc());

    return KErrGeneral;
}
//=================================================================================
TInt Note(const TDesC &aInfo, const TDesC &aTitle)
{
    CAknGlobalMsgQuery * pDlg = CAknGlobalMsgQuery::NewL();
    CleanupStack::PushL(pDlg);
    TRequestStatus iStatus;
    pDlg->ShowMsgQueryL(iStatus, aInfo, R_AVKON_SOFTKEYS_OK_CANCEL, aTitle,
        KNullDesC, 0, -1, CAknQueryDialog::ENoTone);

    User::WaitForRequest(iStatus);
    CleanupStack::PopAndDestroy(pDlg);

    return iStatus.Int() == EAknSoftkeyOk ? 0 : -1;
}
//=================================================================================
TInt Note(const TDesC &aInfo)
{
    CAknGlobalNote *note = CAknGlobalNote::NewLC();
    TRequestStatus iStatus;
    note->SetTone(1000);
    note->ShowNoteL(iStatus, EAknGlobalInformationNote, aInfo);
    User::WaitForRequest(iStatus);
    CleanupStack::PopAndDestroy(note);

    return KErrNone;
}
//=================================================================================
/*
TBool GetLogFileL(TFileName &aFile)
{
    TFileName cmdFile;

    if (IsPathFileExists(KDefCMDFileD))
        cmdFile.Copy(KDefCMDFileD);
    else if (IsPathFileExists(KDefCMDFileE))
        cmdFile.Copy(KDefCMDFileE);
    else if (IsPathFileExists(KDefCMDFileC))
        cmdFile.Copy(KDefCMDFileC);
    else if (IsPathFileExists(KDefCMDFileZ))
        cmdFile.Copy(KDefCMDFileZ);

    if (cmdFile.Length() == 0)
        return EFalse;

    RFile file;
    if (file.Open(iFs, cmdFile, EFileShareReadersOnly | EFileRead) != KErrNone)
        return EFalse;

    TFileText fileText;
    fileText.Set(file);

    HBufC *buffer = HBufC::NewLC(512);
    TPtr ptr = buffer->Des();

    if (fileText.Read(ptr) == KErrNone)
    {
        ptr.Trim();
        if (ptr.Length() > 6 && ptr[0] == '[' && ptr[ptr.Length()-1] == ']')
        {
            ptr.Copy(ptr.Mid(1, ptr.Length() - 2));
            ptr.Trim();

            ptr.LowerCase();

            TInt posEqual = ptr.Locate('=');
            if (KErrNotFound != posEqual)
            {
                cmdFile.Copy(ptr.Mid(0, posEqual));
                cmdFile.Trim();
                if (cmdFile == _L("log"))
                {
                    aFile.Copy(ptr.Mid(posEqual + 1));
                    aFile.Trim();

                    file.Close();
                    CleanupStack::PopAndDestroy(buffer);
                    return aFile.Length() > 0;
                }
                else if(cmdFile == _L("log-all"))
                {
                    iLogAll = ETrue;

                    aFile.Copy(ptr.Mid(posEqual + 1));
                    aFile.Trim();

                    file.Close();
                    CleanupStack::PopAndDestroy(buffer);
                    return aFile.Length() > 0;
                }
            }
        }
    }

    file.Close();
    CleanupStack::PopAndDestroy(buffer);
    return EFalse;
}
*/
//=================================================================================
void LogToFile(TInt aErrCode, const TCommand &aCmd)
{
    if (!iLogAll && aErrCode == KErrNone)
        return ;

    HBufC *logBuf = HBufC::NewLC(512);
    TPtr des = logBuf->Des();
    des.Append('[');

    switch(aCmd.GetCommand())
    {
    case TCommand::EAtt:        des.Append(KCmdAtt);        break;
    case TCommand::ECp:         des.Append(KCmdCp);         break;
    case TCommand::EMv:         des.Append(KCmdMv);         break;
    case TCommand::EMd:         des.Append(KCmdMd);         break;
    case TCommand::ERm:         des.Append(KCmdRm);         break;
    case TCommand::ERn:         des.Append(KCmdRn);         break;
    case TCommand::EDel:        des.Append(KCmdDel);        break;
    case TCommand::ERun:        des.Append(KCmdRun);        break;
    case TCommand::EKill:       des.Append(KCmdKill);       break;
    case TCommand::ESleep:      des.Append(KCmdSleep);      break;
    case TCommand::EKey:        des.Append(KCmdKey);        break;
    case TCommand::ESendKey:    des.Append(KCmdSendKey);    break;
    case TCommand::EFile:       des.Append(KCmdFile);       break;
    case TCommand::ENote:       des.Append(KCmdNote);       break;
    case TCommand::ELog:        des.Append(KCmdLog);        break;
    case TCommand::ECmd:        des.Append(KCmdCmd);        break;
    case TCommand::EFind:       des.Append(KCmdFind);       break;
    case TCommand::EInstall:    des.Append(KCmdInstall);    break;
    case TCommand::EUninstall:  des.Append(KCmdUninstall);  break;
    case TCommand::ELogPs:		des.Append(KCmdLogPs);		break;

    default:                    des.Append(_L("Not defined"));
    }

    if (aCmd.GetSrc().Length() > 0)
    {
        des.Append(' ');
        des.Append('"');
        des.Append(aCmd.GetSrc());
        des.Append('"');
    }

    if (aCmd.GetDst().Length() > 0)
    {
        des.Append(' ');
        des.Append('"');
        des.Append(aCmd.GetDst());
        des.Append('"');
    }

    des.Append(KLogFmt);

    miniLog->Log(des, aErrCode);

    CleanupStack::PopAndDestroy(logBuf);
}
//=================================================================================
TBool IsAppRunning(const TDesC &aAppName)
{
    TFullName pName;
    pName.Copy(aAppName);
    pName.Append('*');

    TFindProcess finder(pName);

    while(finder.Next(pName) == KErrNone)
    {
        if (pName == KNullDesC)
            break;

        return ETrue;
    }

    return EFalse;
}
//=================================================================================
TBool IsAppRunning(TInt aUid)
{
/* cause no use during startup time
    RWsSession wsSession;
    wsSession.Connect();

    TApaTaskList taskList(wsSession);

    TUid iUid = {aUid};
    TApaTask apaTask = taskList.FindApp(iUid);

    wsSession.Close();

    return apaTask.Exists();
 */

    TFullName pName;
    TFindProcess finder(_L("*"));

    TUid tUid = {aUid};

    while(finder.Next(pName) == KErrNone)
    {
        if (pName == KNullDesC)
            return EFalse;

        RProcess process;
        if (process.Open(pName) != KErrNone)
            return EFalse;

        if (tUid == process.Type()[2])
        {
            process.Close();

            return ETrue;
        }

        process.Close();
    }

    return EFalse;
}
//=================================================================================
TBool TestAppRunning(const TDesC &aApp)
{
    if (IsHexString(aApp))
        return IsAppRunning(HexStr2Int32(aApp));

    return IsAppRunning(aApp);

}
//=================================================================================
TBool IsHexString(const TDesC &aStr)
{
    return (aStr.Length() > 2 && aStr[0] == '0'
        && (aStr[1] == 'x' || aStr[1] == 'X'));
}
//=================================================================================
TBool IsCondition(const TCommand &aCmd)
{
    TBool fExists = EFalse;
    if (aCmd.GetParam().a == rm_it)
        fExists = TestAppRunning(aCmd.GetSrc());
    else if(aCmd.GetParam().e == rm_it)
        return iLastErr != KErrNone;
    else if(aCmd.GetParam().s == rm_it)
        return FileSize(aCmd.GetSrc()) == FileSize(aCmd.GetDst());
    else if(aCmd.GetParam().t == rm_it)
        return CmpCurrentTime(aCmd.GetSrc());
    else
        fExists = IsPathFileExists(aCmd.GetSrc());

    return fExists;
}
//=================================================================================
void DoLoadAndRunCmdL(const TDesC &aFileName)
{
    CArrayFixFlat<TCommand> *iCmdSet = new(ELeave) CArrayFixFlat<TCommand>(5);
    CleanupStack::PushL(iCmdSet);

    if (LoadCmdFileL(aFileName, iCmdSet) == KErrNone)
    {
        Run(*iCmdSet);
    }

    CleanupStack::PopAndDestroy(iCmdSet);
}
///////////////////////////////////////////////////////////////////////////////////////////////
TInt FileSize(const TDesC &aFileName)
{
    TEntry entry;
    TInt err = iFs.Entry(aFileName, entry);
    if (err == KErrNone)
        return entry.iSize;
    return err;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void DoFind(const TCommand &aCmd)
{
    TBool isLog = (aCmd.GetDst() == _L("log"));
    TBool iRecursive = aCmd.GetParam().is;
    // nothing to do with log operation
    if (isLog && NULL == miniLog)
        return ;

    CDesCArray *dirs = new (ELeave)CDesCArrayFlat(3);
    if (NULL == dirs)
        return ;

    CleanupStack::PushL(dirs);
    HBufC *iBuf = HBufC::NewLC(128);
    if (NULL == iBuf)
    {
        CleanupStack::PopAndDestroy(dirs);
        return ;
    }
    TPtr targetPtr = iBuf->Des();
    TInt i = 0;

    while(i < aCmd.GetSrc().Length())
    {
        if (aCmd.GetSrc()[i] == ';')
        {
            dirs->AppendL(targetPtr);
            targetPtr.Delete(0, targetPtr.Length());
        }
        else if(aCmd.GetSrc()[i] == '|') //
        {
            if (targetPtr.Length() > 0)
                dirs->AppendL(targetPtr);

            i++; // skip '|'
            targetPtr.Delete(0, targetPtr.Length());
            while(i < aCmd.GetSrc().Length())
                targetPtr.Append(aCmd.GetSrc()[i++]);
        }
        else
        {
            targetPtr.Append(aCmd.GetSrc()[i]);
        }
        i++;
    }

    for(i=0; i<dirs->Count(); i++)
    {
        if (isLog)
            FindFile(dirs->operator[](i), targetPtr, iRecursive, LogFile);
        else
            FindFile(dirs->operator[](i), targetPtr, iRecursive, DelFile);
    }


    CleanupStack::PopAndDestroy(iBuf);
    CleanupStack::PopAndDestroy(dirs);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void FindFile(const TDesC &aDir, const TDesC &aFile, TBool aRecursive, void (*OpFunc)(const TDesC&))
{
    if (NULL == OpFunc)
        return ;

    CDirScan* ds = CDirScan::NewLC(iFs);
    if (NULL == ds)
        return ;
    ds->SetScanDataL(aDir, KEntryAttDir, ESortByName);

    while (ETrue)
    {
        CDir* dir = NULL;
        TRAPD(error, ds->NextL(dir));
        if (KErrNone != error || NULL == dir)
            break;

        FindFileReal(ds->FullPath(), aFile, OpFunc);

        delete dir;

        if (!aRecursive)
            break;
    }

    CleanupStack::PopAndDestroy(ds);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void FindFileReal(const TDesC &aDir, const TDesC &aFile, void (*OpFunc)(const TDesC&))
{
    TFindFile findFile(iFs);
    CDir *dir = NULL;
    /*TInt err = */findFile.FindWildByDir(aFile, aDir, dir);
    if (NULL == dir)
        return ;

    for (TInt i = 0; i < dir->Count(); i++)
    {
        TEntry entry = (*dir)[i];

        TBuf<256> filePath;
        filePath.Append(aDir);
        filePath.Append(entry.iName);

        OpFunc(filePath);
    }

    delete dir;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void LogFile(const TDesC &aFile)
{
    _LOG_(aFile);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void DelFile(const TDesC &aFile)
{
    TCommand iCmd(TCommand::ERm, NULL, &aFile);

    DobyServer(iCmd);
}
///////////////////////////////////////////////////////////////////////////////////////////////
TInt DoInstall(TChar aDrive, const TDesC &aPath)
{
    SwiUI::RSWInstSilentLauncher iLauncher;
    SwiUI::TInstallOptions iOptions;
    SwiUI::TInstallOptionsPckg iOptionsPckg;

    User::LeaveIfError(iLauncher.Connect());

    iOptions.iUpgrade = SwiUI::EPolicyAllowed;
    iOptions.iOCSP = SwiUI::EPolicyNotAllowed;
    iOptions.iDrive = aDrive;
    iOptions.iUntrusted = SwiUI::EPolicyAllowed;
    iOptions.iCapabilities = SwiUI::EPolicyAllowed;
    iOptions.iOverwrite = SwiUI::EPolicyAllowed;
    iOptions.iDownload = SwiUI::EPolicyAllowed;

    iOptionsPckg = iOptions;

/*    TEntry entry;*/
    TInt ret = KErrNone;

/*    if (iFs.Entry(aPath, entry) != KErrNone && entry.IsDir())
    {
        TFindFile findFile(iFs);
        CDir *dir = NULL;
        findFile.FindWildByDir(_L("*.sis*"), aPath, dir);
        if (NULL == dir)
        {
            iLauncher.Close();
            return KErrNotFound;
        }

        for (TInt i = 0; i < dir->Count(); i++)
        {
            TEntry entry = (*dir)[i];

            TBuf<256> filePath;
            filePath.Append(aPath);
            filePath.Append(entry.iName);

            ret = iLauncher.SilentInstall(filePath, iOptionsPckg);
        }

        delete dir;
    }
    else*/
    {
        ret = iLauncher.SilentInstall(aPath, iOptionsPckg);
    }

    iLauncher.Close();

    return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////
TInt DoUninstall(TInt aUid)
{
    TUid iUid = { aUid };

    SwiUI::RSWInstSilentLauncher iLauncher;

    User::LeaveIfError(iLauncher.Connect());

    SwiUI::TUninstallOptions options;
    options.iBreakDependency = SwiUI::EPolicyAllowed;
    options.iKillApp = SwiUI::EPolicyAllowed;
    SwiUI::TUninstallOptionsPckg optPckg(options);

    TInt ret = iLauncher.SilentUninstall(iUid, optPckg, SwiUI::KSisxMimeType());

    iLauncher.Close();

    return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////
TBool CmpCurrentTime(const TDesC &aTimeStr)
{
    TTime curTime;
    curTime.HomeTime();
    TDateTime dt = curTime.DateTime();
    TInt32 time = dt.Hour() * 60 * 60 + dt.Minute() * 60 + dt.Second();

    return time == TimeStr2Int32(aTimeStr);
}
///////////////////////////////////////////////////////////////////////////////////////////////
TInt32 TimeStr2Int32(const TDesC &aTimeStr)
{
    TInt32 time = 0;
    TInt i = 0;

    TBuf<10> temp;

    // hour
    while (i < aTimeStr.Length() && aTimeStr[i] != ':') {
       temp.Append(aTimeStr[i++]);
    }
    temp.Trim();

    time += DecStr2Int32(temp) * 60 * 60;

    temp.Copy(_L(""));
    i++;
    // minute
    while (i < aTimeStr.Length() && aTimeStr[i] != ':') {
        temp.Append(aTimeStr[i++]);
    }
    temp.Trim();

    time += DecStr2Int32(temp) * 60;

    temp.Copy(_L(""));
    i++;

    // sec
    while (i < aTimeStr.Length() && aTimeStr[i] != ' ') {
        temp.Append(aTimeStr[i++]);
    }

    temp.Trim();

    time += DecStr2Int32(temp);

    return time;
}
///////////////////////////////////////////////////////////////////////////////////////////////
TInt DoLogPs(const TDesC &aLogFile)
{
    CMiniLog *iLog = NULL;

    if (aLogFile.Length() == 0)	// Use default log file
    {
        iLog = miniLog;
    }
    else
    {
        TRAPD(iErr, iLog = CMiniLog::NewL(iFs, aLogFile));
        if (iErr != KErrNone || !IsPathFileExists(aLogFile))
        {
            if (iLog != NULL)
                delete iLog;

            return iErr;
        }
    }

    if (iLog == NULL)
        return KErrNotFound;

    TFullName pName;
    pName.Copy(_L("*"));

    TFindProcess finder(pName);

    iLog->Log(_L("[Begin LogPs]"));
    while(finder.Next(pName) == KErrNone)
    {
        if (pName == KNullDesC)
            break;

        iLog->Log(pName);
    }

    iLog->Log(_L("[End LogPs]\n"));

    if (aLogFile.Length() > 0)
        delete iLog;

    return KErrNone;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void DoIF(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i, TInt aLevel)
{
    TBool lastCondition = EFalse;
    TBool fromTop = ETrue;
    TInt curLevel = aLevel;

    for(; i<aCmdSet.Length(); i++)
    {
        const TCommand &cmd = aCmdSet[i];
        TCommand::TCommandSet cs = cmd.GetCommand();

        if (cs == TCommand::EIf || cs == TCommand::EIfn)
        {
            if (fromTop)
            {
                fromTop = EFalse;
                TBool fExists = IsCondition(cmd);

                if ((cs == TCommand::EIf && fExists) || (cs == TCommand::EIfn && !fExists))
                {
                    lastCondition = ETrue;
                    continue;
                }

                SkipIfBlock(aCmdSet, i, aLevel);

                lastCondition = EFalse;
                SkipCmd(aCmdSet, i);
            }
            else
            {
                curLevel++;
                //_LOG_FMT_(_L("DoIF(%d)->>>"), aLevel);
                DoIF(aCmdSet, i, curLevel);
                if (i<aCmdSet.Length() && aCmdSet[i].GetCommand() == TCommand::EEndIf)
                    curLevel--;
                //_LOG_FMT_(_L("<<<-DoIF(%d)"), aLevel);
            }

            continue;
        }
        else if (cs == TCommand::EElseIf || cs == TCommand::EElseIfn )
        {
            if (!lastCondition)
            {
                TBool fExists = IsCondition(cmd);
                if ((cs == TCommand::EElseIf && fExists) ||
                    (cs == TCommand::EElseIfn && !fExists))
                {
                    lastCondition = ETrue;
                    continue;
                }
                else
                    lastCondition = EFalse;
            }

            SkipIfBlock(aCmdSet, i, aLevel);

            SkipCmd(aCmdSet, i);

            continue;
        }
        else if (cs == TCommand::EElse)
        {
            if (!lastCondition)
            {
                continue;
            }

            SkipIfBlock(aCmdSet, i, aLevel);
            SkipCmd(aCmdSet, i);
            continue;
        }
        else if (cs == TCommand::EEndIf)
        {
            // _LOG_FMT_(_L("ENDIF (%d)"), i);
            if (curLevel == aLevel)
                return ;
            curLevel--;

            continue;
        }

        iLastErr = DoCommand(cmd);

        if (miniLog && cmd.GetCommand() != TCommand::ECmd)
            LogToFile(iLastErr, cmd);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
void SkipIfBlock(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i, TInt aLevel)
{
    TInt curLevel = aLevel;

    for( ++i; i<aCmdSet.Count(); i++)
    {
        TCommand::TCommandSet cs = aCmdSet[i].GetCommand();
        if (cs == TCommand::EElseIf || cs == TCommand::EElseIfn ||
            cs == TCommand::EElse)
        {
            if (curLevel == aLevel)
            {
                --i;
                break;
            }
        }
        else if (cs == TCommand::EIf || cs == TCommand::EIfn)
        {
            curLevel++;
        }
        else if (cs == TCommand::EEndIf)
        {
            if (curLevel == aLevel)
            {
                --i;
                break;
            }

            curLevel--;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////////////////
