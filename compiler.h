#ifndef clox_compiler_h
#define clox_compiler_h

#include "common.h"
#include "scanner.h"
#include "chunk.h"
#include "parser.h"

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_POWER,       // **
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence;


struct Compiler {
    Scanner scanner;
    Parser parser;
    Chunk* compilingChunk;
};

extern struct Compiler compiler;

bool compile(const char* source, Chunk * chunk);
void advance();
void consume(TokenType type, const char* message);
void emitByte(uint8_t byte);
Chunk* currentChunk();
void endCompiler();
void emitReturn();
void emitBytes(uint8_t byte1, uint8_t byte2);
void expression();
void number();
void emitConstant(Value value);
void grouping();
void unary();
void parsePrecedence(Precedence precedence);
uint8_t makeConstant(Value value);
void binary();
void errorAtCurrent(const char* message);
void error(const char* message);
void errorAt(Token* token, const char* message);
void literal();

typedef void(*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

ParseRule* getRule(TokenType type);

extern ParseRule rules[42];


#endif   