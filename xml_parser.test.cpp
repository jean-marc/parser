#include <list>
#include "parser.h"
#include "xml_parser.h"
#include "char_iterator.h"
using namespace parser;
using namespace std;
/*
 *
 */
struct my_handler:handler<
	char_iterator,
	std::tuple<
		xml_parser::element,
		//xml_parser::QName,
		xml_parser::NSAttName,
		xml_parser::AttValue
	>
>{
	struct ns{
		string prefix;
		string _ns;
		size_t depth;
	};
	list<ns> nss;
	/*
 	*	structure to store element name (prefix/localname) and attributes (prefix/localname/value)
 	*/ 
	size_t depth=0;
	pair<string,string> qname;
	void start(xml_parser::element){
		++depth;
		cout<<"start element"<<endl;
	}
	void stop(xml_parser::element,ITERATOR begin,ITERATOR end,bool v){
		--depth;
		cout<<"stop element\t"<<v<<endl;
	}
	void start(xml_parser::Name){}
	void stop(xml_parser::Name,ITERATOR begin,ITERATOR end,bool v){
		if(v) cout<<"Name:`"<<string(begin,end)<<"'"<<endl;
	}
	void start(xml_parser::NSAttName){

	}
	void stop(xml_parser::NSAttName,ITERATOR begin,ITERATOR end,bool v){
		if(v) cout<<"NSAttName:`"<<string(begin,end)<<"'"<<endl;
	}
	void start(xml_parser::AttValue){}
	void stop(xml_parser::AttValue,ITERATOR begin,ITERATOR end,bool v){
		if(v) cout<<"Attribute:`"<<string(begin+1,end-1)<<"'"<<endl;
	}
};
int main(){
	my_handler h;
	cerr<<xml_parser::document::go(char_iterator(cin),char_iterator(),h).first<<endl;
}
