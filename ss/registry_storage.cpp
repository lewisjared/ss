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

// registry_storage.cpp: implementation of the registry_storage class.
//
//////////////////////////////////////////////////////////////////////

#include "ss/fwd.h"
#include "ss/configuration.h"
#include "ss/registry_storage.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <assert.h>

// note: it needs to be linked against advapi32.lib

#ifdef SS_USE_UNICODE
    #define RegCreateKeyEx_     RegCreateKeyExW
    #define RegQueryValueEx_    RegQueryValueExW
    #define RegSetValueEx_      RegSetValueExW
    #define RegEnumKeyEx_       RegEnumKeyExW
    #define RegEnumValue_       RegEnumValueW
    #define RegOpenKeyEx_       RegOpenKeyExW

#else
    #define RegCreateKeyEx_     RegCreateKeyExA
    #define RegQueryValueEx_    RegQueryValueExA
    #define RegSetValueEx_      RegSetValueExA
    #define RegEnumKeyEx_       RegEnumKeyExA
    #define RegEnumValue_       RegEnumValueA
    #define RegOpenKeyEx_       RegOpenKeyExA

#endif



namespace ss {

    namespace {
        // we only need 3 types: int, unsigned, string
        typeinfo friendly_type(const typeinfo& type) {
            if ( type == typeid(char))
                return typeid(string);
            else if ( type == typeid(wchar_t) )
                return typeid(string);
            else if ( type == typeid(unsigned char) )
                return typeid(string);
            else if ( type == typeid(signed char) )
                return typeid(string);
            else if ( type == typeid(short) )
                return typeid(long);
            else if ( type == typeid(unsigned short) )
                return typeid(unsigned long);
            else if ( type == typeid(int) )
                return typeid(long);
            else if ( type == typeid(unsigned int) )
                return typeid(unsigned long);
            else if ( type == typeid(long) )
                return typeid(long);
            else if ( type == typeid(unsigned long) )
                return typeid(unsigned long);
            else if ( type == typeid(double) )
                return typeid(string);
            else if ( type == typeid(float) )
                return typeid(string);
            else if ( type == typeid(bool) )
                return typeid(unsigned long);
            else if ( type == typeid(string) )
                return typeid(string);
            else if ( type == typeid(variant) )
                return typeid(variant);

            return typeid(variant); // unknown
        }
    }


    namespace {
    typedef std::vector<HKEY> keys_array;

    void close_reg_key( const keys_array & keys);

    // tries to open this sub key. In case it does not exist, tries to create it.
    std::pair<HKEY,bool> open_reg_subkey( HKEY key, const string & subkey_name, REGSAM access) {
        HKEY result = 0;
        bool ok = RegOpenKeyEx_( key, subkey_name.c_str(), 0, access, &result) == ERROR_SUCCESS;
        if (!ok) 
            // try to create it
            ok = RegCreateKeyEx_( key, subkey_name.c_str(), 0, 0, REG_OPTION_NON_VOLATILE, 
                access | KEY_CREATE_SUB_KEY, 0, &result, 0) == ERROR_SUCCESS;
        return std::pair<HKEY,bool>(result,ok);
    }

    // tries to open this registry key - in case it couldn't, it returns an empty array of keys
    void open_reg_key( const string & name, keys_array & keys, REGSAM access) {
        if ( name.empty() )
            return;

        int idx = 0;
        while ( true) {
            int next = (int)name.find_first_of( TTEXT("./\\"), idx);
            string subkey = name.substr( idx, next - idx);
            std::pair<HKEY,bool> child;
            if ( !keys.empty() ) 
                child = open_reg_subkey( keys.back(), subkey, access);
            else {
                // need to open root
                child.second = true;
                if ( subkey == TTEXT("CR") || subkey == TTEXT("HKCR"))
                    child.first = HKEY_CLASSES_ROOT;
                else if ( subkey == TTEXT("CC") || subkey == TTEXT("HKCC"))
                    child.first = HKEY_CURRENT_CONFIG;
                else if ( subkey == TTEXT("CU") || subkey == TTEXT("HKCU"))
                    child.first = HKEY_CURRENT_USER;
                else if ( subkey == TTEXT("LM") || subkey == TTEXT("HKLM"))
                    child.first = HKEY_LOCAL_MACHINE;
                else if ( subkey == TTEXT("U") || subkey == TTEXT("HKU"))
                    child.first = HKEY_USERS;
                else
                    child.second = false; // unknown root                
            }

            if ( child.second == true)
                keys.push_back( child.first);
            else {
                // there was a failure opening this key
                close_reg_key( keys);
                keys.clear();
                return;
            }

            if ( next != string::npos)
                idx = next + 1;
            else
                break; // reached end of string
        }
    }

