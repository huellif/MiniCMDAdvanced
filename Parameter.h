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
#ifndef __PARAMETER_H__
#define	__PARAMETER_H__

#include <e32base.h>

enum AttStatus
{
    not_set = 0,    //not define
    add_to,         //+
    rm_it           //-
};

struct Parameter
{
    AttStatus s;    //sys attribute
    AttStatus h;    //hidden
    AttStatus r;    //read only
    
    TBool is;       //include subfolder?
    TBool ow;       //overwrite exists file?
    
    AttStatus c;    //use keycode, for key && longkey / for file, create
   
    AttStatus a;    //for if/else ::application
    
    AttStatus e;    //last error status
    
    AttStatus w;    // wait for process finish
    
    AttStatus t;    // time string
    
    Parameter()
    {
        s = h = r = c = a = e = w = t = not_set;
        is = ow = EFalse;
    }
    
    TBool NeedToSetAtt() const{
        return s + h + r > 0;
    }
    
    Parameter & operator = (const Parameter &other)
    {
        this->s = other.s;
        this->h = other.h;
        this->r = other.r;
        this->is = other.is;
        this->ow = other.ow;
        this->c = other.c;
        this->a = other.a;
        this->e = other.e;
        this->w = other.w;
        this->t = other.t;
        
        return *this;
    }
};

#endif  /* __PARAMETER_H__ */
