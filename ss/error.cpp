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


#include "ss/fwd.h"
#include <assert.h>
#include <stdexcept>
#include "ss/util.h"
#include <iostream>

#if defined(WIN32) && !defined(NDEBUG)
#include <windows.h>

#ifdef SS_USE_UNICODE
    #define OutputDebugString_ OutputDebugStringW
#else
    #define OutputDebugString_ OutputDebugStringA
#endif

#endif


namespace ss { namespace err {

void do_ignore(int, const string&) {
}

void do_log_to_console(int error_code, const string& error) {
#ifdef SS_USE_UNICODE
    std::wcout << TTEXT("SS: ") << error_code << TTEXT("/") << error << TTEXT("\n");
#else
    std::cout << TTEXT("SS: ") << error_code << TTEXT("/") << error << TTEXT("\n");
#endif
}

#if defined(WIN32)
void do_log_do_dbg_wnd(int error_code, const string& error) {
    ostringstream out;
    out << TTEXT("SS: ") << error_code << TTEXT("/") << error << TTEXT("\n");
    OutputDebugString_( out.str().c_str());
}
#else
void do_log_do_dbg_wnd(int, const string&) {
}
#endif


void do_assert(int error_code, const string& error) {
    // check the error code and error
    assert( !error_code && (int)error.size() < -1);
}

void do_throw(int, const string& error) {
    throw std::runtime_error( ::ss::detail::narrow(error) );
}

}}
