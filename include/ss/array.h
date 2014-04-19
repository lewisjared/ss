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

#ifndef SS_ARRAY_H
#define SS_ARRAY_H

#pragma once

#include <string>
#include <utility>

namespace ss { 

struct array_stl {
    array_stl(const simple_setting & a) : m_array(a) {}

    template<class array_type> operator array_type() const {
        typedef typename array_type::value_type value_type;
        array_type result;

        string prefix = detail::full_setting_name( m_array.place(), m_array.name());
        int count = setting( prefix + TTEXT(".count") );
        for ( int i = 0; i < count ; ++i) {
            ostringstream out;
            out << prefix << TTEXT(".elems.") << (i+1) ;
            value_type val = setting(out.str());
            result.push_back(val);
        }
        return result;
    }

    template<class array_type> array_stl &operator=(const array_type & src) {
        typedef typename array_type::value_type value_type;
        typedef typename array_type::const_iterator const_iterator;
        string prefix = detail::full_setting_name( m_array.place(), m_array.name());

        int count = (int)src.size();
        setting( prefix + TTEXT(".count")) = count;
        int i = 0;
        for ( const_iterator b = src.begin(), e = src.end(); b != e; ++b) {
            ostringstream out;
            out << prefix << TTEXT(".elems.") << ++i ;
            setting(out.str()) = *b;
        }
            
        return *this;
    }


private:
    simple_setting m_array;
};

struct array_c_like {
    array_c_like(const simple_setting & a, int count) : m_array(a), m_count(count) {}

    template<class value_type> void set_elem_at_idx(value_type * p, int idx, const string & name) {
            value_type val = setting(name);
            p[idx] = val;
    }

    template<class array_type> operator array_type () const {
        array_type result;

        string prefix = detail::full_setting_name( m_array.place(), m_array.name());
        int count = setting( prefix + TTEXT(".count") );
        for ( int i = 0; i < count ; ++i) {
            ostringstream out;
            out << prefix << TTEXT(".elems.") << (i+1) ;
            set_elem_at_idx(result, i, out.str());
        }

        return result;
    }

#if 0
    template<class elem_type,int n> operator elem_type[n] () const {
        elem_type result[n];
        string prefix = detail::full_setting_name( m_array.place(), m_array.name());
        int count = setting( prefix + TTEXT(".count") );
        for ( int i = 0; i < count ; ++i) {
            ostringstream out;
            out << prefix << TTEXT(".elems.") << (i+1) ;
            set_elem_at_idx(result, i, out.str());
        }
        return result;
    }
#endif


    template<class array_type> void operator=(const array_type * src) {

        string prefix = detail::full_setting_name( m_array.place(), m_array.name());
        setting( prefix + TTEXT(".count")) = m_count;
        for ( int i = 0; i < m_count; ++i) {
            ostringstream out;
            out << prefix << TTEXT(".elems.") << (i+1) ;
            setting(out.str()) = src[i];
        }
    }

private:
    simple_setting m_array;
    int m_count;
};


inline array_stl array(const simple_setting & s) { return array_stl(s); }
inline array_c_like array(const simple_setting & s, int count) { return array_c_like(s, count); }



struct coll {
    coll(const simple_setting & c) : m_coll(c) {}

    template<class coll_type> operator coll_type() const {
        typedef typename coll_type::mapped_type value_type;
        typedef typename coll_type::key_type key_type;
        coll_type result;

        string prefix = detail::full_setting_name( m_coll.place(), m_coll.name());
        int count = setting( prefix + TTEXT(".count") );
        for ( int i = 0; i < count ; ++i) {
            ostringstream out_val;
            ostringstream out_key;
            out_val << prefix << TTEXT(".elems.") << (i+1) << "_val";
            out_key << prefix << TTEXT(".elems.") << (i+1) << "_key";
            value_type val = setting(out_val.str());
            key_type key = setting(out_key.str());
            result.insert( std::make_pair(key, val) );
        }
        return result;
    }

    template<class coll_type> coll &operator=(const coll_type & src) {
        typedef typename coll_type::mapped_type value_type;
        typedef typename coll_type::key_type key_type;
        typedef typename coll_type::const_iterator const_iterator;
        string prefix = detail::full_setting_name( m_coll.place(), m_coll.name());

        int count = (int)src.size();
        setting( prefix + TTEXT(".count")) = count;
        int i = 0;
        for ( const_iterator b = src.begin(), e = src.end(); b != e; ++b) {
            ostringstream out_val;
            ostringstream out_key;
            out_val << prefix << TTEXT(".elems.") << (i+1) << "_val";
            out_key << prefix << TTEXT(".elems.") << (i+1) << "_key";

            setting(out_key.str()) = b->first;
            setting(out_val.str()) = b->second;
            ++i;
        }
            
        return *this;
    }


private:
    simple_setting m_coll;
};


}

#endif
