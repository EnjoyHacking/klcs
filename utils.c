#include "utils.h"

int fileCounter = 0;

void dirScan(char* dirName, LST_StringSet *set){
	DIR * dir = NULL;
	struct dirent * file = NULL;
        char pathName[100];
	memset(pathName, '\0', 100);

        strcpy(pathName, dirName);
	if(dirName[strlen(dirName) - 1] != '/'){
                strcat(pathName, "/");
	}

	if(( dir = opendir(pathName)) == NULL){
		fprintf(stderr, "Can't open directory %s \n", pathName);
		exit(1);
	}


	while((file = readdir(dir)) != NULL){
		char* tmp = (char*)malloc(sizeof(char) * 100);
		memset(tmp, '\0', strlen(pathName));
		strcpy(tmp, pathName);
		if(file->d_type == DT_REG){
			// parse sample file
			fprintf(stdout, "parse sample file : %s%s ....\n", dirName, file->d_name);
			readContent(strcat(tmp, file->d_name), set);
                        fileCounter++;

		}else if(file->d_type == DT_DIR && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
			dirScan(strcat(tmp, file->d_name), set);
		}
	}


}
void  readContent(const char* pathName, LST_StringSet *set){
	FILE *f = NULL;
	u_char *data = NULL;
	struct stat st;
	LST_String *string = NULL;

	printf("Reading file %s.\n", pathName);

	if (stat(pathName, & st) < 0)
	{	  
		printf("Skipping file %s.\n", pathName);
		return;
	}

	data = (u_char *)malloc(st.st_size * sizeof(u_char));
	memset(data, '\0', st.st_size);
	if (!data)
	{
		printf("File %s too big, skipping.\n", pathName);
		return;
	}
	
	f = fopen(pathName, "rb");
	if (!f)
	{

		printf("Couldn't open %s, skipping.\n", pathName);
		free(data);
		data = NULL;
		return;
	}

	if (fread(data, 1, st.st_size, f) != (size_t) st.st_size)
	{
		printf("Error reading %s -- skipping.\n", pathName);
		free(data);
		data = NULL;
		return ;
	}

	string = lst_string_new(data, 1, st.st_size);

	lst_stringset_add(set, string);

	if(data){
		free(data);
		data = NULL;
	}

	if(pathName){
		free(pathName);
		pathName = NULL;
	}

	fclose(f);
	return;
}
