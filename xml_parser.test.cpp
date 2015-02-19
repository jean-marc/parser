#include "parser.h"
#include "xml_parser.h"
#include "char_iterator.h"
using namespace parser;
using namespace std;

struct my_handler:handler<
	char_iterator,
	std::tuple<
		xml_parser::attribute_name,
		xml_parser::s_value,
		xml_parser::d_value
	>
>{
	static string current_att;
	my_handler(char_iterator begin,char_iterator end):SELF({begin,end}){}
	static void process(xml_parser::attribute_name,char_iterator begin,char_iterator end){
		current_att=string(begin,end);
	}
	static void process(xml_parser::s_value,char_iterator begin,char_iterator end){
		attribute(current_att,string(begin,end));
	}
	static void process(xml_parser::d_value,char_iterator begin,char_iterator end){
		attribute(current_att,string(begin,end));
	}
	static void attribute(string name,string value){
		cerr<<name<<"->`"<<value<<"'"<<endl;
	}
};
string my_handler::current_att;
int main(){
	cerr<<xml_parser::document::go(my_handler({char_iterator(cin),char_iterator()})).first<<endl;
}
