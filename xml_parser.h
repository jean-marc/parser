#ifndef XML_PARSER_H
#define XML_PARSER_H
#include "parser.h"
namespace parser{
	/*
 	* from:
 	* 	http://www.w3.org/TR/REC-xml/
	*	http://www.w3.org/TR/xml-names/
	* how can we support different encoding?
	*/
	struct xml_parser{
		typedef _pl<WS> S;
		typedef _sq<_sqc<'1','.'>,_rc<'0','9'>> VersionNum;
		typedef _c<'='> Eq;
		typedef _sq<S,_sqc<'v','e','r','s','i','o','n'>,Eq,_or<_sq<_c<'\''>,VersionNum,_c<'\''>>,_sq<_c<'\"'>,VersionNum,_c<'\"'>>>> VersionInfo;
		typedef NOT_IMPLEMENTED EncodingDecl;
		typedef NOT_IMPLEMENTED SDDecl; 
		typedef _sq<_sqc<'<','?','x','m','l'>,VersionInfo,_op<EncodingDecl>,_op<SDDecl>,_op<S>,_sqc<'?','>'>> XMLDecl;
		typedef NOT_IMPLEMENTED PI;
		typedef _sq<_sqc<'<','!','-','-'>,_sqc<'-','-','>'>> Comment;
		typedef _or<Comment,PI,S> Misc;
		typedef NOT_IMPLEMENTED doctypedecl;
		typedef _sq<_op<XMLDecl>,_kl<Misc>,_op<_sq<doctypedecl,_kl<Misc>>>> prolog;
		typedef _or</*_c<':'>,*/_rc<'A','Z'>,_c<'_'>,_rc<'a','z'>> NameStartChar;//could reorder
		typedef _or<NameStartChar,_c<'-'>,_c<'.'>,_rc<'0','9'>> NameChar;
		typedef _sq<NameStartChar,_kl<NameChar>> Name;
		typedef Name NCName; /* an XML Name minus `:' */
		typedef _or<
			_sq<_c<'\"'>,_kl<_nt<_or<_c<'<'>/*,_c<'&'>*/,_c<'\"'>>>>,_c<'\"'>>,
			_sq<_c<'\''>,_kl<_nt<_or<_c<'<'>/*,_c<'&'>*/,_c<'\''>>>>,_c<'\''>>
		> AttValue;
		typedef _sq<_sqc<'x','m','l','n','s'>,_c<':'>,NCName> PrefixedAttName;
		typedef _sqc<'x','m','l','n','s'> DefaultAttName;
		typedef _or<PrefixedAttName,DefaultAttName> NSAttName;
		struct Prefix:NCName{};
		struct LocalPart:NCName{};
		typedef LocalPart UnprefixedName;
		typedef _sq<Prefix,_c<':'>,LocalPart> PrefixedName;
		typedef _or<PrefixedName,UnprefixedName> QName;
		typedef _or<_sq<NSAttName,Eq,AttValue>,_sq</*Name*/QName,Eq,AttValue>> Attribute;
		//annoying because causes Attributes to fire 
		typedef _sq<_c<'<'>,/*Name*/QName,_kl<_sq<S,Attribute>>,_op<S>,_sqc<'/','>'>> EmptyElemTag;
		typedef _sq<_c<'<'>,/*Name*/QName,_kl<_sq<S,Attribute>>,_op<S>,_c<'>'>> STag;
		typedef _sq<_sqc<'<','/'>,/*Name*/QName,_op<S>,_c<'>'>> ETag;
		typedef NOT_IMPLEMENTED Reference;
		typedef NOT_IMPLEMENTED CDSect;
		typedef _kl<_nt<_or<_c<'<'>,_c<'&'>>>> CharData;
		template<typename ELEMENT> struct content:_sq<_op<CharData>,_kl<_sq<_or<ELEMENT,Reference,CDSect,PI,Comment>,_op<CharData>>>>{};
		//not very efficient,because re-parses start tag when non empty element 
		struct element:_or<_sq<STag,content<element>,ETag>,EmptyElemTag>{};
		/*
		//what about:
		struct element:_sq<_c<'<'>,QName,_kl<_sq<S,Attribute>>,_op<S>,_or<
				_sqc<'/','>'>,//empty element
				_sq<_c<'>'>,content<element>,ETag>>
		>{};
		*/
		typedef _sq<prolog,element,_kl<Misc>> document;
	};	
}
#endif
