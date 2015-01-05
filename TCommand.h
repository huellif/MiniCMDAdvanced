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
 * TCommand.h
 *
 *  Created on: 2011-5-27
 *      Author: Tim
 */

#ifndef __TCOMMAND_H__
#define __TCOMMAND_H__

#include <e32base.h>

#include "Parameter.h"

class TCommand
{
public:
    enum TCommandSet
    {
        ENone = -1, //no command
        EAtt = 0,   //attribute
        ECp,        //copy
        EMv,        //move
        EMd,        //make dir
        ERn,        //rename
        ERm,        //remove
        EDel,       //delete (file only)
        ERun,       //run app
        EKill,      //kill app
        ESleep,
        EAbort,     
        EAbes,      //abort if exists
        EAbnes,     //abort if not exists
        ESkes,      //skip if exists
        ESknes,     //skip if not exists
        EBkes,      //back to last cmd if exists
        EBknes,     //back to last cmd if not exists
        ERses,      //restart if exists
        ERsnes,     //restart if not exists
        EIf,
        EIfn,
        EElse,
        EElseIf,
        EElseIfn,
        EEndIf,
        EKey,
        ESendKey,
        EFile,
        ENote,
        ELog,
        ECmd,
        EStop,
        EFind,
        EInstall,
        EUninstall,
        ELogPs
    };
    
    TCommand() : iCmd(ENone){}
    TCommand(TCommandSet aCmd, const Parameter *aParam, const TDesC *aSrc, const TDesC *aDst = NULL){
        SetCommand(aCmd, aParam, aSrc, aDst);
    }
    virtual ~TCommand(){}
public:
    void SetCommand(TCommandSet aCmd, const Parameter *aParam, const TDesC *aSrc, const TDesC *aDst = NULL)
    {
        iCmd = aCmd;

        if (aParam){
            iParam = *aParam;
        }

        if (aSrc && aSrc->Length() > 0){
            iSrc.Copy(*aSrc);
        }

        if (aDst && aDst->Length() > 0){
            iDst.Copy(*aDst);
        }
    }
    
    TCommandSet GetCommand()        const   {    return iCmd;    }
    const Parameter & GetParam()    const   {    return iParam;  }
    
    const TDesC & GetSrc() const        {    return iSrc;    }
    const TDesC & GetDst() const        {    return iDst;    }
    
    TBool AbortIfExists()       const   {   return iCmd == EAbes;   }
    TBool AbortIfNotExists()    const   {   return iCmd == EAbnes;  }
    TBool SkipIfExists()        const   {   return iCmd == ESkes;   }
    TBool SkipIfNotExists()     const   {   return iCmd == ESknes;  }
    TBool BackIfExists()        const   {   return iCmd == EBkes;   }
    TBool BackIfNotExists()     const   {   return iCmd == EBknes;  }
    TBool RestartIfExists()     const   {   return iCmd == ERses;   }
    TBool RestartIfNotExists()  const   {   return iCmd == ERsnes;  }

protected:
    
    TCommandSet iCmd;   //what command
    Parameter   iParam; //
    TFileName   iSrc;   //source path
    TFileName   iDst;   //destination path
};

#endif /* __TCOMMAND_H__ */
