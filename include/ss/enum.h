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


#ifndef SS_ENUM_H
#define SS_ENUM_H

#pragma once

#include <string>

namespace ss { 

/* 
    Enum-to-string convertion rules:

    the string must:
    - start with a letter (a-z,A-Z)
    - must not contain ANY spaces
    - must not contain any quotations (' or ")
    - must not contain the '\\' character

    Examples:
    yes
    no
    this_is_ok
    light_cyan
    green-orange

@remarks

    We don't need thread-safety - the enums are added in init_settings(), then, all we use is getter functions
*/

class enum_holder {
    typedef std::map<string, int> string_to_int_coll;
    typedef std::map<int, string> int_to_string_coll;
    struct info {
        string_to_int_coll str_to_int;
        int_to_string_coll int_to_str;
    };
    typedef std::map<typeinfo, info> enum_coll;
    enum_coll m_enums;
public:
    void add_enum(const typeinfo & type, int enum_, const string& str) {
        m_enums[ type ].str_to_int[str] = enum_;
        m_enums[ type ].int_to_str[enum_] = str;
    }

    bool is_enum(const typeinfo & type) const {
        return m_enums.find(type) != m_enums.end();
    }

    bool get_enum(const typeinfo & type, const string & str, int & result) const {
        result = -1;
        bool enum_found = false;
        enum_coll::const_iterator found = m_enums.find(type);
        if ( found != m_enums.end() ) {
            string_to_int_coll::const_iterator enum_it = found->second.str_to_int.find(str);
            if ( enum_it != found->second.str_to_int.end()) {
                enum_found = true;
                result = enum_it->second;
            }
        }
        return enum_found;
    }

    bool set_enum(const typeinfo & type, int enum_, string & result) const {
        result = -1;
        bool enum_found = false;
        enum_coll::const_iterator found = m_enums.find(type);
        if ( found != m_enums.end() ) {
            int_to_string_coll::const_iterator enum_it = found->second.int_to_str.find(enum_);
            if ( enum_it != found->second.int_to_str.end()) {
                enum_found = true;
                result = enum_it->second;
            }
        }
        return enum_found;
    }

};


void register_enum_value(const typeinfo & type, int enum_, const string& str);

template<class type> struct register_enum {
    typedef register_enum<type> self_type;

    self_type & operator()(type value, const string & str) {
        register_enum_value( typeid(type), value, str);
        return *this;
    }
};




}

#endif
