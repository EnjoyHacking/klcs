
#include "position_constraints.h"
#include "trie.h"


#if 1
int main(int argc, char * argv[]) {
	char *str1 = "HTTP1.1 index.html 200 a HTTP";
	char *str2 = "HTTP1.0 index.html HTTP b 200";
	char *str3 = "GET index.html";
	char *str4 = "POST index.html";
	char *str5 = "GET login.html";
	char *str6 = "POST logout.html";

	LST_String * payload1 = lst_string_new(str1, 1, strlen(str1));
	LST_String * payload2 = lst_string_new(str2, 1, strlen(str2));
	LST_String * payload3 = lst_string_new(str3, 1, strlen(str3));
	LST_String * payload4 = lst_string_new(str4, 1, strlen(str4));
	LST_String * payload5 = lst_string_new(str5, 1, strlen(str5));
	LST_String * payload6 = lst_string_new(str6, 1, strlen(str6));

	LST_StringSet *payloads = lst_stringset_new();
	lst_stringset_add(payloads, payload1);
	lst_stringset_add(payloads, payload2);
	lst_stringset_add(payloads, payload3);
	lst_stringset_add(payloads, payload4);
	lst_stringset_add(payloads, payload5);
	lst_stringset_add(payloads, payload6);

	char *token1 = "HTTP";
	char *token2 = "200";
	char *token3 = "GET";
	char *token4 = "POST";
	LST_StringSet *tokens = lst_stringset_new();
	lst_stringset_add(tokens, lst_string_new(token1, 1, strlen(token1)));
	lst_stringset_add(tokens, lst_string_new(token2, 1, strlen(token2)));
	lst_stringset_add(tokens, lst_string_new(token3, 1, strlen(token3)));
	lst_stringset_add(tokens, lst_string_new(token4, 1, strlen(token4)));

	int k_offset = 1;
	int beta_merge = 5;
	position_constraints_main(payloads, tokens, k_offset, beta_merge);

	return 0;

}
#endif

