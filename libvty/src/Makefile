
build:
	make -C vui build

clean:
	make -C vui clean

install:
	install libvui.a /usr/local/lib
	mkdir -p /usr/local/include/vui/lib/printf
	for file in $(shell find . -name "*.h"); do \
		cp -rf $$file /usr/local/include/$$file; \
	done

uninstall:
	rm -f /usr/local/lib/libvui.a
	rm -rf /usr/local/include/vui
