dln netstat -i | head -n1 | grep -E "Name +Mtu +Network +Address +Ipkts +Ierrs +Opkts +Oerrs +Coll"
dln netstat -i | head -n2 | tail -n1 | grep -E "Iface +MTU +Met +RX-OK +RX-ERR +RX-DRP +RX-OVR +TX-OK +TX-ERR +TX-DRP +TX-OVR +Flg"
