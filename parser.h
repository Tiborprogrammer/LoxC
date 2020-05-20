//
// Created by Tibor Varga on 15/05/2020.
//

#ifndef CLOX_PARSER_H
#define CLOX_PARSER_H

#include "scanner.h"

class Parser {
public:
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;

    void errorAtCurrent(const char* message);
    void error(const char* message);
    void errorAt(Token* token, const char* message);
};

#endif //CLOX_PARSER_H
