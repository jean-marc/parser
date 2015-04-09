#ifndef SPARQL_PARSER_H
#define SPARQL_PARSER_H
/*
 *	PREFIX foaf:   <http://xmlns.com/foaf/0.1/>
 *	SELECT ?name ?mbox
 *	WHERE
 *	  { ?x foaf:name ?name .
 *	      OPTIONAL{?x foaf:mbox ?mbox}}

 *
 */
namespace parser{
	//from http://www.w3.org/TR/2013/REC-sparql11-query-20130321/#sparqlGrammar
	struct sparql_parser{
		typedef _f NOT_IMPLEMENTED;
		typedef NOT_IMPLEMENTED BaseDecl;//not implemented yet	
		typedef _or<_rc<'A','Z'>,_rc<'a','z'>> PN_CHARS_BASE;
		typedef _or<PN_CHARS_BASE,_c<'_'>> PN_CHARS_U;
		typedef _or<PN_CHARS_U,_c<'-'>,_rc<'0','9'>> PN_CHARS;
		typedef _pl<_or<PN_CHARS_U,_rc<'0','9'>>> VARNAME;
		//parser can not handle that
		//typedef _sq<PN_CHARS_BASE,/*_or<*/_sq<_kl<_or<PN_CHARS,_c<'.'>>>,PN_CHARS>/*,_t>*/> PN_PREFIX;
		typedef _sq<PN_CHARS_BASE,_kl<_or<PN_CHARS,_c<'.'>>>> PN_PREFIX;
		//parser can not handle that
		//typedef _sq<_or<PN_CHARS_U,_rc<'0','9'>>,_or<_sq<_kl<_or<PN_CHARS,_c<'.'>>>,PN_CHARS>,_t>> PN_LOCAL;
		typedef _sq<_or<PN_CHARS_U,_rc<'0','9'>>,_kl<_or<PN_CHARS,_c<'.'>>>> PN_LOCAL;
		typedef _sq<_or<PN_PREFIX,_t>,_c<':'>> PNAME_NS;
		typedef _sq<_c<'<'>,_pl<_nt<_c<'>'>>>,_c<'>'>> IRI_REF;
		typedef _sq_ws<_sqc<'P','R','E','F','I','X'>,PNAME_NS,IRI_REF> PrefixDecl;
		typedef _sq_ws<_or<BaseDecl,_t>,_kl<PrefixDecl>> Prologue;
		typedef _sq<_or<_c<'$'>,_c<'?'>>,VARNAME> Var;
		typedef _f DataSetClause;
		typedef _sq<PNAME_NS,PN_LOCAL> PNAME_LN;
		typedef /*_or<*/PNAME_LN/*,PNAME_NS>*/ PrefixedName;
		typedef _or<IRI_REF,PrefixedName> IRIref;
		typedef NOT_IMPLEMENTED LANGTAG;
		typedef NOT_IMPLEMENTED iri;
		typedef NOT_IMPLEMENTED ECHAR;
		typedef _sq<_c<'\''>,_kl<_or<_nt<_or<_c<'\''>,_c<'\\'>,_c<'\n'>,_c<'\r'>>>,ECHAR>>,_c<'\''>> STRING_LITERAL1;
		typedef _sq<_c<'\"'>,_kl<_or<_nt<_or<_c<'\"'>,_c<'\\'>,_c<'\n'>,_c<'\r'>>>,ECHAR>>,_c<'\"'>> STRING_LITERAL2;
		typedef NOT_IMPLEMENTED STRING_LITERAL_LONG1;
		typedef NOT_IMPLEMENTED STRING_LITERAL_LONG2;
		typedef _or<
			STRING_LITERAL1,
			STRING_LITERAL2,
			STRING_LITERAL_LONG1,
			STRING_LITERAL_LONG2
		> String;
		typedef _sq<String,_or<LANGTAG,_sq<_sqc<'^','^'>,iri>,_t>> RDFLiteral;
		typedef NOT_IMPLEMENTED NumericLiteral;
		typedef _or<_sqc<'t','r','u','e'>,_sqc<'f','a','l','s','e'>> BooleanLiteral;
		typedef NOT_IMPLEMENTED BlankNode;
		typedef NOT_IMPLEMENTED NIL;
		typedef _or<IRIref,RDFLiteral,NumericLiteral,BooleanLiteral,BlankNode,NIL> GraphTerm;
		typedef _or<Var,GraphTerm> VarOrTerm;
		typedef _or<Var,IRIref> VarOrIRIref;
		typedef _or<VarOrIRIref,_c<'a'>> Verb;
		typedef /*_or<*/VarOrTerm/*,TriplesNode>*/ GraphNode;
		typedef GraphNode Object;
		typedef _sq_ws<Object,_kl<_sq_ws<_c<','>,Object>>> ObjectList;
		typedef _sq_ws<Verb,ObjectList,_kl<_sq_ws<_c<';'>,Verb,ObjectList>>> PropertyListNotEmpty;
		//literal subject: see http://www.w3.org/TR/2013/REC-sparql11-query-20130321/#sparqlTriplePatterns
		typedef _sq_ws<VarOrTerm,PropertyListNotEmpty/*TriplesNode,PropertyList*/> TriplesSameSubject;
		struct TriplesBlock:_sq_ws<TriplesSameSubject,_or<_sq_ws<_c<'.'>,TriplesBlock>,_t>>{};//recursive
		template<typename T> struct GroupOrUnionGraphPattern:_sq_ws<T,_kl<_sq_ws<_sqc<'U','N','I','O','N'>,T>>>{};
		template<typename T> struct OptionalGraphPattern:_sq_ws<_sqc<'O','P','T','I','O','N','A','L'>,T>{};
		typedef NOT_IMPLEMENTED MinusGraphPattern;
		typedef NOT_IMPLEMENTED GraphGraphPattern;
		typedef NOT_IMPLEMENTED ServiceGraphPattern;
		typedef NOT_IMPLEMENTED Filter;
		typedef NOT_IMPLEMENTED Bind;
		typedef NOT_IMPLEMENTED InlineData;
		template<typename T> struct GraphPatternNotTriples:_or<
			GroupOrUnionGraphPattern<T>,
			OptionalGraphPattern<T>,
			MinusGraphPattern,
			GraphGraphPattern,
			ServiceGraphPattern,
			Filter,
			Bind,
			InlineData
		>{};
		template<typename T> struct GroupGraphPatternSub:_sq_ws<
			_or<TriplesBlock,_t>,
			_kl<_sq_ws<_or<GraphPatternNotTriples<T>,Filter>,_or<_c<'.'>,_t>,_or<TriplesBlock,_t>>>
		>{}; 
		typedef NOT_IMPLEMENTED SubSelect;
		struct GroupGraphPattern:_sq_ws<
			_c<'{'>,
			_or<SubSelect,GroupGraphPatternSub<GroupGraphPattern>>,
			_c<'}'>
		>{};
		typedef _sq_ws</*_or<*/_sqc<'W','H','E','R','E'>/*,_t>*/,GroupGraphPattern> WhereClause;
		typedef _t SolutionModifier;
		typedef _sq_ws<
			_sqc<'S','E','L','E','C','T'>,
			_or<_sqc<'D','I','S','T','I','N','C','T'>,_sqc<'R','E','D','U','C','E','D'>,_t>,
			_or<_pl_ws<Var>,_c<'*'>>,
			_kl<DataSetClause>,
			WhereClause,
			SolutionModifier
		> SelectQuery;
		typedef NOT_IMPLEMENTED ConstructQuery;
		typedef NOT_IMPLEMENTED DescribeQuery;
		typedef NOT_IMPLEMENTED AskQuery;
		typedef _sq_ws<Prologue,_or<SelectQuery,ConstructQuery,DescribeQuery,AskQuery>> Query;
	};
}
#endif
