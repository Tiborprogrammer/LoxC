#include "compiler.h"
void Compiler::compile(const char *source) {
    scanner.initScanner(source);

    int line = -1;
    for (;;) {
        Token token = scanner.scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF) break;
    }
}
