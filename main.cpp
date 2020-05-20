#include "common.h"
#include <iostream>
#include "vm.cpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>

using namespace std;

VM vm = VM();

static char* readFile(const char * path) {
    ifstream file (path, ifstream::binary);
    if (file) {
        // get length of file:
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);

        char * buffer = new char [length];
        if (buffer == NULL) {
            fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
            exit(74);
        }
        std::cout << "Reading " << length << " characters... ";

        // read data as a block:
        file.read (buffer,length);

        if (!file) {
            fprintf(stderr, "Could not read file \"%s\".\n", path);
            exit(74);
        }

        file.close();

    } else {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
}

void repl() {
    string line;
    for (;;) {
        printf("> ");

        if (!getline(cin, line)) {
            printf("\n");
            break;
        }

        vm.interpret(line.c_str());
    }
}

void runFile(const char * path) {
    char * source = readFile(path);
    InterpretResult result = vm.interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
    return 0;
}