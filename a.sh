route del default gw 192.168.3.1 eth0
route add default gw 192.168.3.1 dev eth0 metric 0


route del default ppp0
route add default dev ppp0 metric 99
