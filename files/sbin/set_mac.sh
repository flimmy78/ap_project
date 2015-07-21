#!/bin/bash

set_ap_mac()
{
	local err=0
	local mdmac=$(/etc/jsock/jcmd.sh syn /usr/sbin/get_sysinfo mac)
	local apmac=$(/usr/bin/partool -part mtd7 -show product.mac)
	
	if [[ "${apmac}" != "${mdmac}" ]] ;then
		apmac="${mdmac}"
		/usr/bin/partool -part mtd7 -new product.mac "${apmac}";err=$?
		[[ ${err} -ne 0 ]] && /usr/bin/partool -part mtd7 -empty
	fi
}

main()
{
	sleep 30
	set_ap_mac
}

main "$@"
