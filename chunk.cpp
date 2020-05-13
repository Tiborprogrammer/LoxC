#include "chunk.h"

void Chunk::writeChunk(uint8_t byte, int line) {
        lines.push_back(line);
        code.push_back(byte);
    }

void Chunk::freeChunk() {
    lines.clear();
    constants.freeValueArray();
    code.clear();
}

int Chunk::addConstant(Value value) {
    constants.writeValueArray(value);
    return constants.length() - 1;
}

int Chunk::length() const {
    return code.size();
}