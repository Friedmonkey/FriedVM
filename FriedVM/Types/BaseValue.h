#pragma once
#include <cstdint>
#include <memory>
#include "../Logger.h"
#include "Types.h"

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
        data = new uint8_t[length]; // Allocate memory for the data
        isArray = false;  // Default is not an array
    }

    //// Constructor to initialize the BaseValue
    //BaseValue(ValueType t, size_t len) : type{ 0 }, length(len) {
    //    type_index = t;  // Initialize the type
    //    data = new uint8_t[length * getTypeSize(t)]; // Allocate memory for the data
    //    isArray = false;  // Default is not an array
    //}

    // Destructor to free the allocated memory
    ~BaseValue() {
        delete[] data;
    }

    // Function to get the size of the element based on the type
    constexpr static size_t getTypeSize(ValueType type);

    // Static factory methods for creating BaseValue instances
    template<typename T>
    static BaseValue makeValue(ValueType type, T value) {

        BaseValue val(type, getTypeSize(type));
        std::memcpy(val.data, &value, sizeof(T));  // Copy the raw data
        return val;
    }
    //// Static factory methods for creating BaseValue instances
    //static BaseValue makeRaw(uint8_t* data, size_t len) {
    //    BaseValue val(vt_raw, len);
    //    std::memcpy(val.data, data, len);  // Copy the raw data
    //    return val;
    //}

    //static BaseValue makeNumber(ValueType t, void* value) {
    //    BaseValue val(t, 1); // Only one element (a single number)
    //    std::memcpy(val.data, value, getTypeSize(t));  // Copy the value
    //    return val;
    //}

    //static BaseValue makeArray(ValueType t, void* arr, size_t len) {
    //    BaseValue val(t, len);
    //    std::memcpy(val.data, arr, len * getTypeSize(t));  // Copy the array data
    //    val.isArray = true;  // Set the isArray flag
    //    return val;
    //}

    // Helper methods to manipulate the union members (isArray & type_index)
    void setIsArray(bool isArrayFlag) {
        isArray = isArrayFlag;
    }

    void setTypeIndex(ValueType type) {
        type_index = type;
    }

    bool isNumber() const;
    std::string toString() const;

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
        if (v1.type_index != v2.type_index) {
            DIE << "Type mismatch in AddTyped";
        }

        // Ensure the size of T matches the type size of v1
        if (sizeof(T) != getTypeSize(v1.type_index)) {
            DIE << "Size mismatch between T and BaseValue type";
        }

        // Now, we assume the data in v1 and v2 is of the correct length and type
        // Cast the data from uint8_t* to T* (the target type, e.g., uint32_t*)
        T* data1 = reinterpret_cast<T*>(v1.data);
        T* data2 = reinterpret_cast<T*>(v2.data);

        // Perform the addition
        T result = *data1 + *data2;

        return makeValue<T>(v1.type_index, result);
        //// Create a new BaseValue to store the result
        //BaseValue resultVal(v1.type_index, 1);  // Only one element in the result
        //std::memcpy(resultVal.data, &result, sizeof(T));  // Copy the result back into the data

        //return resultVal;
    }


    BaseValue Add(const BaseValue& v1, const BaseValue& v2);
};