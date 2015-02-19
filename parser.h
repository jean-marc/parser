#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <iostream>
#include <string>
namespace parser{
	//mini EBNF
	//ITERATOR must be a ForwardIterator, but we must make sure it is not a reference
	//so istreambuf_iterator and istream_iterator are NOT valid because single-pass
	struct _t{
		template<typename ITERATOR> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			return {true,begin};
		}
	};
	struct _any{
		template<typename ITERATOR> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			return {true,++begin};
		}
	};
	template<char C> struct _c{
		template<typename ITERATOR> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			return *begin==C ? std::pair<bool,ITERATOR>(true,++begin) : std::pair<bool,ITERATOR>(false,begin);
		}
	};
	template<char A,char B> struct _rc{
		template<typename ITERATOR> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			bool tmp=(A<=*begin)&&(*begin<=B);
			return {tmp,tmp ? ++begin : begin};
		}
	};
	template<typename FIRST,typename... NEXT> struct _sq:_sq<NEXT...>{
		template<typename ITERATOR> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			auto tmp=FIRST::go(begin,end);
			return tmp.first ? _sq<NEXT...>::go(tmp.second,end) : std::pair<bool,ITERATOR>(false,begin);
		}	
	};
	template<typename LAST> struct _sq<LAST>:LAST{};
	//helper for array of chars
	template<char... C> struct _sqc{
		template<typename ITERATOR> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			static const std::vector<char> v={C...};	
			for(auto i=v.cbegin();i<v.cend();++i,++begin){
				if((begin==end)||(*i!=*begin)) return {false,begin};
			}
			return {true,begin};
		}
	};
	template<typename FIRST,typename... NEXT> struct _or:_or<NEXT...>{
		template<typename ITERATOR> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			auto tmp=FIRST::go(begin,end);
			return tmp.first ? tmp : _or<NEXT...>::go(begin,end);
		}
	};
	template<typename LAST> struct _or<LAST>:LAST{};
	template<typename T> struct _nt{
		template<typename ITERATOR> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			auto tmp=T::go(begin,end);
			//return tmp.first ? std::pair<bool,ITERATOR>(false,i) : std::pair<bool,ITERATOR>(true,/*tmp.second*/i+1);//more work...
			return tmp.first ? std::pair<bool,ITERATOR>(false,begin) : std::pair<bool,ITERATOR>(true,/*tmp.second*/++begin);//more work...
		}
	};
	//recursive parsers
	template<typename T> struct _kl:_or<_sq<T,_kl<T>>,_t>{};//kleene star
	template<typename T> struct _pl:_sq<T,_or<_pl<T>,_t>>{}; /* a+ b=a(b|true) */
	//callbacks, the problem is how do we pass the callback, can we instantiate the type?
	//but we wouldn't even have access to the this, global members are not desirable either
	template<typename T> struct event:T{
		template<typename ITERATOR> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			auto tmp=T::go(begin,end);	
			if(tmp.first){
				std::string s(begin,tmp.second);
				std::cerr<<"callback:`"<<s<<"'"<<std::endl;
			}
			return tmp;
		}
	};
}
#endif

