
install_tools:
	make -C tools/nldump install
	make -C tools/nlsniff install
	make -C tools/routerd install

install_tools_docker:
	make -C tools/nldump install_docker
	make -C tools/nlsniff install_docker
	make -C tools/routerd install_docker

