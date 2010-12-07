/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	application.cxx - this file is integral part of `hector' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "application.hxx"
#include "applicationserver.hxx"
#include "setup.hxx"
#include "cgi.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;

namespace hector
{

HApplication::HApplication( void )
	: _dOM(), _name(), _defaultSecurityContext(), _sessions()
	{
	}

HApplication::~HApplication( void )
	{
	out << "Application `" << _name << "' unloaded." << endl;
	}

void HApplication::load( HString const& name, HString const& path )
	{
	M_PROLOG
	static char const* const INTERFACE_FILE = "interface.xml";
	static char const* const TOOLKIT_FILE = "toolkit.xml";
	_name = name;
	HStringStream interface( path );
	HStringStream toolkit( path );
	hcore::log( LOG_TYPE::INFO ) << "Loading application `" << _name << "'." << endl;
	interface << "/" << _name << "/" << INTERFACE_FILE;
	toolkit << "/" << _name << "/" << TOOLKIT_FILE;
	HFSItem app( interface.string() );
	_defaultSecurityContext._user = app.get_user();
	_defaultSecurityContext._group = app.get_group();
	_defaultSecurityContext._mode = static_cast<ACCESS::enum_t>( app.get_permissions() );
	hcore::log( LOG_TYPE::INFO ) << "Using `" << interface.string() << "' as application template." << endl;
	_dOM.init( HStreamInterface::ptr_t( new HFile( interface.string(), HFile::OPEN::READING ) ) );
	hcore::log( LOG_TYPE::INFO ) << "Using `" << toolkit.string() << "' as a toolkit library." << endl;
	_dOM.apply_style( toolkit.string() );
	_dOM.parse();
	do_load();
	cgi::consistency_check( _dOM.get_root() );
	return;	
	M_EPILOG
	}

void HApplication::do_load( void )
	{
	out << __PRETTY_FUNCTION__ << endl;
	}

void HApplication::do_handle_logic( ORequest& req, OSession& )
	{
	M_PROLOG
	out << __PRETTY_FUNCTION__ << endl;
	static char const LOGIC_PATH[] = "/html/logic/";
	HXml::HNodeProxy logic = _dOM.get_element_by_path( LOGIC_PATH );
	if ( !! logic )
		cgi::make_cookies( logic, req );
	return;
	M_EPILOG
	}

void HApplication::do_generate_page( ORequest const& req, OSession const& session_ )
	{
	M_PROLOG
	out << __PRETTY_FUNCTION__ << endl;
	cgi::default_t defaults;
	if ( !! dom().get_root() )
		cgi::waste_children( dom().get_root(), req, defaults );
	if ( !! dom().get_root() )
		cgi::apply_acl( dom().get_root(), req, _defaultSecurityContext, session_ );
	if ( !! dom().get_root() )
		cgi::mark_children( dom().get_root(), req, defaults, dom() );
	if ( !! dom().get_root() )
		cgi::move_children( dom().get_root(), req, dom() );
	if ( !! dom().get_root() )
		cgi::expand_autobutton( dom().get_root(), req );
	return;
	M_EPILOG
	}

void HApplication::generate_page( ORequest const& req, OSession const& session_ )
	{
	M_PROLOG
	out << __PRETTY_FUNCTION__ << endl;
	do_generate_page( req, session_ );
	if ( !! dom().get_root() )
		_dOM.save( req.socket() );
	return;
	M_EPILOG
	}

void HApplication::handle_logic( ORequest& req, OSession& session_ )
	{
	M_PROLOG
	out << __PRETTY_FUNCTION__ << endl;
	do_handle_logic( req, session_ );
	return;
	M_EPILOG
	}

HXml& HApplication::dom( void )
	{
	return ( _dOM );
	}

HApplication::sessions_t const& HApplication::sessions( void ) const
	{
	return ( _sessions );
	}

HApplication::sessions_t& HApplication::sessions( void )
	{
	return ( _sessions );
	}

}

