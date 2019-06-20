
# Libvty (WIP)

```
sudo apt update && sudo apt insetall -y \
  libunwind-dev cmake build-essential
git clone https://github.com/CESNET/libyang.git /tmp/libyang && cd $_
mkdir build; cd build
cmake -DENABLE_LYD_PRIV=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr \
      -D CMAKE_BUILD_TYPE:String="Release" ..
make && sudo make install
```

```
cd libvty/src
make
./a.out
```
