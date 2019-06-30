
# vpp\_routerd

create tap interface
```
sudo ip tuntap add dev vpp0 mode tap
sudo ip tuntap add dev vpp1 mode tap
sudo ip tuntap add dev vpp2 mode tap
sudo ip tuntap add dev vpp3 mode tap
```

/etc/vpp/startup.conf
```
unix {
        nodaemon
        log /var/log/vpp/vpp.log
        full-coredump
        cli-listen /run/vpp/cli.sock
        cli-no-banner
        exec /etc/vpp/exec
        gid vpp
}

api-segment {
        gid vpp
}

dpdk {
        dev 0000:00:08.0
}
```

/etc/vpp/exec
```
```

/etc/routerd/routerd.conf
```
netlink
  set interface pair kernel-ifindex 28 vpp-ifindex 1
  set interface pair kernel-ifindex 29 vpp-ifindex 2
  set interface pair kernel-ifindex 30 vpp-ifindex 3
  set interface pair kernel-ifindex 31 vpp-ifindex 4
  exit
```


