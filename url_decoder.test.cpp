#include "parser.h"
#include "char_iterator.h"
#include "url_decoder.h"

using namespace parser;
using namespace std;
int main(){
	url_decoder::my_handler<string::const_iterator> h;//would be nice if we did not have to do that
	string s;
	getline(cin,s);
	auto r=url_decoder::word::go(s.cbegin(),s.cend(),h);//why does it stop early?
	cerr<<r.first<<"\t`"<<h.decoded<<"'"<<endl;
}
