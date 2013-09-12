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

// configuration.h: interface for the configuration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SS_CONFIGURATION_H)
#define SS_CONFIGURATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable :4786) // msvc

#include "ss/ts.h"
#include "ss/fwd.h"
#include <map>
#include <string>
#include <set>
#include "ss/defaults_holder.h"
#include "ss/bulk_setting.h"
#include "ss/enum.h"

namespace ss {

class setting_storage;

/*
    A configuration contains multiple settings that can be get/set.
    It shields you from where each of these settings are stored (persisted).

    Note that you normally deal with only one configuration throughout the code (the default one).
    However, you can have multiple configs at once for more complex scenarios (like, copying all info
    from a confing into another config)
*/
class configuration  
{
    class def_cfg {};
    configuration( const def_cfg &);
public:
	configuration();
	~configuration();

    static configuration & def();

    typedef enum resolve_type {
        // setting should be writable (default)
        resolve_writable,
        // setting should be read-only (const)
        resolve_read_only,
        // doesn't matter which type of setting it is
        resolve_dont_care
    };

    void resolve_name( const string & name, string & place, string & sett_name, resolve_type resolve) const;
    void get_setting( const string & place, const string & sett_name, string & value, typeinfo &type);
    void set_setting( const string & place, const string & sett_name, const string & value, const typeinfo &type);

    void force_setting_to_be_const(const string & name);

    void setting_defaults(bool we_are_setting_defaults);
    
    void add_storage( const string & storage_name, setting_storage * store);
    void remove_storage( const string & storage_name);
    void remove_all_storages();

    void save();
    void copy_into( configuration & other );
    void copy_into_no_overwrite( configuration & other);

    void add_default_value(const string & name, string & value, const typeinfo & type) ;
    void get_default_value(const string & name, string & value, typeinfo & type, bool & has_default) const;
    void add_enum_value(const typeinfo & type, int enum_, const string& str);
    const enum_holder & enum_holder_() const { return m_enum_holder; }
    
    void set_error_handler(error_handler_func func);
    error_handler_func get_error_handler() const;

private:
    void init_def_cfg() ;
private:
    mutable ::ss::detail::critical_section m_cs;

    error_handler_func m_on_error;

    typedef std::map<string,setting_storage*> coll;
    coll m_storages;

    typedef std::set<string> set;
    set m_const_names;

    bool m_we_are_setting_defaults;
    defaults_holder m_defaults_holder;

    enum_holder m_enum_holder;
};

inline void set_error_handler(error_handler_func func) {
    configuration().set_error_handler(func);
}

// this function needs to be provided by the user of the library, to initalize the (default) configuration
void init_settings();

inline configuration & def_cfg() { return configuration::def(); }

} // namespace ss


#endif // !defined(AFX_CONFIGURATION_H__7F1CC6BE_9F02_465B_8926_3BA732AC28C0__INCLUDED_)
