#include "value.h"

void ValueArray::writeValueArray(Value value) {
    values.push_back(value);
}

void ValueArray::freeValueArray() {
    values.clear();
}

void ValueArray::printValue(Value value) {
    printf("%g", value);
}

int ValueArray::length() const {
  return values.size();
}