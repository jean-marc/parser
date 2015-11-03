CC = g++ -fdiagnostics-color -I .  
CFLAGS = -O3 -Wall -std=c++0x -fstack-check -DOBJRDF_VERB
%.o:%.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@
%.test:%.test.cpp %.h parser.h char_iterator.h
	$(CC) $(CFLAGS) $< -o $@ 
sparql_parser.test:sparql_parser.test.cpp sparql_parser.h parser.h char_iterator.h
	$(CC) $(CFLAGS) $< -lobjrdf -o $@ 
install:parser.h char_iterator.h
	mkdir -p /usr/local/include/parser
	cp ifthenelse.hpp parser.h char_iterator.h xml_parser.h url_decoder.h /usr/local/include/parser/
arm_install:parser.h char_iterator.h
	mkdir -p /opt/ioplex_mx/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/include/parser
	cp ifthenelse.hpp parser.h char_iterator.h xml_parser.h url_decoder.h /opt/ioplex_mx/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/include/parser/
readme.pdf:readme.md
	pandoc -V geometry:margin=1in -o $@ $<
