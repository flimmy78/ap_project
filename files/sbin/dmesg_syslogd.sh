#!/bin/bash

. /sbin/autelan_functions.in

main() {
	/etc/jsock/jcmd.sh syn /usr/sbin/syn_ap_time.sh
	local file=${SYNTIME}
	local i=0
	syslogd

	# todo: check & create SYNTIME here
	while [[ ! -f ${file} ]]
	do
		sleep 9
		((i++))
		if ((i>6)); then
			date '+%F-%H:%M:%S' > ${file}
		fi
	done

	save_init_log
	save_last_syslog
}

main "$@"
