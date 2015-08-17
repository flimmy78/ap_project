#!/bin/bash
main() {
	/etc/jsock/jcmd.sh asyn "route del default gw 1.0.0.1" 2>/dev/null
	/etc/jsock/jcmd.sh asyn "route add default ppp0" 2>/dev/null
	/etc/jsock/jcmd.sh asyn "start_3g"

	cp /rom/etc/config/network /etc/config/network
	cp /rom/etc/config/firewall /etc/config/firewall

	/etc/init.d/network reload > /dev/null 2>&1
	/etc/init.d/firewall restart > /dev/null 2>&1
}

main "$@"
