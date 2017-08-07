#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H
#include <map>
#include "parser.h"

using namespace parser;
struct http_parser{
	/*
		HTTP/1.1 200 OK
		Connection: Keep-Alive
		Content-Length: 4
		Access-Control-Allow-Origin: *
		Content-Type: application/javascript
		Date: Sun, 06 Aug 2017 00:59:01 GMT

		true

	*/
	typedef _sqc<'\r','\n'> rn;
	typedef _sqc<'H','T','T','P','/','1','.','1'> version;
	typedef _rc<'0','9'> digit;
	typedef _sq<digit,digit,digit> code;
	typedef _kl<_or<_rc<'a','z'>,_rc<'A','Z'>,_c<' '>>> reason;
	typedef _sq<code,_c<' '>,reason> status;
	struct key:_pl<_or<_rc<'a','z'>,_rc<'A','Z'>,_c<'-'>>>{};
	//broken parser: does not backtrack!
	//typedef _pl<_or<_nt<_c<'\r'>>,_nt<_c<'\n'>>>> value;
	struct value:_pl<_nt<_c<'\r'>>> {};
	typedef _sq<key,_c<':'>,_c<' '>,value,rn> header;
	typedef _sq<
				version,_c<' '>,status,rn,
				_kl<header>,
				rn
			> response;	
	//does not have to be defined here
	template<typename ITERATOR> struct my_handler:parser::handler<
		ITERATOR,
		std::tuple<
			key,
			value
		>
	>{
		std::map<string,string> headers;
		std::string current_key;
		void start(key){}
		void stop(key,ITERATOR begin,ITERATOR end,bool v){
			if(v)
				current_key={begin,end};
		}
		void start(value){}
		void stop(value,ITERATOR begin,ITERATOR end,bool v){
			if(v)
				headers[current_key]={begin,end};
		}
	};

};
#endif
