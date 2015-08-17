#!/bin/bash

. /sbin/autelan_functions.in

get_part_mtd7() {
	local ssid=""
	local ssid_str=$1

	ssid=$(partool -part mtd7 -show ${ssid_str})
	[[ "${ssid}" == "${ssid_str} not exist" ]] && ssid=""
	echo ${ssid}
}

check_wireless_ssid() {
	local operation=0
	local tmp=/tmp/.wireless_string.tmp
	local string_service=wireless
	local oem_ssid=""
	local ssid0=""
	local ssid1=""
	local option=""
	local value=""
	local i=0

	oem_ssid=$(get_part_mtd7 oem.ssid)
	if [[ "${oem_ssid}" ]]; then
		ssid0=config.${oem_ssid}
		ssid1=${oem_ssid}	
	else
		ssid0=${DEF_SSID0}
		ssid1=${DEF_SSID1}
	fi
	echo "$0: ssid0=${ssid0}, ssid1=${ssid1}" >> ${DEBUG_LOG_LOCAL}

	i=0
	uci show wireless | sed -n 's/=/ /g;/ssid/p' > ${tmp}
	while read option value; do
		if [[ ${i} = 0 ]]; then
			echo "$0: ${i}, opt=${option}, old=${value}, new=${ssid0}" >> ${DEBUG_LOG_LOCAL}
			if [[ ${ssid0} != ${value} ]]; then
				set_option_value ${option} ${ssid0}; operation=$?
			fi
		else
			echo "$0: ${i}, opt=${option}, old=${value}, new=${ssid1}" >> ${DEBUG_LOG_LOCAL}
			if [[ ${ssid1} != ${value} ]]; then
				set_option_value ${option} ${ssid1}; operation=$?
			fi
		fi
		((i++))
	done < ${tmp}

	if [[ ${operation} -eq 1 ]]; then
		echo "need commit & reload"
		#commit_option_value ${string_service}
	fi
	rm ${tmp}
	
	return ${operation}
}


main() {

	local operation2=0

	check_wireless_ssid; operation2=$?

	if [[ ${operation2} -eq 1 ]]; then
		wifi down; wifi up
	fi
	return 1
}

main "$@"
