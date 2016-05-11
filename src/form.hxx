/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	form.hxx - this file is integral part of `hector' project.

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

#ifndef HECTOR_FORM_HXX_INCLUDED
#define HECTOR_FORM_HXX_INCLUDED 1

#include <yaal/hcore/harray.hxx>
#include <yaal/hcore/hresource.hxx>
#include <yaal/dbwrapper/hcruddescriptor.hxx>

#include "verificator.hxx"

namespace hector {

class HForm {
public:
	typedef yaal::hcore::HResource<HForm> ptr_t;
	typedef yaal::hcore::HResource<HVerificatorInterface> verificator_t;
	struct OInput {
		yaal::hcore::HString* _data;
		ACCESS::mode_t _mode;
		OInput( yaal::hcore::HString* data_, ACCESS::mode_t mode_ )
			: _data( data_ )
			, _mode( mode_ ) {
			return;
		}
		OInput( OInput const& ) = default;
		OInput& operator = ( OInput const& ) = default;
	};
private:
	typedef yaal::hcore::HLookupMap<yaal::hcore::HString, OInput> inputs_t;
	yaal::hcore::HString _table;
	yaal::hcore::HString _filter;
	inputs_t _inputs;
	verificator_t _verificator;
	yaal::dbwrapper::HCRUDDescriptor _crud;
	HApplication& _application;
public:
	HForm( HApplication&, yaal::hcore::HString const&, yaal::hcore::HString const& );
	void fill( HSession const& );
	void add_input( yaal::hcore::HString const&, yaal::hcore::HString&, ACCESS::mode_t );
	void set_verificator(
		HVerificatorInterface::TYPE,
		yaal::hcore::HString const&,
		cgi::params_t const&
	);
	bool verify( ORequest&, HSession& );
	HApplication& app( void ) const {
		return ( _application );
	}
	void finalize( void );
private:
	HForm( HForm const& ) = delete;
	HForm& operator = ( HForm const& ) = delete;
};

}

#endif /* #ifndef HECTOR_FORM_HXX_INCLUDED */

