/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	application.hxx - this file is integral part of `hector' project.

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

#ifndef APPLICATION_HXX_INCLUDED
#define APPLICATION_HXX_INCLUDED

#include <yaal/tools/hxml.hxx>
#include <yaal/tools/hplugin.hxx>
#include <yaal/dbwrapper/hdatabase.hxx>
#include "server.hxx"
#include "securitycontext.hxx"
#include "form.hxx"
#include "cgi.hxx"

namespace hector {

struct HSession;
class HApplicationServer;

class HApplication {
public:
	enum class MODE {
		GET,
		POST
	};
	typedef HApplication this_type;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HSession> sessions_t;
	typedef yaal::hcore::HMap<yaal::hcore::HString, HForm::ptr_t> forms_t;
private:
	yaal::tools::HXml _dom;
	yaal::hcore::HString _id;
	yaal::hcore::HString _code;
	yaal::hcore::HString _name;
	yaal::hcore::HString _dsn;
	yaal::hcore::HString _tableUser;
	yaal::hcore::HString _columnLogin;
	yaal::hcore::HString _columnPassword;
	yaal::hcore::HString _authQuery;
	OSecurityContext _defaultSecurityContext;
	sessions_t _sessions;
	yaal::dbwrapper::HDataBase::ptr_t _db;
	forms_t _forms;
	MODE _mode;
public:
	typedef yaal::hcore::HPointer<HApplication> ptr_t;
	HApplication();
	virtual ~HApplication( void );
	void load(
		HApplicationServer*,
		yaal::hcore::HString const&,
		yaal::hcore::HString const&,
		yaal::hcore::HString const&,
		yaal::hcore::HString const&,
		MODE
	);
	void handle_logic( ORequest&, HSession& );
	void generate_page( ORequest const&, HSession const& );
	sessions_t& sessions( void );
	sessions_t const& sessions( void ) const;
	yaal::dbwrapper::HDataBase::ptr_t db( void );
	void add_form( forms_t::value_type&& );
	void fill_form( yaal::hcore::HString const&, HSession const& );
	OSecurityContext const& get_default_security_context( void ) const;
	MODE get_mode( void ) const;
	yaal::hcore::HString const& id( void ) const;
	yaal::hcore::HString const& name( void ) const;
	yaal::hcore::HString const& code( void ) const;
	void init( void );
protected:
	virtual void do_init( void );
	virtual void do_load( void );
	virtual void do_handle_logic( ORequest&, HSession& ) = 0;
	virtual void do_generate_page( ORequest const&, HSession const& ) = 0;
	virtual yaal::dbwrapper::HDataBase::ptr_t do_db( void );
	bool handle_auth( ORequest&, HSession& );
	virtual bool do_handle_auth( ORequest&, HSession& );
	bool handle_forms( ORequest&, HSession& );
	virtual bool do_handle_forms( ORequest&, HSession& );
	yaal::tools::HXml& dom( void );
	void set_input_data( void );
private:
	HApplication( HApplication const& );
	HApplication& operator = ( HApplication const& );
};

typedef yaal::hcore::HExceptionT<HApplication> HApplicationException;

}

#endif /* not APPLICATION_HXX_INCLUDED */

