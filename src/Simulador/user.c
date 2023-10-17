//meter os includes todos
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

static __thread int id;
static bool vipPass;

struct userArgs {
    int id;
    int idade;
    int vipPass;
    pthread_t current_attraction; // Assuming pthread_t is the type for a thread
};

void criarCliente(int waitTime){}
void removerCliente(){}
void chooseAction(){}
void chooseAttraction(){}
