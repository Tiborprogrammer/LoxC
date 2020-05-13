#ifndef clox_common_h
#define clox_common_h

#include <cstddef>
#include <cstdint>
#include <cstdlib>

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

#define DEBUG_TRACE_EXECUTION

#endif               