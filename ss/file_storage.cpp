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
#include "ss/file_storage.h"
#include <algorithm>


namespace ss {

    namespace {
        // converts a value to lower-case
        string locase( const string & str) {
            string lo;
            lo.resize( str.length());
            std::transform( str.begin(), str.end(), lo.begin(), tolower);
            return lo;
        }
    }

file_storage::file_storage(const std::string & file_name, open_type open, save_type save, int interval_ms) 
        : m_file_name(file_name), m_open(open), m_save(save), m_interval_ms(interval_ms), m_is_dirty(false) {

    load();
#ifdef SS_TS_WIN
    DWORD thread_id;
    m_dedicated_thread = ::CreateThread(0, 0, &file_storage::save_thread, this, 0, &thread_id);
    m_is_dedicated_thread_running = true;
#else
    // if we don't have save_at_interval for a platform, revert to closest thing
    if ( save == save_at_interval)
        save = save_each_modify;
#endif
}

file_storage::~file_storage(void) {
#ifdef SS_TS_WIN
    if ( m_save == save_at_interval) {
        // need to close the other thread, peacefully
        {
        scoped_lock lk(cs());
        m_is_dedicated_thread_running = false;
        }
        while ( true) {
            ::Sleep(10);
            scoped_lock lk(cs());
            if (m_is_dedicated_thread_running )
                break; // the other thread has ended
        }
    }
#endif
    save();
}

#ifdef SS_TS_WIN
DWORD WINAPI file_storage::save_thread(LPVOID param) {
    file_storage * self = (file_storage*)param;
    int sleeped = 0;
    int SLEEP_EACH_TIME = 10;
    while ( true) {
        { scoped_lock lk(self->cs());
          if ( !self->m_is_dedicated_thread_running) {
              // signal to the other thread - we've ended
              self->m_is_dedicated_thread_running = true;
              break;
          }
        }
        ::Sleep(SLEEP_EACH_TIME);
        sleeped += SLEEP_EACH_TIME;
        if ( sleeped > self->m_interval_ms) {
            sleeped = 0;
            scoped_lock lk(self->cs());
            self->save();
        }
    }
    return 0;
}
#endif

void file_storage::load() {
    m_infos.clear();
    string last_comment;
    bool is_first_setting = true;
    int idx = 0;
    ifstream in( m_file_name.c_str() );
    string line;
    while ( std::getline(in, line) ) {
        info parsed;
        read_setting(line, parsed);
        if ( !parsed.name.empty() ) {
            // this comment is to be written after this setting
            std::swap(last_comment, parsed.comment);
            parsed.idx = idx++;
            parsed.name = locase(parsed.name);
            m_infos[ parsed.name ] = parsed;
        }
        else {
            // comment - append to last comment
            if ( !is_first_setting) last_comment += TTEXT("\n");
            is_first_setting = false;
            last_comment += parsed.comment;
        }
    }

    if ( !last_comment.empty() ) {
        // there is a comment after all settings
        info last;
        last.comment = last_comment;
        last.idx = idx;
        last.name = TTEXT("");
        m_infos[ last.name ] = last;
    }
}

void file_storage::save() {
    if ( !m_is_dirty)
        return;

    if ( m_open == open_read_only)
        return; // never save

    typedef std::map<int, string> index_to_name_coll;
    index_to_name_coll index_to_name;
    for ( info_coll::const_iterator b = m_infos.begin(), e = m_infos.end(); b != e; ++b)
        index_to_name[ b->second.idx ] = b->first;

    ofstream out(m_file_name.c_str());
    for ( index_to_name_coll::const_iterator b = index_to_name.begin(), e = index_to_name.end(); b != e; ++b) {
        info & cur = m_infos[ b->second ];
        write_setting(out, cur);
    }

    m_is_dirty = false;
}

namespace {
    void strip_comment(string & line, string & comment) {
        comment.erase();
        int to_strip = -1;
        for ( string::reverse_iterator b = line.rbegin(), e = line.rend(); b != e; ++b)
            if ( *b == '#')
                to_strip = (int)(b - line.rbegin()); // found comment
            else if ( *b == '"')
                break; // found end of string

        if ( to_strip >= 0) {
            comment = line.substr( line.size() - to_strip - 1);
            line = line.substr(0, line.size() - to_strip - 1);
        }
    }

}

void file_storage::read_setting(string line, info & parsed) {
    strip_comment(line, parsed.comment);
    string::size_type equal = line.find('=');
    if ( equal != string::npos) {
        parsed.name = line.substr(0, equal);
        string value = line.substr(equal + 1);
        // remove leading and trailing spaces
        detail::trim(value);

        if ( (value.size() > 2) && (value[0] == '"') && (*value.rbegin() == '"') ) {
            parsed.type = typeid(string);
            value.erase(0, 1);
            value.erase( value.size() - 1, 1);
            parsed.value = detail::unescape_string(value);
        }
        // otherwise, it'a number or bool
        else if ( value == TTEXT("true")) {
            parsed.type = typeid(bool);
            parsed.value = TTEXT("1");
        }
        else if ( value == TTEXT("false")) {
            parsed.type = typeid(bool);
            parsed.value = TTEXT("0");
        }
        else if ( !value.empty() && value[0] == '-') {
            parsed.type = typeid(long);
            parsed.value = value;
        }
        else if ( !value.empty() && isdigit(value[0])) {
            parsed.type = typeid(unsigned long);
            parsed.value = value;
        }
        else {
            // note: this could be an enum
            parsed.type = typeid(string);
            parsed.value = value;
        }

        if ( parsed.type != typeid(string))
            if ( parsed.value.find('.') != string::npos)
                parsed.type = typeid(double);
    }
    else
        parsed.comment = line + parsed.comment;
}

void file_storage::write_setting(ofstream & out, const info & parsed) {
    out << parsed.comment << '\n';
    // see if parsed.name is empty - if so, there's no settting to write
    if ( !parsed.name.empty()) {
        out << parsed.name << '=' ;
        if ( parsed.type == typeid(string) || parsed.type == typeid(variant)) 
            out << '"' << detail::escape_string(parsed.value) << '"';
        else if ( parsed.type != typeid(bool))
            out << parsed.value;
        else
            out << ((parsed.value != TTEXT("0")) ? TTEXT("true") : TTEXT("false"));
        out << ' ';
    }
}


void file_storage::get_setting( const string & name, string & value, typeinfo& type) const {
    info_coll::const_iterator found = m_infos.find(name);
    if ( found != m_infos.end() ) {
        value = found->second.value;
        type = found->second.type;
    }
    else {
        value.clear();
        type = typeid(string);
        bool has_default;
        parent()->get_default_value( full_setting_name(name), value, type, has_default);
        if ( !has_default)
            set_error(err::bad_setting_name, TTEXT("cannot get setting ") + full_setting_name(name) );
    }
}

namespace {
    // we only need 4 types: int, unsigned, double, bool, string
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
            return typeid(double);
        else if ( type == typeid(float) )
            return typeid(double);
        else if ( type == typeid(bool) )
            return typeid(bool);
        else if ( type == typeid(string) )
            return typeid(string);
        else if ( type == typeid(variant) )
            return typeid(variant);

        return typeid(variant); // unknown
    }
}


void file_storage::set_setting( const string & name, const string & value, const typeinfo&type) {
    info_coll::iterator found = m_infos.find(name);
    if ( found != m_infos.end() ) {
        // we have this setting
        if ( found->second.value != value) {
            found->second.value = value;
            m_is_dirty = true;
        }
    }
    else {
        if ( m_open != open_read_only) {
            info new_sett;
            new_sett.idx = (int)m_infos.size() ;
            new_sett.name = name;
            new_sett.value = value;
            new_sett.type = friendly_type(type);
            m_infos[ name ] = new_sett;
            m_is_dirty = true;
        }
        else {
            set_error(err::bad_setting_name, TTEXT("cannot set setting (file is readyonly)") + full_setting_name(name) );
        }
    }

    if ( m_is_dirty && (m_save == save_each_modify) )
        save();
}
void file_storage::enum_settings( std::map<string,string> & values) const {
    values.clear();
    for ( info_coll::const_iterator b = m_infos.begin(), e = m_infos.end(); b != e; ++b)
        values[ b->first ] = b->second.value;
}

}
