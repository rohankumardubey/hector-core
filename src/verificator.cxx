/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  verificator.cxx - this file is integral part of `hector' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <yaal/tools/hstringstream.hxx>

#include "verificator.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace hector {

HVerificatorInterface::HVerificatorInterface( cgi::params_t const& params_ )
	: _params( params_ ) {
	return;
}

bool HVerificatorInterface::verify( ORequest const& req_ ) const {
	return ( do_verify( req_ ) );
}

HHuginnVerificator::HHuginnVerificator(
	yaal::hcore::HString const& code_,
	cgi::params_t const& params_
) : HVerificatorInterface( params_ )
	, _huginn() {
	M_PROLOG
	HHuginn::ptr_t h( make_pointer<HHuginn>() );
	HStringStream s( code_ );
	h->load( s );
	h->preprocess();
	if ( h->parse() && h->compile() ) {
		_huginn = h;
	} else {
		throw HRuntimeException( h->error_message() );
	}
	return;
	M_EPILOG
}

bool HHuginnVerificator::do_verify( ORequest const& ) const {
	return ( false );
}

HSQLVerificator::HSQLVerificator(
	yaal::hcore::HString const& code_,
	cgi::params_t const& params_
) : HVerificatorInterface( params_ )
	, _code( code_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HSQLVerificator::do_verify( ORequest const& ) const {
	return ( false );
}

}

