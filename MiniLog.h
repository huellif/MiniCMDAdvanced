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
 * MiniLog.h
 *
 *  Created on: 2011-5-12
 *      Author: Just Fancy
 */

#ifndef __MINI_LOG_H__
#define __MINI_LOG_H__

#include <f32file.h>
#include <S32FILE.H>

class CMiniLog : public CBase
{
public:
    virtual ~CMiniLog();
    
    static CMiniLog* NewL(RFs &aFs, const TDesC &aFile);
    static CMiniLog* NewLC(RFs &aFs, const TDesC &aFile);
    
    void Log(TRefByValue<const TDesC16> aFmt, ...);
private:
    void ConstructL(RFs &aFs, const TDesC &aFile);
    void GetTimeStr(TDes &aBuf);
protected:
    RFile iFile;
    RFileWriteStream iOutputStream;
    RFs *iFs;
};

#endif /* __MINI_LOG_H__ */
