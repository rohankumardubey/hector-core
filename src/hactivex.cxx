/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	oactivex.cxx - this file is integral part of `hector' project.

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

#include <yaal/hcore/hfile.hxx>
#include <yaal/tools/hstringstream.hxx>
M_VCSID( "$Id: " __ID__ " $" )
#include "hactivex.hxx"
#include "setup.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::dbwrapper;

namespace hector {

HActiveX HActiveX::get_instance(
	HString const& name_,
	HApplication::MODE mode_,
	HString const& path_,
	HDataBase::ptr_t db_
) {
	M_PROLOG
	static char const* const SYMBOL_FACTORY = "factory";
	static char const* const ATTRIBUTE_ACTIVEX = "activex";
	HStringStream activex( path_ );
	HPlugin::ptr_t activeX( make_pointer<HPlugin>() );
	activex << "/" << name_ << "/" << ATTRIBUTE_ACTIVEX;
	HApplication::ptr_t app;
	out << "Trying path: `" << activex.raw() << "' for activex: `" << name_ << "'" << endl;
	activeX->load( activex.raw() );
	M_ASSERT( activeX->is_loaded() );
	out << "activex nest for `" << name_ << "' loaded" << endl;
	typedef HApplication::ptr_t ( *factory_t )( void );
	factory_t factory;
	activeX->resolve( SYMBOL_FACTORY, factory );
	M_ASSERT( factory );
	out << "activex factory for `" << name_ << "' connected" << endl;
	app = factory();
	if ( ! app ) {
		throw HActiveXException( "invalid activex" );
	}
	HActiveX proc( activex.raw() );
	proc._application = app;
	proc._activeX = activeX;
	app->set_mode( mode_ );
	app->set_db( db_);
	app->init();
	app->load( name_, path_ );
	return ( proc );
	M_EPILOG
}

void HActiveX::reload_binary( void ) {
	M_PROLOG
	_activeX->unload();
	_activeX->load( _binaryPath );
	return;
	M_EPILOG
}

void HActiveX::handle_logic( ORequest& req_, HSession& session_ ) {
	M_PROLOG
	_application->handle_logic( req_, session_ );
	return;
	M_EPILOG
}

void HActiveX::generate_page( ORequest const& req_, HSession const& session_ ) {
	M_PROLOG
	_application->generate_page( req_, session_ );
	return;
	M_EPILOG
}

HApplication::sessions_t& HActiveX::sessions( void ) {
	return ( _application->sessions() );
}

HApplication::sessions_t const& HActiveX::sessions( void ) const {
	return ( _application->sessions() );
}

HApplication::MODE HActiveX::get_mode( void ) const {
	return ( _application->get_mode() );
}

}

