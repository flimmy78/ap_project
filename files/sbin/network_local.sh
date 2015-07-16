#!/bin/bash
main() {
	cp /etc/config/network.dev /etc/config/network
	cp /etc/config/firewall.dev /etc/config/firewall

	/etc/init.d/network reload > /dev/null 2>&1
	/etc/init.d/firewall restart > /dev/null 2>&1
	/etc/jsock/jcmd.sh syn "stop_3g"
	/etc/jsock/jcmd.sh syn "route del default ppp0" 2>/dev/null
	/etc/jsock/jcmd.sh syn "route add default gw 1.0.0.1" 2>/dev/null
}

main "$@"
