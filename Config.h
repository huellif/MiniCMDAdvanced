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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <e32std.h>
#include <f32file.h>

class Config
{
public:
    Config();
    virtual ~Config();
    
    TBool   LoadL(RFs &aFs, const TDesC &aFileName);
    
    inline TBool        IsMiniCMDEnabled()  const   {   return iEnable;     }
    inline const TDesC& GetDriveLetters()   const   {   return iDrive;      }
    inline const TDesC& GetCMDFile()        const   {   return iCmdFile;    }
    inline TInt         GetLogLevel()       const   {   return iLogLevel;   }
    inline const TDesC& GetLogFile()        const   {   return iLogFile;    }
    
    inline const TDesC& GetCmdFileX(int x)	const
    {	
    	if(x == 'C')		return iCmdFileC;
		else if(x == 'D')	return iCmdFileD;
		else if(x == 'E')	return iCmdFileE;
		else if(x == 'Y')	return iCmdFileY;
		else if(x == 'Z')	return iCmdFileZ;
    	
    	return _L("");
    }
    
    enum TGroup
    {
        EGNull,
        EGMiniCMD
    };
protected:
    TInt    ParseLineL(TInt aSection, TDes &aLine);
    TInt    ParseKeyValueL(TInt aSection, const TDesC &aKey, const TDesC &aValue);
    TInt    ParseSection(const TDesC &aSectionName);
    
    TInt    GetBool(const TDesC &aStr, TBool &aBool);
    TInt    GetInt(const TDesC &aStr, TInt &aInt);

protected:
    TBool       iEnable;
    TBuf<10>    iDrive;
    TFileName   iCmdFile;
    TInt        iLogLevel;
    TFileName   iLogFile;
    
    TFileName	iCmdFileC;
    TFileName	iCmdFileD;
    TFileName	iCmdFileE;
    TFileName	iCmdFileY;
    TFileName	iCmdFileZ;
};

#endif /* __CONFIG_H__ */
