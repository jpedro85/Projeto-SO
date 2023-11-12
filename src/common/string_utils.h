#include <stdbool.h>
//return true if str1 start with str2
bool strStartWith (char* str1 , char* str2);
//return de i number of char of str2 that are in str
int strContain_Substring (char* str1 , char* str2);
//substring
char* strsub(char* str, int start ,int end);
// splits the str at split_at modifying str to point to after split_at char  return null if split_at not found in str
char* strSplit( char** str , char split_at );

int asprintf(char **strp, const char *fmt, ...);