    // closes all the keys (in reverse order)
    void close_reg_key( const keys_array & keys) {
        std::for_each( keys.rbegin(), keys.rend(), RegCloseKey);
    }


    // splits name into names for key & value
    // Example: "app.wnd.left" into "app.wnd" (key) and "left" (value)
    void split_name( const string & name, string & name_key, string & name_val) {
        // find last '.',representing the name value
        string::const_reverse_iterator found = std::find( name.rbegin(), name.rend(), '.');
        if ( found != name.rend() ) {
            string::const_iterator key_start = found.base() -1; 
            name_key = string( name.begin(), key_start);
            name_val = string( key_start + 1 /* ignore the dot */, name.end());

        }
        else {
            // we have just a value name, like "cache_size"
            name_key.erase();
            name_val = name;
        }
    }


    // gets the value from the registry
    bool get_reg_value( const string & name, HKEY key, string & value, typeinfo & set_type) {
        DWORD type;
        DWORD len = 0;
        // first, find out the size of the buffer
        RegQueryValueEx_( key, name.c_str(), 0, &type, 0, &len);
        value.resize( len);

        if ( len == 0) {
            // we don't have this value
            value.erase();
            return false;
        }

        bool ok = RegQueryValueEx_( key, name.c_str(), 0, &type, reinterpret_cast<BYTE*>(&*value.begin()), &len) == ERROR_SUCCESS;
        if ( !ok)
            return false;
        switch ( type) {
        case REG_MULTI_SZ:
            // note: we're also treating this as binary
            /* falls through */
        case REG_BINARY:
            value.resize( len / sizeof(char_t) ); 
            set_type = typeid(variant);
            return true;
        case REG_DWORD: {
            // IMPORTANT: we assume we keep UNsigned integers here
            DWORD int_value = *reinterpret_cast<DWORD*>(&*value.begin());
            ostringstream out;
            out << int_value;
            value = out.str();
            set_type = typeid(unsigned long);
            return true; } 
        case REG_QWORD: {
            // IMPORTANT: we assume we keep signed integers here
            // I assume it's a signed long here
            long long long_value = *reinterpret_cast<long long*>(&*value.begin());
            ostringstream out;
            out << (long)long_value;
            value = out.str();
            set_type = typeid(long);
            return true;
            }
        case REG_EXPAND_SZ:
            /* falls through */
        case REG_SZ:
            value.resize( (len - 1) / sizeof(char_t) ); // ignore the ending '\0'
            set_type = typeid(string);
            return true;
        default:
            // unhandled type of registry value
            assert(false);
            return false;
        };
    }

    // sets the value into the registry
    bool set_reg_value( const string & name, HKEY key, const string & value, const typeinfo & type) {
        if ( type == typeid(unsigned long) ) {
            DWORD int_val = 0;
            istringstream in(value);
            in >> int_val;
            return RegSetValueEx_( key, name.c_str(), 0, REG_DWORD, 
                reinterpret_cast<const BYTE*>(&int_val), sizeof(int_val) ) == ERROR_SUCCESS;
        }
        else if ( type == typeid(long) ) {
            DWORD long_val = 0;
            istringstream in(value);
            in >> long_val;
            long long ll_val = long_val;
            return RegSetValueEx_( key, name.c_str(), 0, REG_QWORD, 
                reinterpret_cast<const BYTE*>(&ll_val), sizeof(ll_val) ) == ERROR_SUCCESS;
        }
        else
            // string
            return RegSetValueEx_( key, name.c_str(), 0, REG_SZ, 
                reinterpret_cast<const BYTE*>(value.c_str()), ((DWORD)value.length() + 1) * sizeof(char_t) ) == ERROR_SUCCESS;
    }

    } // anonymous namespace 

registry_storage::registry_storage( const string & root)
    : m_root( root)
{
#ifndef NDEBUG
    // debug mode - see that root is a valid string
    keys_array keys;
    open_reg_key( root, keys, KEY_QUERY_VALUE);
    // root is not a valid string!
    assert( !keys.empty() );
    close_reg_key( keys);
#endif
}

registry_storage::~registry_storage()
{

}

void registry_storage::save() {
    // nothing to save. - each setting is saved automatically, when set
}

void registry_storage::get_setting( const string & name, string & value,typeinfo& type) const {
    keys_array keys;
    open_reg_key( m_root, keys, KEY_QUERY_VALUE);
    string name_key, name_val;
    split_name( name, name_key, name_val);
    open_reg_key( name_key, keys, KEY_QUERY_VALUE);
    bool is_ok = !keys.empty();
    if ( is_ok)
        is_ok = get_reg_value( name_val, keys.back(), value, type);

    close_reg_key( keys);
    if ( !is_ok) {
        bool has_default;
        parent()->get_default_value( full_setting_name(name), value, type, has_default);
        if ( !has_default)
            set_error(err::bad_setting_name, TTEXT("(get) could not open registry key ") + m_root + TTEXT("/") + name);
    }
}

void registry_storage::set_setting( const string & name, const string & value, const typeinfo& type) {
    keys_array keys;
    open_reg_key( m_root, keys, KEY_SET_VALUE);
    string name_key, name_val;
    split_name( name, name_key, name_val);
    open_reg_key( name_key, keys, KEY_SET_VALUE);
    bool is_ok = !keys.empty() ;
    if ( is_ok)
        is_ok = set_reg_value( name_val, keys.back(), value, friendly_type(type) );

    close_reg_key( keys);
    if ( !is_ok)
        set_error(err::bad_setting_name, TTEXT("(set) could not open registry key ") + m_root + TTEXT("/") + name);
}

namespace {

