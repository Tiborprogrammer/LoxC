#include "common.h"
#include <cstdio>
#include "debug.h"
#include "compiler.h"
#include <cmath>
#include <cstdarg>

using namespace std;

class VM {
public:
    Chunk * chunk;
    int ip;
    vector<Value> stack;
    Compiler compiler = Compiler();
    Scanner scanner;

    VM() {
        chunk = new Chunk();
        resetStack();
    }

    void freeVM() {}

    void resetStack() {
        stack.clear();
    }

    void runtimeError(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputs("\n", stderr);
        int line = chunk->lines.at(ip - 1);
        fprintf(stderr, "[line %d] in script\n", line);

        resetStack();
    }

    void push(Value value) {
        stack.push_back(value);
    }

    Value pop() {
        Value popValue = stack.back();
        stack.pop_back();
        return popValue;
    }

    Value peek(int distance) {
        return stack.at(stack.size() - 1 - distance);
    }

    static bool isFalsey(Value value) {
        return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
    }

    InterpretResult run() {
        #define READ_BYTE() (chunk->code.at(ip++))
        #define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
        #define BINARY_OP(valueType, op) \
            do { \
              if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
                runtimeError("Operands must be numbers."); \
                return INTERPRET_RUNTIME_ERROR; \
              } \
              \
              double b = AS_NUMBER(pop()); \
              double a = AS_NUMBER(pop()); \
              push(valueType(a op b)); \
            } while (false)

        for (;;) {
            #ifdef DEBUG_TRACE_EXECUTION
                        printf("          ");
                        for (Value value : stack) {
                            printf("[ ");
                            ValueArray::printValue(value);
                            printf(" ]");
                        }
                        printf("\n");
                        Debug::disassembleInstruction(chunk, ip);
            #endif

            uint8_t instruction;
            switch (instruction = READ_BYTE()) {
                case OP_CONSTANT: {
                    Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }
                case OP_NIL: push(NIL_VAL); break;
                case OP_TRUE: push(BOOL_VAL(true)); break;
                case OP_FALSE: push(BOOL_VAL(false)); break;
                case OP_ADD:      BINARY_OP(NUMBER_VAL, +); break;
                case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
                case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
                case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
                case OP_MODULUS: {
                    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {
                        runtimeError("Operands must be numbers.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(fmod(a, b)));
                    break;
                }
                case OP_NOT:
                    push(BOOL_VAL(isFalsey(pop())));
                    break;
                case OP_POWER: {
                    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {
                        runtimeError("Operands must be numbers.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    double b = AS_NUMBER(pop());
                    double a = AS_NUMBER(pop());
                    push(NUMBER_VAL(pow(a,b)));
                    break;
                }
                case OP_NEGATE:
                    if (!IS_NUMBER(peek(0))) {
                        runtimeError("Operand must be a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }

                    push(NUMBER_VAL(-AS_NUMBER(pop())));
                    break;
                case OP_RETURN: {
                    ValueArray::printValue(pop());
                    printf("\n");
                    return INTERPRET_OK;
                }
            }
        }

        #undef READ_BYTE
        #undef READ_CONSTANT
        #undef BINARY_OP
    }

    InterpretResult interpret(const char* source) {
        if (!compile(source, chunk)) {
            chunk->freeChunk();
            return INTERPRET_COMPILE_ERROR;
        }

        ip = 0;

        InterpretResult result = run();

        chunk->freeChunk();
        return result;
    }
};