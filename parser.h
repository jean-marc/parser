#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <iostream>
#include <string>
#include <tuple>
#include "ifthenelse.hpp"
namespace parser{
	//need to define context
	template<typename ITERATOR,typename _CB=std::tuple<>> struct handler{
		typedef handler SELF;
		typedef _CB CB;
		ITERATOR begin,end;//maybe could put that outside
		friend std::ostream& operator<<(std::ostream& os,const handler& h){
			return os<<h.begin;
		}
	};
	template<typename _T,typename _Tp> struct tuple_index{
		enum{value=0};//we have reached end of tuple shouldn't the value be >0???
	};
	template<typename _T,typename _Head,typename... _Tail> struct tuple_index<_T, std::tuple<_Head, _Tail...> >{
		enum{value=1+tuple_index<_T,std::tuple<_Tail...> >::value};
	};
	template<typename _Head, typename... _Tail> struct tuple_index<_Head, std::tuple<_Head, _Tail...> >{
		enum{value=0};
	};
	template<typename _T, typename _Tp> struct in_tuple{
		enum{value=tuple_index<_T,_Tp>::value < std::tuple_size<_Tp>::value};
	};
	struct nil{};
	//mini EBNF
	//ITERATOR must be a ForwardIterator, but we must make sure it is not a reference
	//so istreambuf_iterator and istream_iterator are NOT valid because single-pass
	template<typename T> struct event:T{
		/*
 		*	how can we invoke callbacks without side-effect?, we could create a recursive structure that matches the grammar
 		*/ 
		/*
		template<typename HANDLER> void _go(ITERATOR begin,ITERATOR end){
			HANDLER h;
			auto tmp=T::go(begin,end,h);//h is a reference
		}
		*/
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			//callback could make better use of stack, it could set its own callback
			//it might also be interesting to know when we start and stop
			h.instance->start(T());
			auto tmp=T::go(h);	
			h.instance->stop(T(),h.begin,tmp.second.begin,tmp.first);
			//if(tmp.first) h.instance->process(T(),h.begin,tmp.second.begin);
			return tmp;
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			auto tmp=T::template go<ITERATOR,CB>(begin,end);	
			if(tmp.first){
				std::string s(begin,tmp.second);
				std::cerr<<"callback:`"<<s<<"'"<<std::endl;
			}
			return tmp;
		}
	};
	template<typename CURRENT,typename CB> struct _cb{
		typedef CURRENT type;
	};
	template<typename CURRENT,typename... T> struct _cb<CURRENT,std::tuple<T...>>{
		typedef typename IfThenElse<in_tuple<CURRENT,std::tuple<T...>>::value,event<CURRENT>,CURRENT>::ResultT type;
	};
	template<typename CURRENT> struct _cb<CURRENT,CURRENT>{
		typedef event<CURRENT> type;
	};
	struct _t{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			return {true,h};
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			return {true,begin};
		}
	};
	struct _f{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			return {false,h};
		}
	};
	struct _any{//also called epsilon parser
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			if(h.begin==h.end) return {false,h};
			++h.begin;return {true,h};
			//return {true,{++h.begin,h.end}};//new handler
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			return {true,++begin};
		}
	};
	template<char C> struct _c{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			if(h.begin==h.end) return {false,h};
			//if(*h.begin==C) return {true,{++h.begin,h.end}};
			if(*h.begin==C){++h.begin;return {true,h};}
			return {false,h};
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			return *begin==C ? std::pair<bool,ITERATOR>(true,++begin) : std::pair<bool,ITERATOR>(false,begin);
		}
	};
	template<char A,char B> struct _rc{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			if(h.begin==h.end) return {false,h};
			//if((A<=*h.begin)&&(*h.begin<=B)) return {true,{++h.begin,h.end}};
			if((A<=*h.begin)&&(*h.begin<=B)){++h.begin;return {true,h};}
			return {false,h};
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR>	go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			bool tmp=(A<=*begin)&&(*begin<=B);
			return {tmp,tmp ? ++begin : begin};
		}
	};
	template<typename FIRST,typename... NEXT> struct _sq{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			auto tmp=_cb<FIRST,typename HANDLER::CB>::type::go(h);
			return tmp.first ? _cb<_sq<NEXT...>,typename HANDLER::CB>::type::go(tmp.second) : std::pair<bool,HANDLER>(false,h);
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			auto tmp=_cb<FIRST,CB>::type::template go<ITERATOR,CB>(begin,end);
			return tmp.first ? _cb<_sq<NEXT...>,CB>::type::template go<ITERATOR,CB>(tmp.second,end) : std::pair<bool,ITERATOR>(false,begin);
		}	
	};
	template<typename LAST> struct _sq<LAST>:LAST{};
	//template<typename CURRENT,typename CB> struct _cb<_sq<CURRENT>,CB>:_cb<CURRENT,CB>{};
	template<typename CURRENT> struct _cb<_sq<CURRENT>,CURRENT>{
		typedef event<CURRENT> type;
	};
	template<typename CURRENT,typename... T> struct _cb<_sq<CURRENT>,std::tuple<T...>>{
		typedef typename IfThenElse<in_tuple<CURRENT,std::tuple<T...>>::value,event<CURRENT>,CURRENT>::ResultT type;
	};
	//helper for array of chars
	template<char... C> struct _sqc{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			static const std::vector<char> v={C...};	
			for(auto i=v.cbegin();i<v.cend();++i,++h.begin){
				if((h.begin==h.end)||(*i!=*h.begin)) return {false,h};
			}
			return {true,h};
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			static const std::vector<char> v={C...};	
			for(auto i=v.cbegin();i<v.cend();++i,++begin){
				if((begin==end)||(*i!=*begin)) return {false,begin};
			}
			return {true,begin};
		}
	};
	template<typename FIRST,typename... NEXT> struct _or{
		//can we catch recursion and eg: 
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			auto tmp=_cb<FIRST,typename HANDLER::CB>::type::go(h);
			return tmp.first ? tmp : _cb<_or<NEXT...>,typename HANDLER::CB>::type::go(h);
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			//auto tmp=FIRST::go(begin,end);
			auto tmp=_cb<FIRST,CB>::type::template go<ITERATOR,CB>(begin,end);
			return tmp.first ? tmp : _cb<_or<NEXT...>,CB>::type::template go<ITERATOR,CB>(begin,end);
		}
	};
	template<typename... NEXT> struct _or<_t,NEXT...>;//compile error
	//to be tested
	//template<typename... NEXT> struct _or<_f,NEXT...>:_or<NEXT...>{};
	template<typename LAST> struct _or<LAST>:LAST{};
	typedef _or<_c<' '>,_c<'\t'>,_c<'\n'>,_c<'\r'>> WS;

	//template<typename CURRENT,typename CB> struct _cb<_or<CURRENT>,CB>:_cb<CURRENT,CB>{};
	template<typename CURRENT> struct _cb<_or<CURRENT>,CURRENT>{
		typedef event<CURRENT> type;
	};
	template<typename CURRENT,typename... T> struct _cb<_or<CURRENT>,std::tuple<T...>>{
		typedef typename IfThenElse<in_tuple<CURRENT,std::tuple<T...>>::value,event<CURRENT>,CURRENT>::ResultT type;
	};
	template<typename T> struct _nt{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			if(h.begin==h.end) return {false,h};
			auto tmp=_cb<T,typename HANDLER::CB>::type::go(h);
			if(tmp.first) return {false,h};
			++h.begin;
			return {true,h};
			//return {true,{++h.begin,h.end}};
		}
		template<typename ITERATOR,typename CB=nil> static std::pair<bool,ITERATOR> go(ITERATOR begin,ITERATOR end){
			if(begin==end) return {false,begin};
			//auto tmp=T::go(begin,end);
			auto tmp=_cb<T,CB>::type::template go<ITERATOR,CB>(begin,end);
			return tmp.first ? std::pair<bool,ITERATOR>(false,begin) : std::pair<bool,ITERATOR>(true,/*tmp.second*/++begin);//more work...
		}
	};
	//recursive parsers, creates recursive callbacks! better derive class
	//is stack overflow a concern or is it optimized by compiler? no it is not!
	//uname -a and uname -s
	template<typename T> struct _kl:_or<_sq<T,_kl<T>>,_t>{};//kleene star
	template<typename T> struct _pl:_sq<T,_or<_pl<T>,_t>>{}; /* a+ b=a(b|true) */
	template<typename T> struct _nr_kl{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			auto _h=h;
			auto tmp=T::go(_h);
			while(tmp.first) tmp=T::go({++_h.begin,_h.end});
			return {true,_h};//goes one too far
		}
	};	
	//should be able to turn white space on and off
	template<typename FIRST,typename... NEXT> struct _sq_ws{
		template<typename HANDLER> static std::pair<bool,HANDLER> go(HANDLER h){
			h=_kl<WS>::go(h).second;//eats whitespace
			auto tmp=_cb<FIRST,typename HANDLER::CB>::type::go(h);
			return tmp.first ? _cb<_sq_ws<NEXT...>,typename HANDLER::CB>::type::go(tmp.second) : std::pair<bool,HANDLER>(false,h);
		}
	};
	template<typename LAST> struct _sq_ws<LAST>:LAST{};
	template<typename CURRENT> struct _cb<_sq_ws<CURRENT>,CURRENT>{
		typedef event<CURRENT> type;
	};
	template<typename CURRENT,typename... T> struct _cb<_sq_ws<CURRENT>,std::tuple<T...>>{
		typedef typename IfThenElse<in_tuple<CURRENT,std::tuple<T...>>::value,event<CURRENT>,CURRENT>::ResultT type;
	};
	template<typename T> struct _pl_ws:_sq_ws<T,_or<_pl_ws<T>,_t>>{}; /* a+ b=a(b|true) */
}
#endif

