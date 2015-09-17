#include "parser.h"
#include "sparql_parser.h"
#include "char_iterator.h"
#include <objrdf/objrdf.h>
#include <map>
#include <set>
#include <algorithm>
using namespace parser;
using namespace std;

/*
 *	simplest representation of graphs
 */
//using namespace objrdf;
struct node;
typedef shared_ptr<node> NODE_PTR;
enum type{LIT,VAR,IRI,PN,A,BOOL};
struct property{
	type t;
	string s;
	bool optional;//=false;
	objrdf::CONST_PROPERTY_PTR p;
	NODE_PTR object;
	friend ostream& operator<<(ostream& os,const property& p);
};
struct node{
	type t;
	string s;
	bool val;//if boolean
	vector<property> properties;
	void name(ostream& os) const{
		if(t==type::BOOL)
			os<<val;
		else if(t==type::IRI)
			os<<"\e[0;1m<"<<s<<">\e[0m";
		else if(t==type::VAR)
			os<<"?"<<s;
		else
			os<<s;
	}
	friend ostream& operator<<(ostream& os,const node& n){
		n.name(os);
		for(auto i:n.properties){
			if(i.optional) 
				os<<"\n\e[0;32m `--> "<<i<<"\e[0m";
			else
				os<<"\n `--> "<<i;
		}
		return os;
	}
	//evaluate BGP basic graph pattern, 8 possibilities
	void bgp(){
		if(t==type::VAR){
			/*
 			*	find a property that is bound and start joining based on property domain
 			*	maybe rdf:type present 
 			*/ 
			for(auto p:properties){	
				if(p.t!=type::VAR){
					if(p.p || (p.p=objrdf::find_t<rdf::Property>(objrdf::uri::hash_uri(p.s)))){
						cout<<"Property: `"<<p.p->id<<"'"<<endl;
						auto ds=p.p->cget<rdf::Property::domains>();
						for(auto& d:ds)
							cout<<"Property domain: `"<<d->id<<"'"<<endl;
					}
				}
			}
		}else{

		}
	}
};
ostream& operator<<(ostream& os,const property& p){
	if(p.t==type::IRI)
		os<<"\e[0;1m<"<<p.s<<">\e[0m";
	else if(p.t==type::VAR)
		os<<"?"<<p.s;
	else
		os<<p.s;
	if(p.object) os<<"\t--> ";p.object->name(os);
	return os;
}
/*
struct literal:node{
	string val;
};
struct reference:node{
	string s;
	vector<property> properties;
};
*/
struct my_handler:handler<
	char_iterator,
	std::tuple<
		sparql_parser::PrefixDecl,
		sparql_parser::PN_PREFIX,
		sparql_parser::PN_LOCAL,
		sparql_parser::IRI_REF,
		sparql_parser::PrefixedName,
		sparql_parser::Var,
		sparql_parser::WhereClause,
		sparql_parser::String,
		sparql_parser::Subject,	
		sparql_parser::Verb,	
		sparql_parser::a,
		sparql_parser::Object,
		sparql_parser::BooleanLiteral,
		sparql_parser::OptionalGraphPattern<sparql_parser::GroupGraphPattern>
		
	>
