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

#if !defined(SS_TS_H)
#define SS_TS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#undef SS_IS_THREAD_SAFE
#undef SS_TS_WIN
#undef SS_TS_BOOST

#ifdef SETTING_NOT_THREAD_SAFE
// not thread safe
#elif defined(SETTING_THREAD_SAFE_USE_BOOST)
#define SS_IS_THREAD_SAFE
#define SS_TS_BOOST
#elif defined(SETTING_THREAD_SAFE_USE_WIN)
#define SS_IS_THREAD_SAFE
#define SS_TS_WIN
#else
// default - thread safe, use Win Threads
#define SS_IS_THREAD_SAFE
#define SS_TS_WIN
#endif

// thread-safe issues.
namespace ss { namespace detail {

#ifdef SS_IS_THREAD_SAFE

#ifdef SS_TS_WIN
    // on Win32, use critical section
}}
#include <windows.h>
namespace ss { namespace detail {

class critical_section {
    critical_section & operator = ( const critical_section & Not_Implemented);
    critical_section( const critical_section & From);
public:
    critical_section() {    InitializeCriticalSection( GetCriticalSectionPtr() ); }
    void Lock() {           EnterCriticalSection( GetCriticalSectionPtr()); }
    void Unlock() {         LeaveCriticalSection( GetCriticalSectionPtr()); }
    operator LPCRITICAL_SECTION() const { return GetCriticalSectionPtr(); }
private:
    LPCRITICAL_SECTION GetCriticalSectionPtr() const { return &m_cs; }
private:
    // the critical section itself
    mutable CRITICAL_SECTION m_cs;
};



/*
    allows automatic Locking/ Unlocking of a Resource,
    protected by a Critical Section:
*/
class scoped_lock
{
    scoped_lock operator=( scoped_lock & Not_Implemented);
    scoped_lock( const scoped_lock & Not_Implemented);
public:
    scoped_lock( critical_section & csResource) : m_csResource( csResource) {
        m_csResource.Lock();
    }
    ~scoped_lock() {
        m_csResource.Unlock();
    }
private:
    critical_section & m_csResource;
};



#elif defined(SS_TS_BOOST)
#include <boost/thread/recursive_mutex.hpp>
typedef boost::recursive_mutex critical_section;
typedef boost::recursive_mutex::scoped_lock scoped_lock;
#else
#error Invalid thread safety option
#endif


#else
// not thread-safe

struct critical_section {};

class scoped_lock {
    scoped_lock( const scoped_lock&);
    void operator=( const scoped_lock&);
public:
    scoped_lock( const critical_section &) {}
    ~scoped_lock() {}
};

#endif

}}

#endif 
