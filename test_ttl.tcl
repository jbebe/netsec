while 1 {
	hping send "ip(daddr=192.168.0.1,ttl=63)+tcp(sport=123,dport=80,flags=s)"
	after 50
}
