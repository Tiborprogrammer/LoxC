#ifndef clox_value_h
#define clox_value_h

#include <cstdio>
#include <vector>

using namespace std;

typedef double Value;

class ValueArray {
public:
    vector<Value> values;

    void writeValueArray(Value value);

    void freeValueArray();

    static void printValue(Value value);

    int length() const;
};

#endif               