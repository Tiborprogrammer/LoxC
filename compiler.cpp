#include "common.h"
#include "scanner.cpp"

class Compiler {
public:
    Scanner scanner;
    void compile(const char *source) {
        scanner.initScanner(source);
    }
};