>{
	/*
 	*	would be nice to know where we are in the stack
 	*/

	/*
 	*	let's build a map of prefix/namespace
 	*	careful: we might have empty prefix!
 	*/ 
	string prefix,ns,pn_local;
	map<string,string> prefix_ns;
	set<string> variables;
	/*
 	*	graph to represent WHERE statement
 	*/ 
	vector<NODE_PTR> nodes;
	NODE_PTR current_subject;
	property current_property;
	my_handler(){}
	my_handler(ITERATOR begin,ITERATOR end):SELF({begin,end}){}
	//could use enum because some of it mutually exclusive
	bool in_PrefixDecl=false;
	bool in_PrefixedName=false;
	bool in_WhereClause=false;
	bool in_Subject=false;
	bool in_Verb=false;
	bool in_Object=false;
	bool in_Optional=false;
	void start(sparql_parser::OptionalGraphPattern<sparql_parser::GroupGraphPattern>){in_Optional=true;}
	void stop(sparql_parser::OptionalGraphPattern<sparql_parser::GroupGraphPattern>,ITERATOR,ITERATOR,bool){in_Optional=false;}
	void start(sparql_parser::PrefixDecl){in_PrefixDecl=true;}
	void stop(sparql_parser::PrefixDecl,ITERATOR,ITERATOR,bool){in_PrefixDecl=false;}
	void start(sparql_parser::PN_LOCAL){}
	void stop(sparql_parser::PN_LOCAL,ITERATOR begin,ITERATOR end,bool v){if(v) pn_local=string(begin,end);}
	void start(sparql_parser::PrefixedName){in_PrefixedName=true;}
	void stop(sparql_parser::PrefixedName,ITERATOR begin,ITERATOR end,bool v){
		in_PrefixedName=false;
		if(v){
			cout<<"PrefixedName:`"<<string(begin,end)<<"'"<<endl;
			if(in_WhereClause){
				auto i=prefix_ns.find(prefix);
				if(i==prefix_ns.end()){
					cout<<"prefix `"<<prefix<<"' not found"<<endl;
					exit(1);
				}
				string iri=i->second+pn_local;
				if(in_Subject){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::IRI)&&(n->s==iri);});
					if(i!=nodes.end()){
						current_subject=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::IRI,iri})));
						current_subject=nodes.back();
					}
				}else if(in_Verb){
					current_property={type::IRI,iri,in_Optional};	
				}else if(in_Object){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::IRI)&&(n->s==iri);});
					if(i!=nodes.end()){
						current_property.object=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::IRI,iri})));
						current_property.object=nodes.back();
					}
					current_subject->properties.push_back(current_property);
				}
			}
		}
		prefix.clear();
	}
	void start(sparql_parser::PN_PREFIX){}
	void stop(sparql_parser::PN_PREFIX,ITERATOR begin,ITERATOR end,bool v){if(v) prefix=string(begin,end);}
	void start(sparql_parser::IRI_REF){}
	void stop(sparql_parser::IRI_REF,ITERATOR begin,ITERATOR end,bool v){
		if(v){
			string iri(begin+1,end-1);//because <>
			if(in_PrefixDecl){
				prefix_ns[prefix]=iri;prefix.clear();
			}else if(in_WhereClause){
				if(in_Subject){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::IRI)&&(n->s==iri);});
					if(i!=nodes.end()){
						current_subject=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::IRI,iri})));
						current_subject=nodes.back();
					}
				}else if(in_Verb){
					current_property={type::IRI,iri,in_Optional};	
				}else if(in_Object){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::IRI)&&(n->s==iri);});
					if(i!=nodes.end()){
						current_property.object=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::IRI,iri})));
						current_property.object=nodes.back();
					}
					current_subject->properties.push_back(current_property);
				}
			}
		}
	}
	void start(sparql_parser::Var){}
	void stop(sparql_parser::Var,ITERATOR begin,ITERATOR end,bool v){
		if(v){
			string var(begin+1,end);
			if(in_WhereClause){
				if(in_Subject){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::VAR)&&(n->s==var);});
					if(i!=nodes.end()){
						current_subject=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::VAR,var})));
						current_subject=nodes.back();
					}
				}else if(in_Verb){
					current_property={type::VAR,var,in_Optional};	
				}else if(in_Object){
					auto i=find_if(nodes.begin(),nodes.end(),[&](NODE_PTR n){return (n->t==type::VAR)&&(n->s==var);});
					if(i!=nodes.end()){
						current_property.object=*i;
					}else{
						nodes.push_back(NODE_PTR(new node({type::VAR,var})));
						current_property.object=nodes.back();
					}
					current_subject->properties.push_back(current_property);
				}
			}else{
				variables.insert(var);
			}
		}
	}
	void start(sparql_parser::a){}
	void stop(sparql_parser::a,ITERATOR begin,ITERATOR end,bool v){
		if(v){
			current_property={type::A,string(begin,end),in_Optional,rdf::type::get_property()};	
		}
	}
	void start(sparql_parser::WhereClause){in_WhereClause=true;}
	void stop(sparql_parser::WhereClause,ITERATOR,ITERATOR,bool){in_WhereClause=false;}
	void start(sparql_parser::String){}
	void stop(sparql_parser::String,ITERATOR begin,ITERATOR end,bool v){
		if(v){
			if(in_Object){
				nodes.push_back(NODE_PTR(new node({type::LIT,string(begin+1,end-1)})));
				current_property.object=nodes.back();
				current_subject->properties.push_back(current_property);
			}
		}
	}
	void start(sparql_parser::BooleanLiteral){}
	void stop(sparql_parser::BooleanLiteral,ITERATOR begin,ITERATOR end,bool v){
		if(v){
			if(in_Object){
				nodes.push_back(NODE_PTR(new node({type::BOOL,string(begin,end),*begin=='t'})));
				current_property.object=nodes.back();
				current_subject->properties.push_back(current_property);
			}
		}
	}
	/*
 	*	subject can be a variable, a IRI or a blank node
 	*/
	void start(sparql_parser::Subject){in_Subject=true;}
	void stop(sparql_parser::Subject,ITERATOR begin,ITERATOR end,bool v){
		in_Subject=false;
		if(v) cout<<"Subject:`"<<string(begin,end)<<"'"<<endl;
	}
	void start(sparql_parser::Verb){in_Verb=true;}
	void stop(sparql_parser::Verb,ITERATOR begin,ITERATOR end,bool v){
		in_Verb=false;
		if(v) cout<<"\tVerb:`"<<string(begin,end)<<"'"<<endl;
	}
	void start(sparql_parser::Object){in_Object=true;}
	void stop(sparql_parser::Object,ITERATOR begin,ITERATOR end,bool v){
		in_Object=false;
		if(v) cout<<"\t\tObject:`"<<string(begin,end)<<"'"<<endl;
	}
};
int main(){
	/*
	my_handler h({char_iterator(cin),char_iterator()});
	h.instance=&h;
	cerr<<sparql_parser::Query::go(h).first<<endl;
	*/
	my_handler h;
	cout<<"parsing:"<<sparql_parser::Query::go(char_iterator(cin),char_iterator(),h).first<<endl;
	cout<<endl;
	//cerr<<sparql_parser::PrefixDecl::go(char_iterator(cin),char_iterator(),h).first<<endl;
	for(auto i:h.prefix_ns)
		cout<<i.first<<"\t->"<<i.second<<endl;
	cout<<endl;
	for(auto i:h.variables)
		cout<<i<<endl;
	cout<<endl;
	for(auto i:h.nodes)
		cout<<*i<<endl;
	for(auto i:h.nodes) i->bgp();
}
