#include <stdio.h>
#include <string.h>

char *get_filename(const char *str)
{
	char *str_start = strstr(str, "GET /");
	
	if (str_start != NULL) {
		char *filename = str_start + strlen("GET /");
		char *str_end = strchr(filename, ' ');
		if (str_end != NULL) 
			*str_end = '\0';
		return filename;
	} else {
		return NULL;
	}
}

int main()
{
	char str[] = "GET /1.txt HTTP/1.1";

	char *filename = get_filename(str);

	printf("filename = %s\n", filename);

	return 0;
}
