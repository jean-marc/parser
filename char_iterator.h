#ifndef PARSER_CHAR_ITERATOR_H
#define PARSER_CHAR_ITERATOR_H
#include <vector>
#include <memory>
#include <iostream>
/*
 *	similar to istream_iterator except you can go backwards
 *	and make copies
 *	could we make this a template so we only need a header file?
 */
namespace parser{
	template<
		size_t N=0x1000
	>
	struct basic_char_iterator{
		typedef std::random_access_iterator_tag iterator_category;//might be more than what we need
		typedef char value_type;
		typedef char& reference;
		typedef std::ptrdiff_t difference_type;
		typedef basic_char_iterator pointer;
		
		struct source{
			std::istream& in;
			int current=-1,end=-1;//how many chars read so far, we can get that from istream gcount???
			//could we use underlying streambuf??? using seekg? no because
			//we don't control when buffer gets loaded, 
			//enum {N=0x1000};//should it be a template parameter?
			enum {MASK=N-1};
			char buffer[N];
			source(std::istream& in):in(in){}
			char get(int index){
				//we can optimize here by accessing streambuf rather than reading one char at a time
				while(current<index){
					++current;
					//in.get(buffer[current&MASK]);	
					auto tmp=in.get();
					//buffer[current&MASK]=in.get();
					buffer[current&MASK]=tmp;
					//std::cerr<<index<<"\t"<<tmp<<"\t`"<<(char)tmp<<"'\t"<<(tmp==std::istream::traits_type::eof())<<"\teof:"<<in.eof()<<std::endl;
					if(in.eof()) end=current;
				}
				return buffer[index&MASK];
			}
		};
		std::shared_ptr<source> src;
		size_t index=0;
		value_type v=0;
		//we need end iterator, problem we don't know how many chars are available
		basic_char_iterator(){}
		basic_char_iterator(std::istream& in):src(new source(in)),v(src->get(0)){}
		basic_char_iterator& operator++(){
			++index;
			//v=src->get(index);
			return *this;
		}
		basic_char_iterator& operator+=(size_t i){
			index+=i;
			//v=src->get(index);
			return *this;
		}
		basic_char_iterator& operator--(){
			--index;
			//v=src->get(index);
			return *this;
		}
		basic_char_iterator& operator-=(size_t i){
			index-=i;
			//v=src->get(index);
			return *this;
		}
		bool operator==(const basic_char_iterator& c)const{
			return c.src ? index==c.index : index==src->end;//c is the end iterator
		}
		bool operator!=(const basic_char_iterator& c)const{
			return c.src ? index!=c.index : index!=src->end;//c is the end iterator
		}
		bool operator<(const basic_char_iterator& c)const {
			return c.src ? index < c.index : index < src->end;//c is the end iterator
		}
		//operator size_t(){return index;} is this of any use???
		value_type operator*(){
			return src->get(index);
			//return v;
		}
	};
	template<size_t N> basic_char_iterator<N> operator+(const basic_char_iterator<N>& c,size_t i){
		basic_char_iterator<N> tmp=c;
		return tmp+=i;
	}
	template<size_t N> basic_char_iterator<N> operator-(const basic_char_iterator<N>& c,size_t i){
		basic_char_iterator<N> tmp=c;
		return tmp-=i;
	}
	template<size_t N> typename basic_char_iterator<N>::difference_type operator-(const basic_char_iterator<N>& a,const basic_char_iterator<N>& b){
		return a.index-b.index;
	}
	//print
	template<size_t N> std::ostream& operator<<(std::ostream& os,const std::pair<bool,basic_char_iterator<N>>& p){
		return os<<"{"<<p.first<<","<<p.second.index<<"}";
	}
	typedef basic_char_iterator<> char_iterator;
}
#endif
