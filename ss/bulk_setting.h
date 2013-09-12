// Straightforward Settings Library
//
// Copyright 2007 John Torjo (john@macadamian.com)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
//
// Find latest version of this at http://www.macadamian.ro/drdobbs/

#ifndef SS_BULK_SETTING_H
#define SS_BULK_SETTING_H

#pragma once

#include <string>

namespace ss { 

/** 
    Does a bulk setting - up to 40 args.

    Each arg must have the following syntax:
    name=value

    You are not allowed to have any comments
*/
void bulk_setting(
        const string & s00 = string() ,
        const string & s01 = string(),
        const string & s02 = string(),
        const string & s03 = string(),
        const string & s04 = string(),
        const string & s05 = string(),
        const string & s06 = string(),
        const string & s07 = string(),
        const string & s08 = string(),
        const string & s09 = string(),

        const string & s10 = string(),
        const string & s11 = string(),
        const string & s12 = string(),
        const string & s13 = string(),
        const string & s14 = string(),
        const string & s15 = string(),
        const string & s16 = string(),
        const string & s17 = string(),
        const string & s18 = string(),
        const string & s19 = string(),

        const string & s20 = string(),
        const string & s21 = string(),
        const string & s22 = string(),
        const string & s23 = string(),
        const string & s24 = string(),
        const string & s25 = string(),
        const string & s26 = string(),
        const string & s27 = string(),
        const string & s28 = string(),
        const string & s29 = string(),

        const string & s30 = string(),
        const string & s31 = string(),
        const string & s32 = string(),
        const string & s33 = string(),
        const string & s34 = string(),
        const string & s35 = string(),
        const string & s36 = string(),
        const string & s37 = string(),
        const string & s38 = string(),
        const string & s39 = string() );

}

#endif
