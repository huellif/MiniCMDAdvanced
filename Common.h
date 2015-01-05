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
#ifndef __COMMON_H__
#define __COMMON_H__

#include <e32base.h>

_LIT(KMCServerName,"SyncMCServer" );
_LIT(KMCServerSemaphoreName, "MCServerSemaphore" );

const TUint KMCServMajorVersionNumber = 2;
const TUint KMCServMinorVersionNumber = 1;
const TUint KMCServBuildVersionNumber = 1;

enum TMCServRqst
{
    EMCServRequestOp,
    EMCServRequestComplete
};

#endif // __COMMON_H__
