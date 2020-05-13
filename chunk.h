#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include <vector>
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
} OpCode;

class Chunk {
public:
    vector<uint8_t> code;
    vector<int> lines;
    ValueArray constants;

    void writeChunk(uint8_t byte, int line);

    void freeChunk();

    int addConstant(Value value);

    int length() const;
};

#endif     