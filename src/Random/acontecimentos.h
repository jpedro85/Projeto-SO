//Caso queiram enumerar os acontecimentos para evitar erros
//funciona da mesma maneira que os numeros, o simulationstart é 0, o terminated é 1 etc...
//Com este metodo, basta escrever mensagemAcontecimento (UserLeft) e reconhece UserLeft como o case (3)
enum Acontecimentos{
    SimulationStart,
    SimulationTerminated,
    UserJoined,
    UserLeft
};

char* mensagemAcontecimento(int acontecimento);