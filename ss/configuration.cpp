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

// configuration.cpp: implementation of the configuration class.
//
//////////////////////////////////////////////////////////////////////

#include "ss/configuration.h"
#include "ss/setting_storage.h"
#include "ss/setting.h"
#include <algorithm>
#include <assert.h>

using ss::detail::scoped_lock;


namespace ss {

namespace {
    // make sure default configuration is initialized before main
    struct configuration_init {
        configuration_init() { configuration::def(); }
    } s_init;

    // converts a value to lower-case
    string locase( const string & str) {
        string lo;
        lo.resize( str.length());
        std::transform( str.begin(), str.end(), lo.begin(), tolower);
        return lo;
    }

    struct do_delete {
        template< class T> void operator()( T & val) {
            delete val.second;
        }
    };
}

configuration & configuration::def() {
    def_cfg obj;
    static configuration d( obj);
    d.init_def_cfg();
    return d;
}


// constructor for default configuration
configuration::configuration( const configuration::def_cfg &) {
    static int idx = 0;
    ++idx;
    if ( idx > 1)
        // we have entered this constructor recursively !!!
        // make sure you've implemented init_settings correctly!
        assert(false);
}

void configuration::init_def_cfg() {
    static bool already_inited = false;
    if ( already_inited ) {
        // we have entered this constructor recursively !!!
        return;
    }
    already_inited = true;

    m_on_error = err::do_ignore; //default handler
    setting_defaults(true);
    init_settings();
    setting_defaults(false);
}


configuration::configuration() {
}


configuration::~configuration() {
    save();
    std::for_each( m_storages.begin(), m_storages.end(), do_delete() );
}

void configuration::setting_defaults(bool we_are_setting_defaults) {
    scoped_lock lock(m_cs);
    m_we_are_setting_defaults = we_are_setting_defaults;
}

/* 
    resolves the name: finds the storage place where this propery belongs to, and its name there
    note: all names are case-insensitive

    @param is_const if true, this name is supposed to be used as a constant. Unless it's already marked as such,
    we'll trigger an error
*/
void configuration::resolve_name( const string & name, string & place, string & sett_name, resolve_type resolve) const {
    place.erase();
    sett_name.erase();

    // name should not be empty, and should not begin with "." ('.' is a separator)
    if( name.empty() || (name[0] == '.')) {
        get_error_handler()(err::bad_setting_name, TTEXT("bad setting name"));
        return;
    }
    string lo_name = locase(name);

    scoped_lock lock(m_cs);

    if ( (resolve == resolve_writable) && (m_const_names.find(name) != m_const_names.end()) ) {
        assert(false);
        get_error_handler()(err::const_setting, TTEXT("this setting was marked as const") + name);
    }

    if ( m_we_are_setting_defaults) {
        // we're setting defaults - we don't even care of where the real destination is
        place = TTEXT("");
        sett_name = name;
        return;
    }

    // before doing any operation, make sure you have at least one storage to persist settings to
    if ( m_storages.empty() ) {
        get_error_handler()(err::no_storages, TTEXT("no storages, while trying to resolve name"));
        return;
    }

    coll::const_reverse_iterator first = m_storages.rbegin(), last = m_storages.rend();
    while ( first != last) {
        const string & storage_name = first->first;
        if ( lo_name.size() > storage_name.size() ) {
            // name can be a setting in storage name
            int pos = (int)lo_name.find( storage_name);
            if ( pos == 0) {
                int end = (int)storage_name.size();
                if ( (lo_name[end] == '.') || storage_name.empty() ) {
                    // name is a setting in storage name;
                    // note: all names are settings into the empty storage name
                    place = storage_name;
                    sett_name = end > 0 ? lo_name.substr( end + 1) : lo_name;
                    return;
                }
            }
        }
        ++first;
    }

    // the name does not exist in any storage
    sett_name = name;
}


void configuration::get_setting( const string & place, const string & sett_name, string & value, typeinfo &type) {
    setting_storage * dest_storage = 0;
    {
    scoped_lock lock(m_cs);
    // before doing any operation, make sure you have at least one storage to persist settings to
    if ( m_storages.empty() ) {
        get_error_handler()(err::no_storages, TTEXT("no storages, while trying to get setting"));
        return;
    }

    coll::iterator found = m_storages.find( place);
    if ( found != m_storages.end() ) {
        dest_storage = found->second;
        dest_storage->use();
    }
    } // un-lock

    typeinfo original_type = type;
    if ( dest_storage) {
        type = typeid(variant); //default
        dest_storage->do_get_setting( sett_name, value, type );
        dest_storage->un_use();
    }
    else {
        bool has_default;
        m_defaults_holder.get_default(detail::full_setting_name(place,sett_name), value, type, has_default);
        if ( !has_default)
            get_error_handler()( err::storage_not_found, TTEXT("(get) storage not found") );
    }

    // see if it was an enum
    int enum_value;
    if ( m_enum_holder.get_enum(original_type, value, enum_value)) {
        // it's an enum, and it's been converted
        ostringstream out;
        out << enum_value;
        value = out.str();
        type = typeid(variant);
    }
}

void configuration::set_setting( const string & place, const string & sett_name, const string & value, const typeinfo &type) {
    bool should_set_default = false;
    {
    scoped_lock lock(m_cs);
    // are we setting defaults?
    if ( m_we_are_setting_defaults) 
        // resolve_name should have set the place to empty, and sett_name to original setting name
        should_set_default = true;
    }
    if ( should_set_default) {
        assert(place.empty());
        m_defaults_holder.add_default(sett_name, value, type);
        return;
    }

    setting_storage * dest_storage = 0;
    {
    scoped_lock lock(m_cs);
    // before doing any operation, make sure you have at least one storage to persist settings to
    if ( m_storages.empty() ) {
        get_error_handler()(err::no_storages, TTEXT("no storages, while trying to set setting"));
        return;
    }

    coll::iterator found = m_storages.find( place);
    if ( found != m_storages.end() ) {
        dest_storage = found->second;
        dest_storage->use();
    }
    else
        get_error_handler()( err::storage_not_found, TTEXT("(set) storage not found"));

    } // un-lock

    if ( dest_storage) {
        bool was_enum = false;
        if ( m_enum_holder.is_enum(type)) {
            int enum_ = -1;
            string enum_as_string;
            istringstream in(value);
            in >> enum_;
            if ( m_enum_holder.set_enum(type, enum_, enum_as_string)) {
                was_enum = true;
                dest_storage->do_set_setting( sett_name, enum_as_string, type );
            }
        }

        if ( !was_enum)
            dest_storage->do_set_setting( sett_name, value, type );
        dest_storage->un_use();
    }
}

void configuration::force_setting_to_be_const(const string & name) {
    scoped_lock lock(m_cs);
    m_const_names.insert(name);
}



void configuration::add_storage(const string &storage_name, setting_storage *store) {
    store->use();
    store->name(storage_name);
    {
    scoped_lock lock(m_cs);
    // this storage should not exist yet
    if ( m_storages.find(storage_name) != m_storages.end() ) {
        get_error_handler()(err::storage_already_exists, TTEXT("storage already exists") );
        remove_storage(storage_name);
    }
    m_storages[ storage_name] = store;
    }

    store->parent( this);
}


// copies this configuration into another one 
//
// in case the other configuration already contains some settings,
// they will be overwritten (in case some settings have names that are not
// found in the current configuration, their values will remain unchanged)
void configuration::copy_into(configuration &other ) {
    scoped_lock lock(m_cs);
    coll::iterator first = m_storages.begin(), last = m_storages.end();
    while ( first != last) {
        typedef std::map<string,string> vals_coll;
        vals_coll vals;
        first->second->use();
        first->second->do_enum_settings( vals);
        first->second->un_use();

        vals_coll::iterator first_val = vals.begin(), last_val = vals.end();
        while ( first_val != last_val) {
            // find out the name of the setting, within the other configuration
            // (each setting we're enumerating, is relative to the *first storage)
            string full_name = first_val->first;
            if ( !first->first.empty())
                full_name = first->first + TTEXT(".") + full_name;

            string place, sett_name;
            other.resolve_name( full_name, place, sett_name, resolve_dont_care);
            // FIXME(later) at this time, when copying settings, we lose all context of the settings, we should fix that!
            other.set_setting( place, sett_name, first_val->second, typeid(string) );
            ++first_val;
        }
        ++first;
    }
    other.save( );
}

namespace {
    class setting_does_not_exist{};
    void no_overwrite(int, const string&) {
        throw setting_does_not_exist();
    }
}

// copies this configuration into another one 
//
// in case some settings already exist in the other configuration, they WILL NOT be overwritten
// (only new settings are added into the other configuration)
void configuration::copy_into_no_overwrite(configuration &other) {
    error_handler_func old_handler = other.get_error_handler();
    other.set_error_handler(no_overwrite);
    try {
        {
        scoped_lock lock(m_cs);
        coll::iterator first = m_storages.begin(), last = m_storages.end();
        while ( first != last) {
            typedef std::map<string,string> vals_coll;
            vals_coll vals;
            first->second->use();
            first->second->do_enum_settings( vals);
            first->second->un_use();

            vals_coll::iterator first_val = vals.begin(), last_val = vals.end();
            while ( first_val != last_val) {
                // find out the name of the setting, within the other configuration
                // (each setting we're enumerating, is relative to the *first storage)
                string full_name = first_val->first;
                if ( !first->first.empty())
                    full_name = first->first + TTEXT(".") + full_name;

                string place, sett_name;
                other.resolve_name( full_name, place, sett_name, resolve_dont_care);
    			
                try {
			        string ignore;
                    typeinfo ignore_type;
			        other.get_setting( place, sett_name, ignore, ignore_type);
                    // setting exists - no overwrite
                }
                catch(setting_does_not_exist&) {
				    // an error occured - we assume the setting did not exist
                    // FIXME(later) at this time, when copying settings, we lose all context of the settings, we should fix that!
				    other.set_setting( place, sett_name, first_val->second, typeid(string) );
                }
                ++first_val;
            }
            ++first;
        }
        }
        other.save();
    }
    catch(...) {
        other.set_error_handler(old_handler);
        throw;
    }
    other.set_error_handler(old_handler);
}



// saves this configuration to the underlying storages
// (useful when any of the storages has a caching mechanism)
void configuration::save() {
    scoped_lock lock(m_cs);
    coll::iterator first = m_storages.begin(), last = m_storages.end();
    while ( first != last) {
        first->second->do_save();
        ++first;
    }
}

// removes one storage from this configuration
void configuration::remove_storage( const string & storage_name) {
    setting_storage * dest_storage = 0;
    {
    scoped_lock lock(m_cs);
    coll::iterator found = m_storages.find(storage_name);
    if ( found != m_storages.end() ) {
        dest_storage = found->second;
        m_storages.erase( found);
    }
    }

    if ( dest_storage) {
        dest_storage->do_save();
        dest_storage->un_use();
    }
}

// removes all storages from this configuration
void configuration::remove_all_storages() {
    scoped_lock lock(m_cs);
    save();
    std::for_each( m_storages.begin(), m_storages.end(), do_delete() );
    m_storages.clear();
}

void configuration::set_error_handler(error_handler_func func) {
    scoped_lock lock(m_cs);
    m_on_error = func;
}

error_handler_func configuration::get_error_handler() const {
    scoped_lock lock(m_cs);
    return m_on_error;
}

void configuration::get_default_value(const string & name, string & value, typeinfo & type, bool & has_default) const {
    m_defaults_holder.get_default(name, value, type, has_default);
}

void configuration::add_default_value(const string & name, string & value, const typeinfo & type) {
    m_defaults_holder.add_default(name, value, type);
}

void configuration::add_enum_value(const typeinfo & type, int enum_, const string& str) {
    m_enum_holder.add_enum(type, enum_, str);
}


} // namespace ss




