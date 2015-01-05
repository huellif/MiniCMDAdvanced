/*
 ============================================================================
 Name		    : MiniCMD
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
#ifndef __MINI_CMD_H__
#define __MINI_CMD_H__
//=================================================================================
#include <APGTASK.H>

#include "TCommand.h"
//=================================================================================
TInt    LoadCmdFileL(const TDesC &aFileName, CArrayFixFlat<TCommand> *aCmdSet);
void    ParseLineL(TDes &aLine, CArrayFixFlat<TCommand> *aCmdSet);
TInt    FindPath(TDes &aLine, TDes &aPath);
TInt    GetParams(TDes &aLine, Parameter &aParam);
TBool   GetCMD(TDes &aLine, TDes &cmd);
void    ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest = NULL, Parameter *aParam = NULL);

TInt    DoCommand(const TCommand &aCmd);

TBool   IsCmdDisabled();    //test if usr don't want to run minicmd

TBool   IsPathFileExists(const TDesC &aPath);

void    Run(const CArrayFixFlat<TCommand> &aCmdSet);
inline  void SkipCmd(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i);

TInt    RunApp(const TDesC &aName);
TInt    RunApp(TInt aUid);
TInt    DoRunApp(const TDesC &aStr, TBool aIsConsole = EFalse, TBool aWaitForFinish = EFalse);
TInt    RunConsole(const TDesC &aName, TBool aWaitForFinish);

TInt    KillApp(TInt aUid);
TInt    KillApp(const TDesC &aName);
TInt    DoKillApp(const TDesC &aStr);

inline  void Sleep(TInt aMs);

TInt32  HexStr2Int32(const TDesC & aHexStr);
TInt32  DecStr2Int32(const TDesC &aDecStr);

TInt    DobyServer(const TCommand &aCmd);

void    PressKey(TInt aKey, TInt aTime = 50000);
void    PressKey(const TDesC &aKey, TInt aTime = 50000);
void    DoPressKey(const TCommand &aCmd);

void    SendKey(TApaTask &app, TInt aKey);
TInt    SendKey(TInt aUid, const TDesC &aKey);
TInt    SendKey(TInt aUid, TInt aKey);
TInt    SendKey(const TDesC &aAppName, const TDesC &aKey);
TInt    SendKey(const TDesC &aAppName, TInt aKey);

TInt    DoSendKey(const TCommand &aCmd);

TInt    OpenFile(const TDesC &aFileName);
TInt    DoFile(const TCommand &aCmd);

TInt    Note(const TDesC &aInfo, const TDesC &aTitle);  //need user confirm
TInt    Note(const TDesC &aInfo);   //just a popup info

/*
TBool   GetLogFileL(TFileName &aFile);
*/
void    LogToFile(TInt aErrCode, const TCommand &aCmd);

TBool   IsAppRunning(const TDesC &aAppName);
TBool   IsAppRunning(TInt aUid);
TBool   TestAppRunning(const TDesC &aApp);

inline  TBool IsHexString(const TDesC &aStr); //len >= 3 && begin with '0x' or '0X'

inline  TBool IsCondition(const TCommand &aCmd);

void    DoLoadAndRunCmdL(const TDesC &aFileName);

TInt    FileSize(const TDesC &aFileName);

void    DoFind(const TCommand &aCmd);
void    FindFile(const TDesC &aDir, const TDesC &aFile, TBool aRecursive, void (*OpFunc)(const TDesC&));
void    FindFileReal(const TDesC &aDir, const TDesC &aFile, void (*OpFunc)(const TDesC&));
void    LogFile(const TDesC &aFile);
void    DelFile(const TDesC &aFile);

TInt    DoInstall(TChar aDrive, const TDesC &aPath);
TInt    DoUninstall(TInt aUid);

TBool   CmpCurrentTime(const TDesC &aTimeStr);
TInt32  TimeStr2Int32(const TDesC &aTimeStr);

TInt	DoLogPs(const TDesC &aLogFile);

void	DoIF(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i, TInt aLevel);
void 	SkipIfBlock(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i, TInt aLevel);
//=================================================================================
#endif  // __MINI_CMD_H__
//=================================================================================
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
