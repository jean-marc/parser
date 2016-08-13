CC = g++ -fdiagnostics-color -I .  
CFLAGS = -O3 -Wall -std=c++0x -fstack-check
%.o:%.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@
%.test:%.test.cpp %.h parser.h char_iterator.h
	$(CC) $(CFLAGS) $< -o $@ 
empty:

sparql_parser.test:sparql_parser.test.cpp sparql_parser.h parser.h char_iterator.h
	$(CC) $(CFLAGS) $< -lobjrdf -o $@ 
readme.pdf:readme.md
	pandoc -V geometry:margin=1in -o $@ $<
prefix=/usr
exec_prefix=$(prefix)
includedir=$(prefix)/include
libdir=$(exec_prefix)/lib
INSTALL=install
INSTALL_PROGRAM=$(INSTALL)
INSTALL_DATA=$(INSTALL) -m 644
install:
	mkdir -p $(DESTDIR)$(includedir)/parser
	$(INSTALL_DATA) parser.h char_iterator.h xml_parser.h url_decoder.h ifthenelse.hpp $(DESTDIR)$(includedir)/parser
#more work here
check:xml_parser.test url_decoder.test
	echo 'it is all good!'
