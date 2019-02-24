
install_tools:
	make -C tools/nldump install
	make -C tools/nlsniff install
	make -C tools/routerd install

install_tools_docker:
	make -C tools/nldump install_docker
	make -C tools/nlsniff install_docker
	make -C tools/routerd install_docker

clean_tools:
	make -C tools/nldump clean
	make -C tools/nlsniff clean
	make -C tools/routerd clean

clean_debian:
	rm -f tools/routerd-dbgsym_*.ddeb
	rm -f tools/routerd_*.dsc
	rm -f tools/routerd_*.tar.gz
	rm -f tools/routerd_*.build
	rm -f tools/routerd_*.buildinfo
	rm -f tools/routerd_*.changes
	rm -f tools/routerd_*.deb
	rm -f tools/routerd/debian/routerd.substvars
	rm -f tools/routerd/debian/debhelper-build-stamp
	rm -rf tools/routerd/debian/routerd
