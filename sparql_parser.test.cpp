#include "parser.h"
#include "sparql_parser.h"
#include "char_iterator.h"
using namespace parser;
using namespace std;

struct my_handler:handler<
	char_iterator,
	std::tuple<
		sparql_parser::PN_PREFIX,
		sparql_parser::IRI_REF
	>
>{
	/*
 	*	let's build a map of prefix/namespace
 	*	careful: we might have empty prefix!
 	*/ 
	my_handler(char_iterator begin,char_iterator end):SELF({begin,end}){}
	static void process(sparql_parser::PN_PREFIX,char_iterator begin,char_iterator end){
		cout<<"PN_PREFIX:"<<string(begin,end)<<endl;
	}
	static void process(sparql_parser::IRI_REF,char_iterator begin,char_iterator end){
		cout<<"IRI_REF:"<<string(begin,end)<<endl;
	}
};
int main(){
	cerr<<sparql_parser::Query::go(my_handler({char_iterator(cin),char_iterator()})).first<<endl;
}
