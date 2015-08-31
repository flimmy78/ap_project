#!/bin/bash

set_ap_mac()
{
	local err=0
	local mdmac=$(/etc/jsock/jcmd.sh syn /usr/sbin/get_sysinfo mac 2> /dev/null)
	local apmac=$(partool -part mtd7 -show product.mac 2> /dev/null)
	
	if [[ "${apmac}" != "${mdmac}" && "${mdmac}" ]] ;then
		apmac="${mdmac}"
		partool -part mtd7 -new product.mac "${apmac}";err=$?
		[[ ${err} -ne 0 ]] && partool -part mtd7 -empty 
	fi
}

main()
{
	sleep 30
	set_ap_mac
}

main "$@"
