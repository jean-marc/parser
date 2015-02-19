#ifndef XML_PARSER_H
#define XML_PARSER_H
#include "parser.h"
namespace parser{
	struct xml_parser{
		typedef _pl<_or<_rc<'a','z'>,_rc<'A','Z'>,_rc<'0','9'>,_c<'_'>,_c<'-'>>> nname;
		typedef _or<_c<' '>,_c<'\n'>,_c<'\r'>,_c<'\t'>> space;
		typedef nname element_name;
		typedef nname attribute_name;
		typedef _sq<_c<'\''>,_kl<_nt<_c<'\''>>>,_c<'\''>> att_single_quote;
		typedef _sq<_c<'\"'>,_kl<_nt<_c<'\"'>>>,_c<'\"'>> att_double_quote;
		typedef _sq<attribute_name,_c<'='>,_or<att_single_quote,att_double_quote>> attribute;
		typedef _kl<_sq<_pl<space>,attribute>> attributes;//white space
		typedef _sq<_c<'<'>,element_name,attributes> start_tag;
		typedef _sq<_c<'<'>,_c<'/'>,element_name,_c<'>'>> end_tag;
		typedef _sq<_c<'/'>,_c<'>'>> empty_end_tag;
		typedef event<_pl<_nt<_c<'<'>>>> text;
		typedef _sqc<'<','!','-','-'> start_comment;
		typedef _sqc<'-','-','>'> stop_comment;
		typedef _sq<start_comment,_kl<_nt<stop_comment>>,stop_comment> comment;
		struct element:_sq<
				start_tag,_or<
					_sq<_c<'>'>,_kl<_or<element,comment,text>>,end_tag>, //<abc ...>...</abc>
					_sq<_c<'/'>,_c<'>'>>
				>
			>{};


	};	
}
#endif
