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

#if !defined(SS_ERROR_H)
#define SS_ERROR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable :4786) // msvc

#include <stdexcept>
#include <sstream>

namespace ss {

namespace err {
    typedef enum type {
        storage_not_found = 1,
        // value cannot be converted from string to underlying type, or the other way around
        cannot_convert,
        no_storages,
        // an invalid setting name - like, an empty name or something that starts with "."
        bad_setting_name,
        storage_already_exists,
        // this is not necessary an error - it just signals that we cannot enumerate a certain storage's settings
        cannot_enum_settings,
        const_setting
    };

    ////////////////////////////////////////////////////
    // Existing error function impleemntation
    void do_ignore(int, const string&);
    void do_log_to_console(int, const string&);
    void do_log_do_dbg_wnd(int, const string&);
    void do_assert(int, const string&);
    void do_throw(int, const string&);

};

} 

#endif 
