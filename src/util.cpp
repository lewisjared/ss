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

#include "ss/fwd.h"

namespace ss { namespace detail {

/** 
    Converts from Unicode to Ascii (no encoding whatsoever)
*/
std::string narrow(const std::wstring & src) {
    std::string dest;
    dest.resize( src.size());
    for ( int idx = 0; idx < (int)src.size(); ++idx)
        dest[idx] = (char)src[idx];
    return dest;
}

/** 
    Converts from Ascii to Unicode
*/
std::wstring widen(const std::string & src) {
    std::wstring dest;
    dest.resize( src.size());
    for ( int idx = 0; idx < (int)src.size(); ++idx)
        dest[idx] = src[idx];
    return dest;
}

string full_setting_name(const string & place, const string & sett_name) {
    if ( !place.empty() && !sett_name.empty() )
        return place + TTEXT(".") + sett_name;
    else
        // one of these is empty anyway, we'll return the non-empty one
        return place + sett_name;
}

void trim(string & value) {
    // remove leading and trailing spaces
    while ( !value.empty() && isspace(value[0]))
        value.erase(0, 1);
    while ( !value.empty() && isspace( *value.rbegin() ))
        value.erase( value.size() - 1, 1);
}

string unescape_string(const string & value) {
    string unescaped;
    unescaped.reserve(value.size());
    for ( string::const_iterator b = value.begin(), e = value.end(); b != e ; ++b)
        if ( *b == '\\' && ((b + 1) != e)) {
            switch ( b[1]) {
                case '\\': unescaped += '\\'; ++b; break;
                case 'n': unescaped += '\n'; ++b; break;
                case 'r': unescaped += '\r'; ++b; break;
                case '"': unescaped += '"'; ++b; break;
                default: unescaped += *b; break;
            }
        }
        else
            unescaped += *b;
    return unescaped;
}

string escape_string(const string & value) {
    string escaped;
    escaped.reserve(value.size() * 2);
    for ( string::const_iterator b = value.begin(), e = value.end(); b != e ; ++b)
        switch ( *b) {
            case '\\': escaped += TTEXT("\\\\"); break;
            case '\n': escaped += TTEXT("\\n"); break;
            case '\r': escaped += TTEXT("\\r"); break;
            case '"':  escaped += TTEXT("\\\""); break;
            default:   escaped += *b; break;
        }
    return escaped;
}



}}

