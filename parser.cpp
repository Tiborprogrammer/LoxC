//
// Created by Tibor Varga on 15/05/2020.
//

#include "parser.h"

void Parser::errorAt(Token * token, const char* message) {
    if (panicMode) return;
    panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    hadError = true;
}

void Parser::errorAtCurrent(const char* message) {
    errorAt(&current, message);
}

void Parser::error(const char* message) {
    errorAt(&previous, message);
}