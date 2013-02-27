#! /bin/sh

export LD_LIBRARY_PATH=/home/amok/usr/lib
export HOME=/home/amok/

LOG="/tmp/cgi.log"

env > ${LOG}
echo "${@}" >> ${LOG}

if [ -z "${REMOTE_ADDR}" ] ; then
	export REMOTE_ADDR="127.0.0.1"
fi

if [ -z "${HTTP_USER_AGENT}" ] ; then
	export HTTP_USER_AGENT="terminal"
fi

#while read LINE ; do
#	echo "#${LINE}#" >> ${LOG}
#done

FILTER=""

BOA=`echo ${SERVER_SOFTWARE} | grep "Boa"`

if [ ! -z "${BOA}" ] ; then
	env -i \
		LD_LIBRARY_PATH="${LD_LIBRARY_PATH}" \
		HOME="${HOME}" \
		CONTENT_LENGTH="${CONTENT_LENGTH}" \
		CONTENT_TYPE="${CONTENT_TYPE}" \
		DOCUMENT_ROOT="${DOCUMENT_ROOT}" \
		GATEWAY_INTERFACE="${GATEWAY_INTERFACE}" \
		HTTP_ACCEPT_ENCODING="${HTTP_ACCEPT_ENCODING}" \
		HTTP_ACCEPT_LANGUAGE="${HTTP_ACCEPT_LANGUAGE}" \
		HTTP_CACHE_CONTROL="${HTTP_CACHE_CONTROL}" \
		HTTP_COOKIE="${HTTP_COOKIE}" \
		HTTP_DNT="${HTTP_DNT}" \
		HTTP_HOST="${HTTP_HOST}" \
		HTTP_REFERER="${HTTP_REFERER}" \
		HTTP_USER_AGENT="${HTTP_USER_AGENT}" \
		QUERY_STRING="${QUERY_STRING}" \
		REMOTE_ADDR="${REMOTE_ADDR}" \
		REMOTE_HOST="${REMOTE_HOST}" \
		REMOTE_PORT="${REMOTE_PORT}" \
		REMOTE_USER="${REMOTE_USER}" \
		REQUEST_METHOD="${REQUEST_METHOD}" \
		REQUEST_URI="${REQUEST_URI}" \
		SCRIPT_FILENAME="${SCRIPT_FILENAME}" \
		SCRIPT_NAME="${SCRIPT_NAME}" \
		SERVER_ADDR="${SERVER_ADDR}" \
		SERVER_ADMIN="${SERVER_ADMIN}" \
		SERVER_NAME="${SERVER_NAME}" \
		SERVER_PORT="${SERVER_PORT}" \
		SERVER_PROTOCOL="${SERVER_PROTOCOL}" \
		SERVER_SOFTWARE="${SERVER_SOFTWARE}" \
		./build/debug/hector.cgi/1exec "${@}" | sed -e "s@:SSL@ssl-redirect.cgi?REQUEST_URI=${REQUEST_URI}@"
else
	env -i \
		LD_LIBRARY_PATH="${LD_LIBRARY_PATH}" \
		HOME="${HOME}" \
		CONTENT_LENGTH="${CONTENT_LENGTH}" \
		CONTENT_TYPE="${CONTENT_TYPE}" \
		DOCUMENT_ROOT="${DOCUMENT_ROOT}" \
		GATEWAY_INTERFACE="${GATEWAY_INTERFACE}" \
		HTTP_ACCEPT_ENCODING="${HTTP_ACCEPT_ENCODING}" \
		HTTP_ACCEPT_LANGUAGE="${HTTP_ACCEPT_LANGUAGE}" \
		HTTP_CACHE_CONTROL="${HTTP_CACHE_CONTROL}" \
		HTTP_COOKIE="${HTTP_COOKIE}" \
		HTTP_DNT="${HTTP_DNT}" \
		HTTP_HOST="${HTTP_HOST}" \
		HTTP_REFERER="${HTTP_REFERER}" \
		HTTP_USER_AGENT="${HTTP_USER_AGENT}" \
		HTTPS="${HTTPS}" \
		QUERY_STRING="${QUERY_STRING}" \
		REMOTE_ADDR="${REMOTE_ADDR}" \
		REMOTE_HOST="${REMOTE_HOST}" \
		REMOTE_PORT="${REMOTE_PORT}" \
		REMOTE_USER="${REMOTE_USER}" \
		REQUEST_METHOD="${REQUEST_METHOD}" \
		REQUEST_URI="${REQUEST_URI}" \
		SCRIPT_FILENAME="${SCRIPT_FILENAME}" \
		SCRIPT_NAME="${SCRIPT_NAME}" \
		SERVER_ADDR="${SERVER_ADDR}" \
		SERVER_ADMIN="${SERVER_ADMIN}" \
		SERVER_NAME="${SERVER_NAME}" \
		SERVER_PORT="${SERVER_PORT}" \
		SERVER_PROTOCOL="${SERVER_PROTOCOL}" \
		SERVER_SOFTWARE="${SERVER_SOFTWARE}" \
		./build/debug/hector.cgi/1exec "${@}"

fi

