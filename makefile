DESTDIR=/usr/local/bin

all: setgroup

setgroup: main.c
	gcc -std=gnu11 -pedantic -Wall -Wextra main.c -o setgroup

clean:
	rm -rf setgroup

install: setgroup
	cp setgroup $(DESTDIR)/setgroup
	chown root:root $(DESTDIR)/setgroup \
	&& chmod u+s $(DESTDIR)/setgroup \
	|| rm setgroup # Clean up this temp file if the above fails.

uninstall:
	rm $(DESTDIR)/setgroup
