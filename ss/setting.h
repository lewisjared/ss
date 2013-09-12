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

//
//////////////////////////////////////////////////////////////////////

#if !defined(SS_SETTING_H)
#define SS_SETTING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ss/fwd.h"
#include "ss/configuration.h"

#if defined(SS_USE_BOOST_ENUM)
#include <boost/type_traits/is_enum.hpp>
#endif

namespace ss {

    namespace detail {
#if defined(SS_USE_BOOST_ENUM)
        template<bool is_enum> struct helper {};

        template< class type> void from_stream_impl( istringstream & in, type & val, helper<false> ) {
            in >> val;
        }

        template< class type> void from_stream_impl( istringstream & in, type & val, helper<true> ) {
            int n = 0;
            in >> n;
            val = (type)n;
        }

        template< class type> void from_stream( istringstream & in, type & val) {
            from_stream_impl< type>(in, val, helper< boost::is_enum<type>::value>() );
        }
#else
        template< class type> void from_stream( istringstream & in, type & val) {
            in >> val;
        }
#endif

        // special case for string
        inline void from_stream( istringstream & in, string & val) {
            val = in.str();
        }
    }


/** 
    represents a setting, with a known type, like
    setting<some_type>("some_name")
*/
template<class type> class forced_setting_t {
    typedef forced_setting_t<type> self_type;
public:

    forced_setting_t( const string & name, configuration & conf = configuration::def(), bool is_const = false ) 
        : m_conf( conf) {
        m_conf.resolve_name( name, m_place, m_name, 
            (is_const ? configuration::resolve_read_only : configuration::resolve_writable) );
    }
    ~forced_setting_t() {}

    // easy conversion
    operator type() const { return get(); }

    self_type & operator=( const type & val) {
        set( val);
        return *this;
    }

    // allow things like:
    // setting<long>("app.first") = setting<int>("app.second");
    // (note: first type is long, second type is int)
    template< class other_type> self_type & operator=( const forced_setting_t<other_type> & other) {
        set( other.get() );
        return *this;
    }
    // allow things like:
    // setting<int>("app.first") = setting<int>("app.second");
    self_type & operator=( const self_type & other) {
        set( other.get() );
        return *this;
    }

private:
    type get() const {
        string val_str;
        typeinfo set_type = typeid(type);
        m_conf.get_setting( m_place, m_name, val_str, set_type);
        istringstream in( val_str);
        type val = type();
        detail::from_stream( in, val);
        if ( in.fail() ) {
            m_conf.get_error_handler()( err::cannot_convert, TTEXT("value cannot be converted to underlying type") );
        }
        return val;
    }

    void set( const type & val) {
        ostringstream out;
        out << val;
        m_conf.set_setting( m_place, m_name, out.str(), typeid(type) );
    }

private:
    // where this setting is persisted
    string m_place;
    // the real name of the setting
    string m_name;
    // the configuration this setting_t belongs to
    configuration & m_conf;

};



/** 
    represents a setting, with an unknown type, like
    setting("some_name")

    The type of the setting is known only when used:

    // get
    string name = setting("user.name");             // type string
    string pass = setting("user.passw");            // type string
    long retries = setting("app.retries");          // type long

    // set
    setting("app.retries") = 5;                     // type int
    setting("user.name") = "John";                  // type string
    setting("user.passw") = "secret";               // type string


    @remarks

    When you need to convert a setting to a C-string, use the .c_str() function.
    However, use this with caution, since the returned value is temporary and will be destroyed 
    when the setting goes out of scope. So, you should use this only to pass a value to a function
    (and the returned value will be destroyed after the function ends)

    Example:
    // ... function prototype
    details GetUserDetails(const char * username); 

    //
    // good usage

    details d = GetUserDetails(setting("user.name").c_str() );

    // bad usage
    const char * username = setting("user.name").c_str();
    // ... at this point, username points to uninitialized memory
    details d = GetUserDetails(username);

*/
class simple_setting {
    typedef simple_setting self_type;
public:

    simple_setting( const string & name, configuration & conf = configuration::def(), bool is_const = false ) 
        : m_conf( conf) {
        m_conf.resolve_name( name, m_place, m_name, 
            (is_const ? configuration::resolve_read_only : configuration::resolve_writable) );
    }
    ~simple_setting() {}

    // easy conversion
    template<class type> operator type() const { return get<type>(); }

    template<class type> self_type & operator=( const type & val) {
        set<type>( val);
        return *this;
    }

    // allow things like:
    // setting("app.first") = setting("app.second");
    self_type & operator=( const self_type & other) {
        string s = other; // convert the other setting to string
        set<string>( s);
        return *this;
    }

private:
    template<class type> type get() const {
        string val_str;
        typeinfo set_type = typeid(type);
        m_conf.get_setting( m_place, m_name, val_str, set_type);
        istringstream in( val_str);
        type val = type();
        detail::from_stream( in, val);
        if ( in.fail() ) {
            m_conf.get_error_handler()( err::cannot_convert, TTEXT("value cannot be converted to underlying type") );
        }
        return val;
    }

    template<class type> void set( const type & val) {
        ostringstream out;
        out << val;
        m_conf.set_setting( m_place, m_name, out.str(), typeid(type) );
    }

public:
    const char_t * c_str() const {
        m_value = get<string>();
        return m_value.c_str();
    }

    const configuration & conf() const { return m_conf; }
    const string & place() const { return m_place; }
    const string & name() const { return m_name; }

private:
    // where this setting is persisted
    string m_place;
    // the real name of the setting
    string m_name;
    // the value of the setting - as a string, used only if you call c_str()
    mutable string m_value;
    // the configuration this setting_t belongs to
    configuration & m_conf;

};



template< class t> inline forced_setting_t<t> setting( const string & name, configuration & conf = configuration::def()) {
    return forced_setting_t<t>( name, conf);
}

inline simple_setting setting( const string & name, configuration & conf = configuration::def()) {
    return simple_setting( name, conf);
}








struct enum_ {
    enum_(const simple_setting & sett) : m_delegate(sett) {}

    template<class type> operator type() const { 
        string str_value = m_delegate.c_str();
        return (type)str_to_enum( typeid(type), str_value);
    }

    template<class type> enum_ & operator=( const type & val) {
        string str_value = enum_to_str( typeid(type), val);
        m_delegate = str_value;
        return *this;
    }

private:
    int str_to_enum( const typeinfo & type, const string & str_value) const ;
    string enum_to_str( const typeinfo & type, int enum_value) const ;

private:
    simple_setting m_delegate;
};



}

// needs to be included after the implentation of setting classes
#include "ss/const_.h"
#include "ss/array.h"

#endif
