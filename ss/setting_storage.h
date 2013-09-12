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

// setting_storage.h: storage base class
//
//////////////////////////////////////////////////////////////////////

#if !defined(SS_SETTING_STORAGE_H)
#define SS_SETTING_STORAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ss/fwd.h"
#include <map>
#include <assert.h>

namespace ss {

class configuration;


/*
    Represents a storage when settings are persisted.
    You should note that a configuration can consist of more than one storage.

    Abstract class.

    Note that you always create an setting storage with new. It will be destroyed by default

    Example:
    def_cfg.add_storage("", new file_storage(...));
*/
class setting_storage  
{
protected:
    setting_storage() : m_use_count(0), m_conf(0) {}
public:
    virtual ~setting_storage() {}

protected:
    // saves all (modified) settings;
    // useful in case your storage class has a caching mechanism)
    virtual void save( ) = 0;
    // returns a setting as a string. In case an error appears, sets the 'error' string
    virtual void get_setting( const string & name, string & value, typeinfo&) const = 0;
    // sets a setting. In case an error appears, sets the 'error' string
    virtual void set_setting( const string & name, const string & value, const typeinfo&) = 0;
    // enumerates all settings. If an error occurs, just sets the error string.
    //
    // note that some of the settings might still be valid, even if the error string is set
    // (for example, some of the registry settings cannot be read, while most of them were successfully read)
    virtual void enum_settings( std::map<string,string> & values) const {
        // by default, we cannot enumerate the settings
        values.clear();
        set_error(err::cannot_enum_settings, TTEXT("cannot enumerate settings"));
    }

public:
    void use() {
        { scoped_lock lk(m_use_cs);
          ++m_use_count;
        }
    }

    void un_use() {
        int count;
        { scoped_lock lk(m_use_cs);
          count = --m_use_count;
        }
        delete_if_needed(count);
    }

    void do_save() {
        // client has already called use()
        scoped_lock lk(m_cs);
        save();
        // client will call un_use()
    }

    void do_get_setting(const string & name, string & value, typeinfo& t) {
        // client has already called use()
        scoped_lock lk(m_cs);
        get_setting(name, value, t);
        // client will call un_use()
    }

    void do_set_setting(const string & name, const string & value, const typeinfo& t) {
        // client has already called use()
        scoped_lock lk(m_cs);
        set_setting(name, value, t);
        // client will call un_use()
    }

    void do_enum_settings(std::map<string,string> & values) {
        // client has already called use()
        scoped_lock lk(m_cs);
        enum_settings(values);
        // client will call un_use()
    }


    void parent(configuration * conf) {
        scoped_lock lk(m_cs);
        // you should set this only once!
        assert( !m_conf);
        m_conf = conf;
    }

    configuration * parent() const {
        return m_conf;
    }

    // in the configuration - the name of this setting storage
    void name(const string& n) {
        scoped_lock lk(m_cs);
        m_name = n;
    }

protected:
    void set_error(int err_code, const string & error) const {
        // already in scoped lock
        m_conf->get_error_handler()( err_code, error);
    }

private:
    string name() const {
        scoped_lock lk(m_cs);
        return m_name;
    }
protected:
    string full_setting_name(const string & sett_name) const {
        return detail::full_setting_name( name(), sett_name);
    }

private:
    void delete_if_needed(int count) {
        if ( count <= 0)
            delete this;
    }


protected:
    typedef ::ss::detail::scoped_lock scoped_lock;
    ::ss::detail::critical_section & cs() const { return m_cs; }
    
private:
    // note: we use 2 critical sections :
    // m_use_cs - to increase/decrease whenever the setting is used
    // m_cs     - to provide thread-safety of the class's operations
    //
    // we could use only one critical section, but then just calling use()/un_use() could take too long
    // if another thread is doing an operation (save, get_setting, etc)
    mutable ::ss::detail::critical_section m_use_cs;
    // how many times is this setting used?
    int m_use_count;

    mutable ::ss::detail::critical_section m_cs;

    mutable configuration * m_conf;

    string m_name;
};


}


#endif
