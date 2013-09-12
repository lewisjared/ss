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
#include <assert.h>
#include "ss/configuration.h"

namespace ss { 

namespace {
    void bulk_setting_impl(const string & name_and_value) {
        if ( name_and_value.empty() )
            return;

        string::size_type equal = name_and_value.find('=');
        assert( equal != string::npos);
        string name = name_and_value.substr(0, equal);
        string value = name_and_value.substr(equal + 1);
        detail::trim(value);
        typeinfo type;
        type = typeid(variant);

        if ( (value.size() > 2) && (value[0] == '"') && (*value.rbegin() == '"') ) {
            type = typeid(string);
            value.erase(0, 1);
            value.erase( value.size() - 1, 1);
            value = detail::unescape_string(value);
        }

        def_cfg().add_default_value(name, value, type);

    }

}

void bulk_setting(
        const string & s00,
        const string & s01,
        const string & s02,
        const string & s03,
        const string & s04,
        const string & s05,
        const string & s06,
        const string & s07,
        const string & s08,
        const string & s09,

        const string & s10,
        const string & s11,
        const string & s12,
        const string & s13,
        const string & s14,
        const string & s15,
        const string & s16,
        const string & s17,
        const string & s18,
        const string & s19,

        const string & s20,
        const string & s21,
        const string & s22,
        const string & s23,
        const string & s24,
        const string & s25,
        const string & s26,
        const string & s27,
        const string & s28,
        const string & s29,

        const string & s30,
        const string & s31,
        const string & s32,
        const string & s33,
        const string & s34,
        const string & s35,
        const string & s36,
        const string & s37,
        const string & s38,
        const string & s39) {

    bulk_setting_impl(s00);
    bulk_setting_impl(s01 );
    bulk_setting_impl(s02 );
    bulk_setting_impl(s03 );
    bulk_setting_impl(s04 );
    bulk_setting_impl(s05 );
    bulk_setting_impl(s06 );
    bulk_setting_impl(s07 );
    bulk_setting_impl(s08 );
    bulk_setting_impl(s09 );

    bulk_setting_impl(s10);
    bulk_setting_impl(s11 );
    bulk_setting_impl(s12 );
    bulk_setting_impl(s13 );
    bulk_setting_impl(s14 );
    bulk_setting_impl(s15 );
    bulk_setting_impl(s16 );
    bulk_setting_impl(s17 );
    bulk_setting_impl(s18 );
    bulk_setting_impl(s19 );

    bulk_setting_impl(s20);
    bulk_setting_impl(s21 );
    bulk_setting_impl(s22 );
    bulk_setting_impl(s23 );
    bulk_setting_impl(s24 );
    bulk_setting_impl(s25 );
    bulk_setting_impl(s26 );
    bulk_setting_impl(s27 );
    bulk_setting_impl(s28 );
    bulk_setting_impl(s29 );

    bulk_setting_impl(s30);
    bulk_setting_impl(s31 );
    bulk_setting_impl(s32 );
    bulk_setting_impl(s33 );
    bulk_setting_impl(s34 );
    bulk_setting_impl(s35 );
    bulk_setting_impl(s36 );
    bulk_setting_impl(s37 );
    bulk_setting_impl(s38 );
    bulk_setting_impl(s39 );

}

}

