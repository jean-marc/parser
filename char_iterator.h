#ifndef CHAR_ITERATOR_H
#define CHAR_ITERATOR_H
#include <vector>
#include <memory>
#include <iostream>
/*
 *	similar to istream_iterator except you can go backwards
 *	and make copies
 */
namespace parser{
	struct char_iterator{
		typedef std::random_access_iterator_tag iterator_category;//might be more than what we need
		typedef char value_type;
		typedef char& reference;
		typedef std::ptrdiff_t difference_type;
		typedef char_iterator pointer;
		
		struct source{
			std::istream& in;
			int current=-1,end=-1;//how many chars read so far, we can get that from istream gcount???
			//could we use underlying streambuf??? using seekg? no because
			//we don't control when buffer gets loaded, 
			enum {N=0x100};
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
					std::cerr<<tmp<<"\t`"<<(char)tmp<<"'\t"<<(tmp==std::istream::traits_type::eof())<<"\teof:"<<in.eof()<<std::endl;
					if(in.eof()) end=current;
				}
				return buffer[index&MASK];
			}
		};
		std::shared_ptr<source> src;
		size_t index=0;
		value_type v=0;
		//we need end iterator, problem we don't know how many chars are available
		char_iterator(){}
		char_iterator(std::istream& in):src(new source(in)),v(src->get(0)){}
		char_iterator& operator++(){
			++index;
			v=src->get(index);
			return *this;
		}
		char_iterator& operator+=(size_t i){
			index+=i;
			v=src->get(index);
			return *this;
		}
		char_iterator& operator--(){
			--index;
			v=src->get(index);
			return *this;
		}
		char_iterator& operator-=(size_t i){
			index-=i;
			v=src->get(index);
			return *this;
		}
		bool operator==(const char_iterator& c)const{
			return c.src ? index==c.index : index==src->end;//c is the end iterator
		}
		bool operator!=(const char_iterator& c)const{
			return c.src ? index!=c.index : index!=src->end;//c is the end iterator
		}
		//operator size_t(){return index;} is this of any use???
		value_type operator*(){
			//return src->get(index);
			return v;
		}
	};
	char_iterator operator+(const char_iterator& c,size_t i){
		char_iterator tmp=c;
		return tmp+=i;
	}
	char_iterator operator-(const char_iterator& c,size_t i){
		char_iterator tmp=c;
		return tmp-=i;
	}
	char_iterator::difference_type operator-(const char_iterator& a,const char_iterator& b){
		return a.index-b.index;
	}
	//print
	std::ostream& operator<<(std::ostream& os,const std::pair<bool,char_iterator>& p){
		return os<<"{"<<p.first<<","<<p.second.index<<"}";
	}
}
#endif
