#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

