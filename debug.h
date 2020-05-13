#ifndef clox_debug_h
#define clox_debug_h

#include <string>
#include "chunk.h"

class Debug {
public:
    static void disassembleChunk(Chunk * chunk, const string& name);

    static int constantInstruction(const char* name, Chunk* chunk, int index);

    static int simpleInstruction(const char* name, int index);

    static int disassembleInstruction(Chunk* chunk, int index);
};

#endif          