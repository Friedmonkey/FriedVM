#pragma once
#include "BaseValue.h"                                                                            

class RawValueType : public BaseValue {
public:
    uint8_t *internal_value;
    size_t length;
};
