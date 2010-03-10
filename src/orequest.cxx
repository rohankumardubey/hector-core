/*
---           `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	orequest.cxx - this file is integral part of `hector' project.

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
#include "orequest.hxx"
#include "setup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace hector
{

ORequest::ORequest( HSocket::ptr_t a_oSocket )
	: f_oSocket( a_oSocket ),
	f_oEnvironment( new dictionary_t() ),
	f_oGET( new dictionary_t() ),
	f_oPOST( new dictionary_t() ),
	f_oCookies( new dictionary_t() ),
	f_oJar( new dictionary_t() )
	{
	}

ORequest::ORequest( ORequest const& req )
	: f_oSocket( req.f_oSocket ),
	f_oEnvironment( req.f_oEnvironment ),
	f_oGET( req.f_oGET ), f_oPOST( req.f_oPOST ),
	f_oCookies( req.f_oCookies ), f_oJar( req.f_oJar )
	{
	}

ORequest& ORequest::operator = ( ORequest const& req )
	{
	if ( &req != this )
		{
		f_oSocket = req.f_oSocket;
		f_oEnvironment = req.f_oEnvironment;
		f_oGET = req.f_oGET;
		f_oPOST = req.f_oPOST;
		f_oCookies = req.f_oCookies;
		f_oJar = req.f_oJar;
		}
	return ( *this );
	}

void ORequest::update( HString const& key, HString const& value, origin_t const& origin )
	{
	M_PROLOG
	dictionary_t* dict = NULL;
	switch ( origin.value() )
		{
		case ( ORIGIN::ENV ):    dict = &*f_oEnvironment; break;
		case ( ORIGIN::POST ):   dict = &*f_oPOST;        break;
		case ( ORIGIN::GET ):    dict = &*f_oGET;         break;
		case ( ORIGIN::COOKIE ): dict = &*f_oCookies;     break;
		case ( ORIGIN::JAR ):    dict = &*f_oJar;     break;
		default:
			M_ASSERT( ! "bad origin" );
		}
	(*dict)[ key ] = value;
	return;
	M_EPILOG
	}

bool ORequest::lookup( HString const& key, HString& value, origin_t const& origin ) const
	{
	M_PROLOG
	dictionary_t::const_iterator it = f_oEnvironment->find( key );
	bool bFound = false;
	( ! bFound ) && ( !!( origin & ORIGIN::ENV ) )
		&& ( bFound = ( ( it = f_oEnvironment->find( key ) ) != f_oEnvironment->end() ) )
		&& ( !! ( value = it->second ) );
	( ! bFound ) && ( !!( origin & ORIGIN::POST ) )
		&& ( bFound = ( ( it = f_oPOST->find( key ) )        != f_oPOST->end() ) )
		&& ( !! ( value = it->second ) );
	( ! bFound ) && ( !!( origin & ORIGIN::GET ) )
		&& ( bFound = ( ( it = f_oGET->find( key ) )         != f_oGET->end() ) )
		&& ( !! ( value = it->second ) );
	( ! bFound ) && ( !!( origin & ORIGIN::COOKIE ) )
		&& ( bFound = ( ( it = f_oCookies->find( key ) )     != f_oCookies->end() ) )
		&& ( !! ( value = it->second ) );
	return ( ! bFound );
	M_EPILOG
	}

void ORequest::decompress_jar( yaal::hcore::HString const& app )
	{
	M_PROLOG
	static int const MAX_COOKIE_SIZE = 4096;
	static int const MAX_COOKIES_PER_PATH = 20;
	static int const SIZE_SIZE = 4; /* 12ab */
	HString buf;
	HString& properName = buf;
	HString jar( MAX_COOKIES_PER_PATH * MAX_COOKIE_SIZE, true );
	int cookieNo = 0;
	int size = 0;
	jar = "";
	for ( dictionary_t::iterator it = f_oJar->begin(); it != f_oJar->end(); ++ it )
		{ 
		properName.format( "%s%02d", app.raw(), cookieNo );
		if ( it->first != properName )
			continue;
		if ( ! cookieNo )
			{
			size = lexical_cast<int>( it->second.left( SIZE_SIZE ) );
			jar += it->second.mid( SIZE_SIZE );
			}
		else
			jar += it->second;
		++ cookieNo;
		}
	M_ENSURE( jar.get_length() == size );
	jar = base64::decode( jar );
	f_oJar->clear();
	cookieNo = 0;
	HString name;
	HTokenizer t( jar, "\001", HTokenizer::SKIP_EMPTY );
	for ( HTokenizer::HIterator it = t.begin(), endIt = t.end(); it != endIt; ++ it )
		{
		int long sepIdx = (*it).find( "=" );
		name = (*it).left( sepIdx >= 0 ? sepIdx : meta::max_signed<int long>::value );
		M_ENSURE( ! name.is_empty() );
		(*f_oCookies)[ name ] = (*it).mid( sepIdx + 1 ); /* + 1 for '=' char */
		}
	return;
	M_EPILOG
	}

