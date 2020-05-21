#include "value.h"

void ValueArray::writeValueArray(Value value) {
    values.push_back(value);
}

void ValueArray::freeValueArray() {
    values.clear();
}

void ValueArray::printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
    }
}

int ValueArray::length() const {
  return values.size();
}