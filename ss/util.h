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

#ifndef SS_UTIL_H
#define SS_UTIL_H

#pragma once

#include <string>

namespace ss { namespace detail {

std::string narrow(const std::wstring & src) ;
inline std::string narrow(const std::string & src) { return src; } 

std::wstring widen(const std::string & src);
inline std::wstring widen(const std::wstring & src) { return src; }

string full_setting_name(const string & place, const string & sett_name) ;
void trim(string & value);
string unescape_string(const string & value) ;
string escape_string(const string & value) ;


}}

#endif
