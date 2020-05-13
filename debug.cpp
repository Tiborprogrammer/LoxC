#include "debug.h"
#include <cstdio>

void Debug::disassembleChunk(Chunk * chunk, const string& name) {
    printf("== %s ==\n", name.c_str());

    for (int index = 0; index < chunk->length();) {
        index = disassembleInstruction(chunk, index);
    }
}

int Debug::constantInstruction(const char * name, Chunk* chunk,
                               int index) {
    uint8_t constant = chunk->code[index + 1];
    printf("%-16s %4d '", name, constant);
    ValueArray::printValue(chunk->constants.values[constant]);
    printf("'\n");
    return index + 2;
}

int Debug::simpleInstruction(const char* name, int index) {
    printf("%s\n", name);
    return index + 1;
}

int Debug::disassembleInstruction(Chunk* chunk, int index) {
    printf("%04d ", index);
    if (index > 0 && chunk->lines[index] == chunk->lines[index - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->lines[index]);
    }

    uint8_t instruction = chunk->code[index];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, index);
        case OP_ADD:
            return simpleInstruction("OP_ADD", index);
        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", index);
        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", index);
        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", index);
        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", index);
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", index);
        default:
            printf("Unknown opcode %d\n", instruction);
            return index + 1;
    }
}