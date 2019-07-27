
def:
	@echo
	@echo "   make build   --   build routerd"
	@echo "   make pkg-deb --   build routerd\'s debian package"
	@echo "   make clean   --   clean build objects"
	@echo "   make run     --   execute routerd"
	@echo "   make debug   --   execute routerd on GDB"
	@echo

build:
	make -C src

clean:
	make -C src clean

run:
	make -C src run

debug:
	make -C src debug

pkg-deb:
	make -C src pkg-deb

