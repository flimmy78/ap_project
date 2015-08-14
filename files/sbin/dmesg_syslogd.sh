#!/bin/bash

. /sbin/autelan_functions.in

main() {
	/etc/jsock/jcmd.sh syn /usr/sbin/syn_ap_time.sh
	local file=${SYNTIME}
	syslogd

	# todo: check & create SYNTIME here
	while [[ ! -f ${file} ]]
	do
		sleep 9
	done

	save_init_log
	save_last_syslog
}

main "$@"
