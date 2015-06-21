eqselect: eqselect.c
	cc -o eqselect eqselect.c

clean:
	rm eqselect

install:
	cp eqselect /usr/bin
	chmod a+rx /usr/bin/eqselect

uninstall:
	rm /usr/bin/eqselect
