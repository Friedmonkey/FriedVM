#pragma once
#include "BaseValue.h"

#define TYPECAST(type)                                                              \
const auto* type##Value = dynamic_cast<const NumberValue<type##_t>*>(otherValue);   \
if (type##Value) {                                                                  \
return static_cast<T>(type##Value->internal_value);                                 \
}                                                                                       

template <typename T>
class NumberValue : public BaseValue {
public:
    T internal_value;

    explicit NumberValue(T value) : internal_value(value) {}
    virtual ~NumberValue() = default;

    // Arithmetic operations accepting any NumberValue
    std::unique_ptr<NumberValue<T>> sub(const BaseValue& otherValue) const {
        T result = internal_value - CastToType(&otherValue);
        return std::make_unique<NumberValue<T>>(result);
    }

    std::unique_ptr<NumberValue<T>> add(const BaseValue& otherValue) const {
        T result = internal_value + CastToType(&otherValue);
        return std::make_unique<NumberValue<T>>(result);
    }

    std::unique_ptr<NumberValue<T>> mul(const BaseValue& otherValue) const {
        T result = internal_value * CastToType(&otherValue);
        return std::make_unique<NumberValue<T>>(result);
    }

    std::unique_ptr<NumberValue<T>> div(const BaseValue& otherValue) const {
        T divisor = CastToType(&otherValue);
        if (divisor == 0) {
            DIE << "Division by zero.";
        }
        T result = internal_value / divisor;
        return std::make_unique<NumberValue<T>>(result);
    }

    // BaseValue implementation
    bool IsNumberType() const override {
        return true; // Indicates this is a number type
    }

protected:

    // Generalized casting logic
    virtual T CastToType(const BaseValue* otherValue) const {
        // Check if the `otherValue` is a number
        if (!otherValue->IsNumberType()) {
            DIE << "Invalid cast: Not a number type.";
        }

        // Attempt to cast to NumberValue
        const auto* numberValue = dynamic_cast<const NumberValue<T>*>(otherValue);
        if (numberValue) {
            return numberValue->internal_value;
        }

        TYPECAST(uint8)
        TYPECAST(uint16)
        TYPECAST(uint32)
        TYPECAST(uint64)

        TYPECAST(int8)
        TYPECAST(int16)
        TYPECAST(int32)
        TYPECAST(int64)

        TYPECAST(float)
        TYPECAST(double)

        // For mismatched types, handle conversion from data (extend as needed)
        return ConvertFromData(otherValue);
    }

    // Default implementation for conversion, override for custom logic
    virtual T ConvertFromData(const BaseValue* otherValue) const {
        DIE << "Conversion logic not implemented.";
    }
};
