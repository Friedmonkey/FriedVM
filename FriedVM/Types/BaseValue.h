#pragma once
#include <cstdint>
#include <memory>
#include "../Logger.h"
//#include "Types.h"

enum ValueType
{
    vt_raw = 0,
    vt_uint8_t = 1,
    vt_uint16_t = 2,
    vt_uint32_t = 3,
    vt_uint64_t = 4,
    vt_int8_t = 5,
    vt_int16_t = 6,
    vt_int32_t = 7,
    vt_int64_t = 8,
    vt_float_t = 9,
    vt_double_t = 10,
    vt_pointer = 11,
};

struct BaseValue
{
    uint8_t* data;
    size_t length;

    // Union to store type and array information in 1 byte using bitfields
    union {
        uint8_t type;  // Type of the data (e.g., UINT32, FLOAT)
        struct {
            bool isArray : 1;  // Topmost bit for isArray (1 bit)
            ValueType type_index : 7; // Remaining 7 bits for type_index
        };
    };

    // Constructor to initialize the BaseValue
    BaseValue(ValueType t, size_t len) : type{ 0 }, length(len) {
        type_index = t;  // Initialize the type
        data = new uint8_t[length * getTypeSize(t)]; // Allocate memory for the data
        isArray = false;  // Default is not an array
    }

    // Destructor to free the allocated memory
    ~BaseValue() {
        delete[] data;
    }

    // Function to get the size of the element based on the type
    constexpr static size_t getTypeSize(ValueType type) {
        switch (type) {
        case vt_raw: return 4;
        case vt_uint8_t: return sizeof(uint8_t);
        case vt_uint16_t: return sizeof(uint16_t);
        case vt_uint32_t: return sizeof(uint32_t);
        case vt_uint64_t: return sizeof(uint64_t);
        case vt_int8_t: return sizeof(int8_t);
        case vt_int16_t: return sizeof(int16_t);
        case vt_int32_t: return sizeof(int32_t);
        case vt_int64_t: return sizeof(int64_t);
        case vt_float_t: return sizeof(float);
        case vt_double_t: return sizeof(double);
        case vt_pointer: return 4;
        default: return 0;
        }
    }

    // Static factory methods for creating BaseValue instances
    static BaseValue makeRaw(uint8_t* data, size_t len) {
        BaseValue val(vt_raw, len);
        std::memcpy(val.data, data, len);  // Copy the raw data
        return val;
    }

    static BaseValue makeNumber(ValueType t, void* value) {
        BaseValue val(t, 1); // Only one element (a single number)
        std::memcpy(val.data, value, getTypeSize(t));  // Copy the value
        return val;
    }

    static BaseValue makeArray(ValueType t, void* arr, size_t len) {
        BaseValue val(t, len);
        std::memcpy(val.data, arr, len * getTypeSize(t));  // Copy the array data
        val.isArray = true;  // Set the isArray flag
        return val;
    }

    // Helper methods to manipulate the union members (isArray & type_index)
    void setIsArray(bool isArrayFlag) {
        isArray = isArrayFlag;
    }

    void setTypeIndex(ValueType type) {
        type_index = type;
    }

    bool isNumber() const {
        // Return true if the type is one of the numeric types
        return type_index == vt_uint8_t || type_index == vt_uint16_t ||
            type_index == vt_uint32_t || type_index == vt_uint64_t ||
            type_index == vt_int8_t || type_index == vt_int16_t ||
            type_index == vt_int32_t || type_index == vt_int64_t ||
            type_index == vt_float_t || type_index == vt_double_t;
    }

    //// Cast function to convert BaseValue to a different type
    //BaseValue Cast(ValueType targetType) const {
    //    // Create a new BaseValue to store the casted result
    //    BaseValue casted(targetType, length);

    //    size_t targetTypeSize = getTypeSize(targetType);
    //    size_t currentTypeSize = getTypeSize(type_index);

    //    if (targetType == type_index) {
    //        std::memcpy(casted.data, data, length * currentTypeSize);  // Direct copy if types match
    //        return casted;
    //    }

    //    if (type_index == vt_raw || targetType == vt_raw) {
    //        // Handle raw data cases, typically byte-to-byte copy
    //        std::memcpy(casted.data, data, length * currentTypeSize);
    //        return casted;
    //    }

    //    // Handle casting between numeric types
    //    if (isNumber() && targetType != vt_raw) {
    //        size_t newLength = length * targetTypeSize / currentTypeSize;

    //        if (newLength != length) {
    //            // If new length doesn't match, resize or truncate
    //            std::memcpy(casted.data, data, newLength * targetTypeSize);
    //        }
    //        else {
    //            std::memcpy(casted.data, data, length * targetTypeSize);
    //        }
    //        return casted;
    //    }

    //    // Handling for other types (such as float-to-double or integer-to-float)
    //    if ((type_index == vt_float_t && targetType == vt_double_t) ||
    //        (type_index == vt_double_t && targetType == vt_float_t)) {
    //        // Example: Adjusting precision for float to double (or vice versa)
    //        std::memcpy(casted.data, data, length * targetTypeSize);
    //        return casted;
    //    }

    //    // If not handled, throw an exception
    //    throw std::runtime_error("Unsupported casting between types");
    //}
    // Template function for adding two BaseValue instances
    template<typename T>
    BaseValue AddTyped(const BaseValue& v1, const BaseValue& v2) {
        if (v1.length != v2.length) {
            throw std::runtime_error("Length mismatch in Add");
        }

        // Create result BaseValue (same type and length as v1)
        BaseValue result(v1.type_index, v1.length);

        for (size_t i = 0; i < v1.length; i++) {
            T val1 = reinterpret_cast<T*>(v1.data)[i];
            T val2 = reinterpret_cast<T*>(v2.data)[i];
            reinterpret_cast<T*>(result.data)[i] = val1 + val2; // Perform addition
        }

        return result;
    }

    // The main Add method to dispatch based on type
    BaseValue Add(const BaseValue& v1, const BaseValue& v2) {
        // Validate if both values are numbers
        if (!v1.isNumber() || !v2.isNumber()) {
            throw std::runtime_error("Cannot add non-number values");
        }
        //if (v1.type_index != v2.type_index || v1.length != v2.length) {
        //    // Cast v2 to the same type as v1
        //    BaseValue casted = v2.Cast(v1.type_index);
        //    return Add(v1, casted);  // Now they are both of the same type
        //}

        BaseValue result(v1.type_index, v1.length);

        // Dispatch based on type
        switch (v1.type_index) {
        case vt_uint8_t:   result = AddTyped<uint8_t>(v1, v2); break;
        case vt_uint16_t:  result = AddTyped<uint16_t>(v1, v2); break;
        case vt_uint32_t:  result = AddTyped<uint32_t>(v1, v2); break;
        case vt_uint64_t:  result = AddTyped<uint64_t>(v1, v2); break;
        case vt_int8_t:    result = AddTyped<int8_t>(v1, v2); break;
        case vt_int16_t:   result = AddTyped<int16_t>(v1, v2); break;
        case vt_int32_t:   result = AddTyped<int32_t>(v1, v2); break;
        case vt_int64_t:   result = AddTyped<int64_t>(v1, v2); break;
        case vt_float_t:   result = AddTyped<float>(v1, v2); break;
        case vt_double_t:  result = AddTyped<double>(v1, v2); break;
        default:
            throw std::runtime_error("Unsupported type in Add");
        }

        return result;
    }

};