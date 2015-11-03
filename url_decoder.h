#ifndef URL_DECODER_H
#define URL_DECODER_H
#include "parser.h"

using namespace parser;
struct url_decoder{
	typedef _or<_rc<'0','9'>,_rc<'a','f'>,_rc<'A','F'>> hex;
	typedef _sq<_c<'%'>,hex,hex> encoded;
	//let's be pretty conservative
	typedef _or<_rc<'0','9'>,_rc<'a','z'>,_rc<'A','Z'>,_c<'_'>,_c<'.'>,_c<'-'>> allowed;
	typedef _kl<_or<allowed,encoded>> word;
	template<typename ITERATOR> struct my_handler:parser::handler<
		ITERATOR,
		std::tuple<
			allowed,
			encoded
		>
	>{
		std::string decoded;
		void start(allowed){}
		void stop(allowed,ITERATOR begin,ITERATOR end,bool v){
			if(v) decoded.append(1,*begin);//we could process more that 1 char at a time
		}
		void start(encoded){}
		//could also use built-in conversion iostream
		static uint8_t hex_to_int(char c){
			if('0'<=c && c<='9') return c-'0';
			if('A'<=c && c<='F') return 0xA+c-'A';
			if('a'<=c && c<='f') return 0xA+c-'a';
		}
		void stop(encoded,ITERATOR begin,ITERATOR end,bool v){
			if(v){
				char a=(hex_to_int(begin[1])<<4)+hex_to_int(begin[2]);
				decoded.append(1,a);
			}
		}
	};
	/*
	string go(string s){
		my_handler<string::const_iterator> h;//would be nice if we did not have to do that, works if go function returns handler
		auto r=url_decoder::word::go(s.cbegin(),s.cend(),h);//why does it stop early?
		return h.decoded;
	}
	*/
};
#endif
