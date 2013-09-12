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

#ifndef SS_FILESTORAGE_H
#define SS_FILESTORAGE_H

#pragma once

#include "ss/fwd.h"
#include "ss/setting_storage.h"

namespace ss {



// when thread-safety on, you can have it run on a dedicated thread (to write the settings). Or,
// otherwise, rewrite each time a setting is set.
class file_storage : public setting_storage
{
public:
    typedef enum open_type {
	    // opens the file as writable
        open_writable,
        // treats the file as read-only - does not write to it
        open_read_only
    };

    typedef enum save_type {
        // saves settings only when requested to (when you save() the configuration = a call to configuration::save)
        save_on_request,
        // saves each time a modification takes place
        save_each_modify,
        // saves at a given interval, on a dedicated thread (like, every second)
        //
        // IMPORTANT: At this time, only available for windows
        save_at_interval
    };

    file_storage(const std::string & file_name, open_type open = open_writable, save_type save = save_at_interval, int interval_ms = 1000);
    ~file_storage(void);

    void save() ;
    void get_setting( const string & name, string & value, typeinfo&) const ;
    void set_setting( const string & name, const string & value, const typeinfo&) ;
    void enum_settings( std::map<string,string> & values) const ;

private:
    void load();

private:
    std::string m_file_name;
    open_type m_open;
    save_type m_save;
    int m_interval_ms;

    bool m_is_dirty;

    // information about ONE setting
    struct info {
        info() : idx(0) {}
        string name;
        string value;
        string comment;
        typeinfo type;
        // the setting's index (this is useful when saving, to preserve the original layout of the file)
        int idx;
    };
    typedef std::map<string, info> info_coll;
    info_coll m_infos;

    static void read_setting(string line, info & parsed);
    static void write_setting(ofstream & out, const info & parsed);


    // FIXME(later) at this time save_at_interval is available only for windows
#ifdef SS_TS_WIN
    static DWORD WINAPI save_thread(LPVOID);
    HANDLE m_dedicated_thread;
    bool m_is_dedicated_thread_running;
#endif

};


}

#endif
