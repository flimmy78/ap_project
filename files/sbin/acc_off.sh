#!/bin/bash
. /sbin/autelan_functions.in

main() {
	mv /tmp/log/messages ${SYSLOGDPATH}/
	mv ${SYNTIME} ${LASTSYNTIME}
}
main "$@"
