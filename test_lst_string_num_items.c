#include "lst_string.h"
#include <string.h>



int main(void) {

	char *str = "hello world";
	LST_String *string = lst_string_new(str, 1, strlen(str));

	printf("num_items : %d \n", string->num_items);
	printf("strlen : %d \n", strlen((char *)string->data));

	return 0;
}