ORequest::dictionary_ptr_t ORequest::compress_jar( yaal::hcore::HString const& app )
	{
	M_PROLOG
	static int const MAX_COOKIE_SIZE = 4096;
	static int const MAX_COOKIES_PER_PATH = 20;
	static int const META_SIZE = 512;
	static int const PAYLOAD_SIZE = MAX_COOKIE_SIZE - META_SIZE;
	HString jar( MAX_COOKIES_PER_PATH * MAX_COOKIE_SIZE, true );
	jar = "";
	int cookieNo = 0;
	for ( dictionary_t::iterator it = f_oCookies->begin(); it != f_oCookies->end(); ++ it, ++ cookieNo )
		{
		if ( cookieNo )
			jar += "\001";
		jar += it->first;
		jar += '=';
		jar += it->second;
		}
	jar = base64::encode( jar );
	int size = static_cast<int>( jar.get_length() );
	HString properName;
	HString payload;
	cookieNo = 0;
	f_oJar->clear();
	for ( int offset = 0; offset < size; offset += PAYLOAD_SIZE, ++ cookieNo )
		{
		properName.format( "%s%02d", app.raw(), cookieNo );
		if ( ! offset )
			{
			payload.format( "%04d", size );
			payload += jar.mid( offset, PAYLOAD_SIZE );
			}
		else
			payload = jar.mid( offset, PAYLOAD_SIZE );
		(*f_oJar)[ properName ] = payload;
		}
	return ( f_oJar );
	M_EPILOG
	}

HSocket::ptr_t ORequest::socket( void )
	{
	return ( f_oSocket );
	}

HSocket::ptr_t const ORequest::socket( void ) const
	{
	return ( f_oSocket );
	}

ORequest::const_iterator ORequest::begin( void ) const
	{
	dictionary_t::const_iterator it = f_oGET->begin();
	origin_t o = it != f_oGET->end() ? ORIGIN::GET : ORIGIN::POST;
	return ( const_iterator( this, o, o == ORIGIN::GET ? it : f_oPOST->begin() ) );
	}

ORequest::const_iterator ORequest::end( void ) const
	{
	return ( const_iterator( this, ORIGIN::POST, f_oPOST->end() ) );
	}


ORequest::HConstIterator::HConstIterator( HConstIterator const& it )
	: f_poOwner( it.f_poOwner ), f_eOrigin( it.f_eOrigin ), f_oIt( it.f_oIt )
	{
	}

bool ORequest::HConstIterator::operator != ( HConstIterator const& it ) const
	{
	return ( ( f_eOrigin != it.f_eOrigin ) || ( f_oIt != it.f_oIt ) );
	}

ORequest::HConstIterator& ORequest::HConstIterator::operator ++ ( void )
	{
	M_ASSERT( ( f_eOrigin == ORequest::ORIGIN::GET ) || ( f_oIt != f_poOwner->f_oPOST->end() ) );
	++ f_oIt;
	if ( ( f_eOrigin == ORequest::ORIGIN::GET ) && ( f_oIt == f_poOwner->f_oGET->end() ) )
		{
		f_eOrigin = ORequest::ORIGIN::POST;
		f_oIt = f_poOwner->f_oPOST->begin();
		}
	return ( *this );
	}

ORequest::dictionary_t::map_elem_t const& ORequest::HConstIterator::operator* ( void ) const
	{
	return ( *f_oIt );
	}

ORequest::HConstIterator::HConstIterator( ORequest const* a_poOwner,
		ORequest::origin_t const& origin, ORequest::dictionary_t::const_iterator it )
	: f_poOwner( a_poOwner ), f_eOrigin( origin ), f_oIt( it )
	{
	}

}

