#pragma once
#include "NumberValue.h"

#define NUMBERTYPE(type)                                \
class type##Value : public NumberValue<type##_t> {      \
public:                                                 \
    type##Value(type##_t value) : NumberValue(value) {} \
                                                        \
    ValueType GetValueType() const override {           \
        return vt_##type##_t;                           \
    }                                                   \
};                                                             


NUMBERTYPE(uint8)
NUMBERTYPE(uint16)
NUMBERTYPE(uint32)
NUMBERTYPE(uint64)

NUMBERTYPE(int8)
NUMBERTYPE(int16)
NUMBERTYPE(int32)
NUMBERTYPE(int64)

NUMBERTYPE(float)
NUMBERTYPE(double)