#include <stdio.h>
#include <string.h>

#include "common.h"

class Scanner {
public:
    const char* start;
    const char* current;
    int line;

    void initScanner(const char* source) {
        start = source;
        current = source;
        line = 1;
    }
};