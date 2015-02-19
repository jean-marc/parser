CC = g++ -I .  
CFLAGS = -O3 -std=c++0x
%.o:%.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@
xml_parser.test:xml_parser.test.cpp parser.h char_iterator.h xml_parser.h
	$(CC) $(CFLAGS) $< -o $@ 
install:parser.h char_iterator.h
	mkdir -p /usr/local/include/parser
	cp parser.h char_iterator.h xml_parser.h /usr/local/include/parser/
arm_install:parser.h char_iterator.h
	mkdir -p /opt/ioplex_mx/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/include/parser
	cp parser.h char_iterator.h xml_parser.h /opt/ioplex_mx/usr/arm-buildroot-linux-gnueabihf/sysroot/usr/include/parser/
