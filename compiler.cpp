#include "compiler.h"
#include <cstdlib>
#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

Compiler compiler;
ParseRule rules[] = {
        {grouping, nullptr, PREC_NONE},       // TOKEN_LEFT_PAREN
        {nullptr,nullptr, PREC_NONE},       // TOKEN_RIGHT_PAREN
        {nullptr,nullptr, PREC_NONE},      // TOKEN_LEFT_BRACE
        {nullptr,nullptr, PREC_NONE},       // TOKEN_RIGHT_BRACE
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_COMMA
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_DOT
        {unary,    binary,  PREC_TERM},       // TOKEN_MINUS
        {nullptr,  binary,  PREC_TERM},       // TOKEN_PLUS
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_SEMICOLON
        {nullptr,  binary,  PREC_FACTOR},     // TOKEN_SLASH
        {nullptr,  binary,  PREC_FACTOR},     // TOKEN_STAR
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_BANG
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_BANG_EQUAL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_EQUAL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_EQUAL_EQUAL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_GREATER
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_GREATER_EQUAL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_LESS
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_LESS_EQUAL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_IDENTIFIER
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_STRING
        {number,   nullptr, PREC_NONE},       // TOKEN_NUMBER
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_AND
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_CLASS
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_ELSE
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_FALSE
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_FOR
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_FUN
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_IF
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_NIL
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_OR
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_PRINT
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_RETURN
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_SUPER
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_THIS
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_TRUE
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_VAR
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_WHILE
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_ERROR
        {nullptr,  nullptr, PREC_NONE},       // TOKEN_EOF
};

void errorAt(Token* token, const char* message) {
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    compiler.parser.hadError = true;
}

void errorAtCurrent(const char* message) {
    errorAt(&compiler.parser.current, message);
}

void error(const char* message) {
    errorAt(&compiler.parser.previous, message);
}

bool compile(const char* source, Chunk* chunk) {
    compiler.scanner.initScanner(source);
    compiler.compilingChunk = chunk;

    compiler.parser.hadError = false;
    compiler.parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !compiler.parser.hadError;
}

void advance() {
    compiler.parser.previous = compiler.parser.current;

    for (;;) {
        compiler.parser.current = compiler.scanner.scanToken();
        if (compiler.parser.current.type != TOKEN_ERROR) break;

        compiler.parser.errorAtCurrent(compiler.parser.current.start);
    }
}

void consume(TokenType type, const char* message) {
    if (compiler.parser.current.type == type) {
        advance();
        return;
    }

   compiler.parser.errorAtCurrent(message);
}

void emitByte(uint8_t byte) {
    currentChunk()->writeChunk(byte, compiler.parser.previous.line);
}

Chunk* currentChunk() {
    return compiler.compilingChunk;
}

void endCompiler() {
    emitReturn();

#ifdef DEBUG_PRINT_CODE
    if (!compiler.parser.hadError) {
        Debug::disassembleChunk(currentChunk(), "code");
    }
#endif
}

void binary() {
    // Remember the operator.
    TokenType operatorType = compiler.parser.previous.type;

    // Compile the right operand.
    ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    // Emit the operator instruction.
    switch (operatorType) {
        case TOKEN_PLUS:          emitByte(OP_ADD); break;
        case TOKEN_MINUS:         emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:          emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH:         emitByte(OP_DIVIDE); break;
        default:
            return; // Unreachable.
    }
}

void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void number() {
    double value = strtod(compiler.parser.previous.start, nullptr);
    emitConstant(value);
}

void unary() {
    TokenType operatorType = compiler.parser.previous.type;

    // Compile the operand.
    expression();

    // Emit the operator instruction.
    switch (operatorType) {
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default:
            return; // Unreachable.
    }
}

ParseRule* getRule(TokenType type) {
    int a = 0;
    return &rules[type];
}

void parsePrecedence(Precedence precedence) {
    advance();
    ParseFn prefixRule = getRule(compiler.parser.previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    prefixRule();


    while (precedence <= getRule(compiler.parser.current.type)->precedence) {
        advance();
        ParseFn infixRule = getRule(compiler.parser.previous.type)->infix;
        infixRule();
    }
}

void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

void emitReturn() {
    emitByte(OP_RETURN);
}

void emitConstant(Value value) {
    emitBytes(OP_CONSTANT, makeConstant(value));
}

uint8_t makeConstant(Value value) {
    int constant = currentChunk()->addConstant(value);
    return (uint8_t)constant;
}

void emitBytes(uint8_t byte1, uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}
