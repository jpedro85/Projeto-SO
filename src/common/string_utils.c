#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdbool.h>

int strContain_Substring (char* str1 , char* str2) {

    char* str_searched = strchr(str1, str2[0] );

    if(str_searched != NULL){

        int i = 1;
        
        while(str_searched[i] != '\0' && str2[i] != '\0') {
           
            if(str_searched[i] != str2[i]){
                return -1;
            }

            i++;

        }
        return i;

    }else
        return -1;
}

bool strStartWith (char* str1 , char* str2) {

    if(str1 != NULL && str2 != NULL){

        int i = 0;
        while(str1[i] != '\0' && str2[i] != '\0') {
           
            if(str1[i] != str2[i]) return false;

            i++;
        }
        return true;

    } else {
        printf("strIsSubstring: Can not resolve string NULL pointers.");
        return false;
    }
}

char* strsub(char* str, int start ,int end){

    int i = 0;
    char* substr = (char*)malloc( sizeof(char)*(end - start) );

    while( str[i] != '\0' && i < end ){
        substr[i] = str[i];
        i++;
    }

    return substr;
}

char* strSplit( char** str , char split_at ){

    int i = 0;
    char* str_spliced = NULL;
    while( (*str)[i] != '\0' ){

        if( (*str)[i] == split_at){

            // aaaa bbbb cccc\0
            (*str)[i] = '\0'; // terminate the firs string // aaaa\0bbbb cccc\0
            str_spliced = *str; // pointer to aaaa\0
            *str = &(*str)[++i]; // pointer to bbbb cccc\0
            
            return str_spliced;
        }

        i++;
    }
    return str_spliced;
}


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define insane_free(ptr) { free(ptr); ptr = 0; }

int vasprintf(char **strp, const char *fmt, va_list ap)
{
    int r = -1, size;

    va_list ap2;
    va_copy(ap2, ap);

    size = vsnprintf(0, 0, fmt, ap2);

    if ((size >= 0) && (size < INT_MAX))
    {
        *strp = (char *)malloc(size + 1); //+1 for null
        if (*strp)
        {
            r = vsnprintf(*strp, size + 1, fmt, ap); //+1 for null
            if ((r < 0) || (r > size))
            {
                insane_free(*strp);
                r = -1;
            }
        }
    }
    else
    {
        *strp = 0;
    }

    va_end(ap2);

    return (r);
}

/**
 * The function `asprintf` is a variadic function that allocates a string and formats it according to
 * the provided format string and arguments.
 * 
 * @param strp strp is a pointer to a pointer to a character array. It is used to store the resulting
 * string created by the function.
 * @param fmt The `fmt` parameter is a format string that specifies how the subsequent arguments should
 * be formatted and inserted into the resulting string. It is similar to the format string used in
 * functions like `printf` and `sprintf`.
 * 
 * @return an integer value.
 */
int asprintf(char **strp, const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = vasprintf(strp, fmt, ap);
    va_end(ap);
    return (r);
}