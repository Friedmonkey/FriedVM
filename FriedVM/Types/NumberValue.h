#pragma once
#include "BaseValue.h"

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

        // For mismatched types, handle conversion from data (extend as needed)
        return ConvertFromData(otherValue);
    }

    // Default implementation for conversion, override for custom logic
    virtual T ConvertFromData(const BaseValue* otherValue) const {
        DIE << "Conversion logic not implemented.";
    }
};
