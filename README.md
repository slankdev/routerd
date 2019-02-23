
# Routerd (this is under the developing)

Install and Start routerd
```
git clone <this-repo> && cd _
make && sudo make install
sudo systemctl start routerd
systemctl status routerd
```

Check routerd
```
sudo tail -f /var/log/syslog | grep routerd
```

