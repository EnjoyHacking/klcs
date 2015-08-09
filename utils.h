#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "lst_string.h"
#include <sys/stat.h>

//typedef void (*File_Content_Operate) (char* pathName, LST_StringSet *set);
void dirScan(char* dirName, LST_StringSet *set);
void  readContent(char* pathName, LST_StringSet *set);
#endif