    void enum_settings_impl( HKEY key, REGSAM access, const string & subkey_name, std::map<string,string> & values, string & error) {
        string buff;
        buff.resize( 512);
        DWORD len = 0;
        FILETIME ignore;
        int idx = 0; 
        // go though children
        while ( true) {
            len = 512;
            LONG res = RegEnumKeyEx_( key, idx, &*buff.begin(), &len, 0, 0, 0, &ignore);
            bool ok = ( res == ERROR_SUCCESS || res == ERROR_MORE_DATA || res == ERROR_NO_MORE_ITEMS);
            if ( !ok)
                error += TTEXT("Could not get registry key for ") + subkey_name + TTEXT("\n");
            bool do_continue = ok && (res != ERROR_NO_MORE_ITEMS);
            if ( !do_continue) break;

            const string curr_key_name( buff.begin(), buff.begin() + len);
            HKEY child;
            ok = RegOpenKeyEx_( key, curr_key_name.c_str(), 0, access, &child) == ERROR_SUCCESS;
            if ( ok) {
                const string new_name = !subkey_name.empty() ? (subkey_name + TTEXT(".") + curr_key_name) : curr_key_name;
                enum_settings_impl( child, access, new_name, values, error);
                RegCloseKey( child);
            }
            else
                error += TTEXT("Could not get registry key for ") + subkey_name + TTEXT("\n");
            ++idx;
        }

        // now, enumerate values
        idx = 0;
        while ( true) {
            len = 512;
            LONG res = RegEnumValue_( key, idx, &*buff.begin(), &len, 0, 0, 0, 0);
            bool ok = ( res == ERROR_SUCCESS || res == ERROR_MORE_DATA || res == ERROR_NO_MORE_ITEMS);
            if ( !ok)
                error += TTEXT("Could not get registry value for ") + subkey_name + TTEXT("\n");
            bool do_continue = ok && (res != ERROR_NO_MORE_ITEMS);
            if ( !do_continue) break;

            string curr_name( buff.begin(), buff.begin() + len);
            string curr_val;
            typeinfo ignore;
            if ( get_reg_value( curr_name, key, curr_val, ignore) ) {
                if ( curr_name.empty() ) 
                    curr_name = TTEXT("(default)"); // the registry "(Default)" value.
                string name_val = !subkey_name.empty() ? (subkey_name + TTEXT(".") + curr_name) : curr_name;
                values[ name_val] = curr_val;
            }
            else
                error += TTEXT("Could not get registry value for ") + subkey_name + TTEXT(".") + curr_val + TTEXT("\n");
            ++idx;
        }        
    }

}


void registry_storage::enum_settings( std::map<string,string> & values) const {
    string error;
    values.clear();

    keys_array keys;
    REGSAM access = KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS;
    open_reg_key( m_root, keys, access);
    if ( !keys.empty() )
        enum_settings_impl( keys.back(), access, TTEXT(""), values, error);

    if ( !error.empty() )
        set_error(err::cannot_enum_settings, error);
    close_reg_key( keys);
}



} // namespace persist
