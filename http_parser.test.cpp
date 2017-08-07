#include <string>
using namespace std;
#include "http_parser.h"
int main(){
	string s=
		"HTTP/1.1 200 OK\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Length: 4\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Type: application/javascript\r\n"
		"Date: Sun, 06 Aug 2017 00:59:01 GMT\r\n"
		"\r\n"
		"true\r\n";
	http_parser::my_handler<string::const_iterator> h;//would be nice if we did not have to do that
	auto r=http_parser::response::go(s.cbegin(),s.cend(),h);
	cerr<<"parsing `"<<s<<"'\n:"<<r.first<<endl;
	for(auto& header:h.headers)
		cerr<<header.first<<":"<<header.second<<"\n";
}
