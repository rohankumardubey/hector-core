/*
---            `hector' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski             ---

	main.cxx - this file is integral part of `hector' project.

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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h> /* the `char** environ;' variable */

#include <yaal/hcore/hstring.hxx> /* all hAPI headers */
M_VCSID ( "$Id$" )
#include <yaal/hcore/hsocket.hxx>
#include <yaal/hcore/hlog.hxx>
#include <yaal/hcore/htokenizer.hxx>
#include <yaal/tools/signals.hxx>
#include <yaal/tools/hstringstream.hxx>
#include <yaal/tools/util.hxx>

#include "setup.hxx"
#include "options.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::util;
using namespace hector;

namespace hector
{

OSetup setup;

}

void query( int, char** );

int main( int argc_, char* argv_[] )
	{
	M_AT_END_OF_SCOPE( HSignalService::get_instance().stop(); );
	M_PROLOG
	try
		{
/* TO-DO: enter main loop code here */
		HSignalService::get_instance();
		setup._programName = argv_[ 0 ];
		handle_program_options( argc_, argv_ );
		setup._logPath.replace( "hectord", "hector.cgi" );
		hcore::log.rehash( setup._logPath, setup._programName );
		setup.test_setup();
/* *BOOM* */
		query( argc_, argv_ );
/* ... there is the place main loop ends. :OD-OT */
		}
	catch ( ... )
		{
		throw;
		}
	return ( 0 );
	M_FINAL
	}

HString escape( HString const& source )
	{
	M_PROLOG
	static HString result;
	result = source;
	result.trim_left().replace( "\\", "\\\\" ).replace( "\n", "\\n" );
	M_ASSERT( result.get_length() == static_cast<int long>( ::strlen( result.raw() ) ) );
	return ( result );
	M_EPILOG
	}

void push_query( HSocket& sock, HString const& query, char const* const mode, char const* const delim )
	{
	M_PROLOG
	HStringStream buffer;
	HTokenizer t( query, delim, HTokenizer::SKIP_EMPTY );
	for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it )
		sock << ( buffer << mode << ":" << escape( *it ) << endl << buffer );
	return;
	M_EPILOG
	}

void query( int argc, char** argv )
	{
	M_PROLOG
	HString sockPath( setup._socketRoot );
	sockPath += "/request.sock";
	try
		{
		HSocket sock( HSocket::TYPE::FILE );
		sock.connect( sockPath );
		HStringStream buffer;
		HString POST( "" );
		HFile in( stdin );
		while ( in.read_line( POST, HFile::READ::UNBUFFERED_READS ) >= 0 )
			push_query( sock, POST, "post", "&" );
		for ( int i = 1; i < argc; ++ i )
			sock << ( buffer << "get:" << escape( argv[ i ] ) << endl << buffer );
		char QS[] = "QUERY_STRING=";
		char CS[] = "HTTP_COOKIE=";
		for ( int i = 0; environ[ i ]; ++ i )
			{
			if ( ! strncmp( environ[ i ], QS, sizeof ( QS ) - 1 ) )
				{
				push_query( sock, environ[ i ] + sizeof ( QS ) - 1, "get", "&" );
				continue;
				}
			else if ( ! strncmp( environ[ i ], CS, sizeof ( CS ) - 1 ) )
				{
				push_query( sock, environ[ i ] + sizeof ( CS ) - 1, "cookie", ";" );
				continue;
				}
			buffer << "env:" << escape( environ[ i ] ) << endl;
			sock << buffer.consume();
			}
		sock << ( buffer << "done" << endl << buffer );
		HString msg;
		while ( sock.read_until( msg ) > 0 )
			cout << msg << endl;
		}
	catch ( HSocketException& e )
		{
		cout << "Cannot connect to `hector' daemon.<br />" << endl;
		cout << e.what() << endl;
		}
	return;
	M_EPILOG
	}

