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

#if !defined(SS_CONST__H)
#define SS_CONST__H

#if !defined(SS_SETTING_H)
#error Don't include directly. Include ss/setting.h instead
#endif

namespace ss {

struct force_setting_to_be_const {
    force_setting_to_be_const(const string & name, configuration & conf) {
        conf.force_setting_to_be_const(name);
    }
};


/** 
    represents a setting, with a known type, like
    const_<some_type>("some_name")
*/
template<class type> class forced_const_t {
    typedef forced_const_t<type> self_type;
    self_type & operator=( const type & not_allowed) ;
    self_type & operator=( const self_type & not_allowed) ;
public:

    forced_const_t( const string & name, configuration & conf = configuration::def() ) 
            : m_forcer(name, conf), 
              m_delegate(name, conf, true /* is const */) {
    }
    ~forced_const_t() {}

    // easy conversion
    operator type() const { return get(); }

private:
    type get() const {
        return static_cast<type>(m_delegate);
    }

private:
    // this needs CONSTRUCTED before m_delegate, to mark the setting as const
    force_setting_to_be_const m_forcer;
    forced_setting_t<type> m_delegate;
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
class simple_const {
    typedef simple_const self_type;
    self_type & operator=( const self_type & not_allowed) ;
public:

    simple_const( const string & name, configuration & conf = configuration::def() ) 
        : m_forcer(name, conf),
         m_delegate(name, conf, true /* is const */) {
    }
    ~simple_const() {}

    // easy conversion
    template<class type> operator type() const { return get<type>(); }

private:
    template<class type> type get() const {
        return static_cast<type>(m_delegate);
    }

public:
    const char_t * c_str() const {
        return m_delegate.c_str();
    }

private:
    // this needs CONSTRUCTED before m_delegate, to mark the setting as const
    force_setting_to_be_const m_forcer;
    simple_setting m_delegate;
};



template< class t> inline forced_const_t<t> const_( const string & name, configuration & conf = configuration::def()) {
    return forced_const_t<t>( name, conf);
}

inline simple_const const_( const string & name, configuration & conf = configuration::def()) {
    return simple_const( name, conf);
}





}

#endif
