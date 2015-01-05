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

#include "Config.h"

Config::Config()
{
    iEnable = ETrue;
    iLogLevel = 0;
}

Config::~Config()
{
}

TBool Config::LoadL(RFs &aFs, const TDesC &aFileName)
{
    RFile iFile;
    TInt ret;

    ret = iFile.Open(aFs, aFileName, EFileShareReadersOnly | EFileRead);
    if (KErrNone != ret)
        return EFalse;
      
    HBufC* heapBuf = HBufC::NewLC(256);
    TPtr lineBuf = heapBuf->Des();
    
    TFileText fileText;
    fileText.Set(iFile);
    
    TInt section = 0;
    
    while ((ret = fileText.Read(lineBuf)) != KErrEof)
    {
        if (ret != KErrNone)
            break;
        
        section = ParseLineL(section, lineBuf);
    }
    
    CleanupStack::PopAndDestroy(heapBuf);
    iFile.Close();
    
    return ETrue;
}

TInt Config::ParseLineL(TInt aSection, TDes &aLine)
{
    aLine.Trim();
    
    if (0 == aLine.Length() || ';' == aLine[0])
        return aSection;

    if (aLine.Length() > 2 && '[' == aLine[0] && ']' == aLine[aLine.Length() - 1])
    {
        aLine.Copy(aLine.Mid(1, aLine.Length() - 2));
        aLine.Trim();
        return ParseSection(aLine);
    }

    TInt pos = aLine.Locate('=');
    
    if (KErrNotFound != pos)
    {
        TBuf<30> key;
        TBuf<200> value;
        
        key.Copy(aLine.Mid(0, pos));
        value.Copy(aLine.Mid(pos + 1));
        
        key.Trim();
        value.Trim();
        
        ParseKeyValueL(aSection, key, value);
    }
    
    return aSection;
}

TInt Config::ParseKeyValueL(TInt aSection, const TDesC &aKey, const TDesC &aValue)
{
    switch(aSection)
    {
    case EGNull:
        return KErrNotSupported;
        
    case EGMiniCMD:
    {
        if (aKey.CompareF(_L("Enable")) == 0)
        {
            if (!GetBool(aValue, iEnable))
                return KErrArgument;
        }
        
        else if(aKey.CompareF(_L("Drive")) == 0)
        {
            iDrive = aValue;
            iDrive.Trim();
        }
        
        else if(aKey.CompareF(_L("CMDFile")) == 0)
        {
            iCmdFile = aValue;
            iCmdFile.Trim();
        }
        
        else if(aKey.CompareF(_L("LogLevel")) == 0)
        {
            if (!GetInt(aValue, iLogLevel))
                return KErrArgument;
        }
        
        else if(aKey.CompareF(_L("LogFile")) == 0)
        {
            iLogFile = aValue;
            iLogFile.Trim();
        }
        else if (aKey.CompareF(_L("CMDFileC")) == 0)
		{
			iCmdFileC = aValue;
			iCmdFileC.Trim();
		}
		else if (aKey.CompareF(_L("CMDFileD")) == 0)
		{
			iCmdFileD = aValue;
			iCmdFileD.Trim();
		}
		else if (aKey.CompareF(_L("CMDFileE")) == 0)
		{
			iCmdFileE = aValue;
			iCmdFileE.Trim();
		}
		else if (aKey.CompareF(_L("CMDFileY")) == 0)
		{
			iCmdFileY = aValue;
			iCmdFileY.Trim();
		}
		else if (aKey.CompareF(_L("CMDFileZ")) == 0)
		{
			iCmdFileZ = aValue;
			iCmdFileZ.Trim();
		}
    }
        break;
    }
    
    return KErrNone;
}

TInt Config::ParseSection(const TDesC &aSectionName)
{
    if (aSectionName.CompareF(_L("MiniCMD")) == 0)
        return EGMiniCMD;
    
    return EGNull;
}

TInt Config::GetBool(const TDesC &aStr, TBool &aBool)
{
    if (1 == aStr.Length())
    {
        if ('0' == aStr[0])
            aBool = EFalse;
        else if('1' == aStr[0])
            aBool = ETrue;
        else
            return EFalse;
        
        return ETrue;
    }
    
    return EFalse;
}

TInt Config::GetInt(const TDesC &aStr, TInt &aInt)
{
    if (0 == aStr.Length())
        return EFalse;

    TLex lex(aStr);
    
    if (KErrNone != lex.Val(aInt))
        return EFalse;
    
    return ETrue;
}
