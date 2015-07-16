#!/bin/bash

set_ap_mac()
{
	local mdmac=$(/etc/jsock/jcmd.sh syn atenv infos/product/mac)
	local apmac=$(/usr/bin/partool -part mtd7 -show product.mac)
	
	if [[ "${apmac}" != "${mdmac}" ]] ;then
		apmac="${mdmac}"
		/usr/bin/partool -part mtd7 -new product.mac "${apmac}"
	fi
}

main()
{
	sleep 30
	set_ap_mac
}

main "$@"
