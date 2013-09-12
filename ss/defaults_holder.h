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

#ifndef SS_DEFAULTS_FOLDER_H
#define SS_DEFAULTS_FOLDER_H

#pragma once

namespace ss {

/** 
    holds default values for settings
*/
class defaults_holder {
    typedef ::ss::detail::scoped_lock scoped_lock;

    struct info {
        info(const string & value = string(), const typeinfo & type = typeinfo() )
            : value(value), type(type) {}
        string value;
        typeinfo type;
    };

public:
    void add_default(const string & name, const string & value, const typeinfo & type) {
        scoped_lock lk(m_cs);
        m_infos[name] = info(value, type);
    }

    void get_default(const string & name, string & value, typeinfo & type, bool & has_default) const {
        scoped_lock lk(m_cs);
        info_coll::const_iterator found = m_infos.find(name);
        if ( found != m_infos.end()) {
            has_default = true;
            value = found->second.value;
            type = found->second.type;
        }
        else 
            has_default = false;
    }
    
private:
    typedef std::map<string,info> info_coll;
    info_coll m_infos;

    mutable ::ss::detail::critical_section m_cs;
};

}

#endif
