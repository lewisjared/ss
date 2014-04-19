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

// fwd.h: forward declarations/defines
//
//////////////////////////////////////////////////////////////////////

#if !defined(SS_FWD_H)
#define SS_FWD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>
#include <fstream>

#include "ss/ts.h"
#include <typeinfo>

#if defined(_UNICODE) || defined(UNICODE)

    #ifndef SETTING_CHAR
        #define SS_USE_UNICODE
        #define SETTING_CHAR wchar_t
    #else
        // even if your app is unicode, you can use the settings to be ASCII

        // if you've defined SETTING_CHAR, and you're app is unicode,
        // if you want this library to use Unicode, you'll have to #define SS_USE_UNICODE
    #endif

#endif

#ifndef SETTING_CHAR
#define SETTING_CHAR char
#endif

#ifndef SS_DONT_USE_BOOST
#define SS_USE_BOOST_ENUM
#endif

namespace ss {
    typedef SETTING_CHAR char_t;
    typedef std::basic_string<char_t> string;

    typedef std::basic_istringstream<char_t> istringstream;
    typedef std::basic_ostringstream<char_t> ostringstream;
    typedef std::basic_stringstream<char_t> stringstream;

    typedef std::basic_ifstream<char_t> ifstream;
    typedef std::basic_ofstream<char_t> ofstream;
    typedef std::basic_fstream<char_t> fstream;


    // forward declaration
    class setting_storage;

    typedef void (*error_handler_func)(int, const string&);

    // in case for a type we don't know its real underlying type for sure, we'll consider it variant
    struct variant {};


    struct typeinfo {
        typeinfo(const std::type_info& val) : raw_type( const_cast<std::type_info*>(&val) ) {}
        typeinfo() : raw_type(0) {}
        std::type_info * raw_type;
    };
    inline bool operator==(const typeinfo & a, const typeinfo & b) { return a.raw_type == b.raw_type; }
    inline bool operator!=(const typeinfo & a, const typeinfo & b) { return !(a == b); }
    inline bool operator<(const typeinfo & a, const typeinfo & b) { return a.raw_type->before(*(b.raw_type)) > 0; }

}

#include "ss/error.h"
#include "ss/util.h"


// for string errors
#ifdef SS_USE_UNICODE
#define TTEXT(x)      L ## x
#else
#define TTEXT(x) x
#endif

#include <tchar.h>
#endif

