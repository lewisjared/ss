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

#include "ss/configuration.h"
#include "ss/setting.h"

namespace ss {

void register_enum_value(const typeinfo & type, int enum_, const string& str) {
    def_cfg().add_enum_value(type, enum_, str);
}

int enum_::str_to_enum( const typeinfo & type, const string & str_value) const {
    int result = 0;
    if ( m_delegate.conf().enum_holder_().get_enum(type, str_value, result))
        ; // converted string to enum
    else {
        // it's a number
        istringstream in(str_value);
        in >> result;
    }
    return result;
}

string enum_::enum_to_str( const typeinfo & type, int enum_value) const {
    string result;
    if ( m_delegate.conf().enum_holder_().set_enum(type, enum_value, result))
        ; // converted enum to string
    else {
        // it's a number
        ostringstream out;
        out << enum_value;
        result = out.str();
    }
    return result;
}


}
