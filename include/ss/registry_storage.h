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

// registry_storage.h: interface for the registry_storage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SS_REGISTRY_STORAGE_H)
#define SS_REGISTRY_STORAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable :4786) // msvc

#include "setting_storage.h"

namespace ss {

class registry_storage  : public setting_storage
{
public:
	registry_storage( const string & root);
	virtual ~registry_storage();

	void save();
    void get_setting( const string & name, string & value, typeinfo&) const ;
    void set_setting( const string & name, const string & value, const typeinfo&) ;
    void enum_settings( std::map<string,string> & values) const ;

private:
    string m_root;
};


} // namespace ss

#endif 
