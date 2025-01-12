#pragma once
#include "NumberValue.h"

class Int32Value : public NumberValue<int32_t> {
public:
    Int32Value(int32_t value) : NumberValue(value) {}

    ValueType GetValueType() const override {
        return vt_int32_t;
    }

protected:
    int32_t ConvertFromData(const BaseValue* otherValue) const override {
        // Example: Default conversion logic
        const auto* floatValue = dynamic_cast<const NumberValue<float_t>*>(otherValue);
        if (floatValue) {
            return static_cast<int32_t>(floatValue->internal_value); // Convert float to int
        }
        DIE << "Unsupported type for conversion.";
    }
};
