#ifndef clox_compiler_h
#define clox_compiler_h

#include "common.h"
#include "scanner.h"

class Compiler {
public:
    Scanner scanner;
    void compile(const char *source);
};

#endif   