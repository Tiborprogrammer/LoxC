#include "scanner.h"
#include <iostream>

using namespace std;

void Scanner::initScanner(const char* source) {
    start = source;
    current = source;
    line = 1;
}

bool Scanner::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool Scanner::isDigit(char c) {
    return c >= '0' && c <= '9';
}

Token Scanner::scanToken() {
    if (peek() == '/' && peekNext() == '*') {
        if (!multiLineComment()) {
            return errorToken("Are you that lazy that you can't even terminate a multi line comment");
        }
    }

    skipWhitespace();

    start = current;

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TokenType::TOKEN_LEFT_PAREN);
        case ')': return makeToken(TokenType::TOKEN_RIGHT_PAREN);
        case '{': return makeToken(TokenType::TOKEN_LEFT_BRACE);
        case '}': return makeToken(TokenType::TOKEN_RIGHT_BRACE);
        case ';': return makeToken(TokenType::TOKEN_SEMICOLON);
        case ',': return makeToken(TokenType::TOKEN_COMMA);
        case '.': return makeToken(TokenType::TOKEN_DOT);
        case '-': return makeToken(TokenType::TOKEN_MINUS);
        case '+': return makeToken(TokenType::TOKEN_PLUS);
        case '/':
            return makeToken(TokenType::TOKEN_SLASH);
        case '*': return makeToken(TokenType::TOKEN_STAR);
        case '!':
            return makeToken(match('=') ? TokenType::TOKEN_BANG_EQUAL : TokenType::TOKEN_BANG);
        case '=':
            return makeToken(match('=') ? TokenType::TOKEN_EQUAL_EQUAL : TokenType::TOKEN_EQUAL);
        case '<':
            return makeToken(match('=') ? TokenType::TOKEN_LESS_EQUAL : TokenType::TOKEN_LESS);
        case '>':
            return makeToken(match('=') ? TokenType::TOKEN_GREATER_EQUAL : TokenType::TOKEN_GREATER);
        case '"': return string();
    }

    return errorToken("Unexpected character.");
}

bool Scanner::isAtEnd() const {
    return *current == '\0';
}

char Scanner::advance() {
    current++;
    return current[-1];
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (*current != expected) return false;

    current++;
    return true;
}

Token Scanner::makeToken(TokenType type) const {
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;

    return token;
}

Token Scanner::errorToken(const char* message) const {
    Token token;
    token.type = TokenType::TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = line;

    return token;
}

void Scanner::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;

            case '\n':
                line++;
                advance();
                break;

            case '/':
                if (peekNext() == '/') {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    return;
                }
                break;

            default:
                return;
        }
    }
}

bool Scanner::multiLineComment() {
    char c = '*';
    // While we still have at least 2 more chars in front of us
    while ('\0' != *(current + 1) && !isAtEnd()) {
        // If the next char is a new line, increment the line counter
        if (c == '\n') line++;

        // If we found the end of the comment
        if (peek() == '*' && peekNext() == '/') {
            // Advance two times for '*' and '/'
            advance();
            advance();
            // Exit the function
            return true;
        }
        // Advance to the next char
        c = advance();
    }
    advance();
    return false;
}

TokenType Scanner::checkKeyword(int startPos, int length, const char* rest, TokenType type) const {
    if ((long)(current - start) == (startPos + length) && memcmp(start + startPos, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    return makeToken(identifierType());
}

TokenType Scanner::identifierType() const {
    switch (start[0]) {
        case 'a': return checkKeyword(1, 2, "nd", TokenType::TOKEN_AND);
        case 'c': return checkKeyword(1, 4, "lass", TokenType::TOKEN_CLASS);
        case 'e': return checkKeyword(1, 3, "lse", TokenType::TOKEN_ELSE);
        case 'f':
            if (current - start > 1) {
                switch (start[1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TokenType::TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TokenType::TOKEN_FOR);
                    case 'u': return checkKeyword(2, 1, "n", TokenType::TOKEN_FUN);
                }
            }
            break;
        case 't':
            if (current - start > 1) {
                switch (start[1]) {
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TokenType::TOKEN_IF);
        case 'n': return checkKeyword(1, 2, "il", TokenType::TOKEN_NIL);
        case 'o': return checkKeyword(1, 1, "r", TokenType::TOKEN_OR);
        case 'p': return checkKeyword(1, 4, "rint", TokenType::TOKEN_PRINT);
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::TOKEN_RETURN);
        case 's': return checkKeyword(1, 4, "uper", TokenType::TOKEN_SUPER);
        case 'v': return checkKeyword(1, 2, "ar", TokenType::TOKEN_VAR);
        case 'w': return checkKeyword(1, 4, "hile", TokenType::TOKEN_WHILE);
    }
    return TokenType::TOKEN_IDENTIFIER;
}

Token Scanner::number() {
    while (isDigit(peek())) advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
        // Consume the ".".
        advance();

        while (isDigit(peek())) advance();
    }

    return makeToken(TokenType::TOKEN_NUMBER);
}

Token Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    // The closing quote.
    advance();
    return makeToken(TokenType::TOKEN_STRING);
}

char Scanner::peek() const {
    return *current;
}

char Scanner::peekNext() const {
    if (isAtEnd()) return '\0';
    return current[1];
}