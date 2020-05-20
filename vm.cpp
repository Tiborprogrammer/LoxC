#include "common.h"
#include <cstdio>
#include "debug.h"
#include "compiler.h"

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

    void push(Value value) {
        stack.push_back(value);
    }

    Value pop() {
        Value popValue = stack.back();
        stack.pop_back();
        return popValue;
    }

    InterpretResult run() {
        #define READ_BYTE() (chunk->code.at(ip++))
        #define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
        #define BINARY_OP(op) \
            do { \
              double b = pop(); \
              double a = pop(); \
              push(a op b); \
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
                case OP_ADD:      BINARY_OP(+); break;
                case OP_SUBTRACT: BINARY_OP(-); break;
                case OP_MULTIPLY: BINARY_OP(*); break;
                case OP_DIVIDE:   BINARY_OP(/); break;
                case OP_NEGATE:   push(-pop()); break;
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