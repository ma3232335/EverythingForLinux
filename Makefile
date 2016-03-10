
TARGET := bin/everything bin/everything-updatedb

all: $(TARGET)

bin/everything: 
	cd everything; /usr/bin/qmake-qt4
	make -C everything
	install -D everything/EverythingForLinux $@ 

bin/everything-updatedb:
	make -C everything-updatedb
	install -D everything-updatedb/everything-updatedb $@

clean:
	make clean -C everything
	make clean -C everything-updatedb
	rm -f $(TARGET)

.PHONY: all clean $(TARGET)
