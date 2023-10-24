#include <unistd.h>

#include "../consoleAddons.h"


int main(int argc , char *argv[] ){

    printInfo("Monitor Started !");

    usleep(2000*1000);
    for (int i = 0; i < argc; i++)
    {
        printInfo("Monitor reading : ");
        printOption(argv[i]);
        printInfo("\n");
    }
    
    printInfo("Monitor exiting");
    return 0;
}