#pragma once
#include <cstdint>
#include <memory>
#include "../Logger.h"
#include "Types.h"
#include <functional>

struct BaseValue
{
    uint8_t* data;
    size_t length;
    ValueType type_index;

    union {
        uint8_t metadata;
        struct
        {
            bool isConst : 1;
            uint8_t rest : 7;
        };
    };

    BaseValue(ValueType t, uint8_t *newData, size_t len, uint8_t meta = 0) : type_index(t), length(len), metadata(meta) {
        data = newData;
    }

    // Constructor to initialize the BaseValue
    BaseValue(ValueType t, size_t len, uint8_t meta = 0) : type_index(t), length(len), metadata(meta) {
        data = new uint8_t[length](); // Allocate memory for the data
    }

    // Destructor to free the allocated memory
    ~BaseValue() {
        delete[] data;
    }

    // Function to get the size of the element based on the type
    static size_t getTypeSize(ValueType type);

    static void FillDefaultValue(BaseValue *value);

    static BaseValue* createValue(ValueType type) {
        BaseValue* val = new BaseValue(type, getTypeSize(type));
        //std::memcpy(val->data, &value, sizeof(T));  // Copy the raw data
        return val;
    }

    // Static factory methods for creating BaseValue instances
    template<typename T>
    static BaseValue* makeValue(ValueType type, T value) {
        BaseValue* val = new BaseValue(type, getTypeSize(type));
        std::memcpy(val->data, &value, sizeof(T));  // Copy the raw data
        return val;
    }

    template<typename T>
    static T getValue(const BaseValue* value) {
        // Verify the length matches the expected type size
        if (value->length != getTypeSize(value->type_index))
            DIE << "Type mismatch: stored data doesn't match requested type!";

        // Create a temporary variable to store the value
        T result;
        // Safely copy the data into the result variable
        std::memcpy(&result, value->data, sizeof(T));
        return result;
    }

    static BaseValue* dupValue(const BaseValue* value) {
        auto size = value->length;
        BaseValue* val = new BaseValue(value->type_index, size, value->metadata);
        std::memcpy(val->data, value->data, size);
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

    //// Helper methods to manipulate the union members (isArray & type_index)
    //void setIsArray(bool isArrayFlag) {
    //    isArray = isArrayFlag;
    //}

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
    static BaseValue* HandleTyped(const BaseValue& v1, const BaseValue& v2, std::function<T(T,T)> operation) {
        if (v1.type_index != v2.type_index) {
            DIE << "Type mismatch in HandleTyped";
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
        T result = operation(*data1, *data2);


        return makeValue<T>(v1.type_index, result);
        //// Create a new BaseValue to store the result
        //BaseValue resultVal(v1.type_index, 1);  // Only one element in the result
        //std::memcpy(resultVal.data, &result, sizeof(T));  // Copy the result back into the data

        //return resultVal;
    }

    template<typename T>
    static void HandleSingleTyped(BaseValue *result, const BaseValue& v1, std::function<void(T, BaseValue*)> operation) {
        if (sizeof(T) != getTypeSize(v1.type_index)) {
            DIE << "Size mismatch between T and BaseValue type";
        }
        T* data1 = reinterpret_cast<T*>(v1.data);

        operation(*data1, result);

        //return makeValue<T>(v1.type_index, result);
    }

    //template<typename T>
    //static BaseValue AddTyped(const BaseValue& v1, const BaseValue& v2)
    //{
    //    return HandleTyped<T>(v1, v2, [](T a, T b) {
    //        return a + b;
    //    });
    //}
    static struct ToStringTypedFunctor {
        template <typename T>
        void operator()(const BaseValue* v1, BaseValue* outsideResult) const {
            HandleSingleTyped<T>(outsideResult, *v1, [](T var, BaseValue* result) {
                std::ostringstream oss;
                oss << var;
                std::string str = oss.str();

                result->length= str.length();
                result->data = new uint8_t[result->length]; // or malloc if you want C-style
                std::memcpy(result->data, str.data(), result->length);
            });
        }
    };

    static BaseValue* ToString(const BaseValue* var1) 
    {
        BaseValue* result = new BaseValue(vt_string, 0);
        BaseValue::ExecuteSingleTypedTemplate(BaseValue::ToStringTypedFunctor(), var1, result);
        return result;
    }

    template <typename Func>
    static BaseValue* ExecuteSingleTypedTemplate(Func func, const BaseValue* v1, BaseValue *result) {
        if (!v1) {
            DIE << "Null pointer passed to ExecuteSingleTypedTemplate";
            //return nullptr;
        }

        // Dispatch based on type
        switch (v1->type_index) {
        case vt_uint8_t:   func.template operator()<uint8_t>(v1, result); break;
        case vt_uint16_t:  func.template operator()<uint16_t>(v1, result); break;
        case vt_uint32_t:  func.template operator()<uint32_t>(v1, result); break;
        case vt_uint64_t:  func.template operator()<uint64_t>(v1, result); break;
        case vt_int8_t:    func.template operator()<int8_t>(v1, result); break;
        case vt_int16_t:   func.template operator()<int16_t>(v1, result); break;
        case vt_int32_t:   func.template operator()<int32_t>(v1, result); break;
        case vt_int64_t:   func.template operator()<int64_t>(v1, result); break;
        case vt_float_t:   func.template operator()<float>(v1, result); break;
        case vt_double_t:  func.template operator()<double>(v1, result); break;
        default:
            DIE << "Unsupported type in ExecuteTyped";
        }

        return result;
    }

    //BaseValue ExecuteTyped(Func func, const BaseValue& v1, const BaseValue& v2);
    template <typename Func>
    static BaseValue* ExecuteTyped(Func func, const BaseValue* v1, const BaseValue* v2, BaseValue* result) {
        if (!v1 || !v2) {
            DIE << "Null pointer passed to ExecuteTyped";
            //return nullptr;
        }


        // Dispatch based on type
        switch (v1->type_index) {
        case vt_uint8_t:   result = func.template operator()<uint8_t>(v1, v2); break;
        case vt_uint16_t:  result = func.template operator()<uint16_t>(v1, v2); break;
        case vt_uint32_t:  result = func.template operator()<uint32_t>(v1, v2); break;
        case vt_uint64_t:  result = func.template operator()<uint64_t>(v1, v2); break;
        case vt_int8_t:    result = func.template operator()<int8_t>(v1, v2); break;
        case vt_int16_t:   result = func.template operator()<int16_t>(v1, v2); break;
        case vt_int32_t:   result = func.template operator()<int32_t>(v1, v2); break;
        case vt_int64_t:   result = func.template operator()<int64_t>(v1, v2); break;
        case vt_float_t:   result = func.template operator()<float>(v1, v2); break;
        case vt_double_t:  result = func.template operator()<double>(v1, v2); break;
        default:
            DIE << "Unsupported type in ExecuteTyped";
        }

        return result;
    }
    static struct AddTypedFunctor {
        template <typename T>
        BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
            return HandleTyped<T>(*v1, *v2, [](T a, T b) {
                return a + b;
                });
        }
    };
    static struct SubTypedFunctor {
        template <typename T>
        BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
            return HandleTyped<T>(*v1, *v2, [](T a, T b) {
                return a - b;
                });
        }
    };
    static struct MulTypedFunctor {
        template <typename T>
        BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
            return HandleTyped<T>(*v1, *v2, [](T a, T b) {
                return a * b;
                });
        }
    };
    static struct DivTypedFunctor {
        template <typename T>
        BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
            return HandleTyped<T>(*v1, *v2, [](T a, T b) {
                return a / b;
                });
        }
    };
    //static struct AndTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a & b;
    //            });
    //    }
    //};
    //static struct OrTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a | b;
    //        });
    //    }
    //};
    static struct RndTypedFunctor {
        mutable uint64_t seed = 0xDEADBABE12345678; // you can make it settable if u want :)

        template <typename T>
        BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
            return BaseValue::HandleTyped<T>(*v1, *v2, [this](T a, T b) -> T {
                if (a > b) std::swap(a, b);

                // === custom LCG RNG ===
                seed = seed * 6364136223846793005ULL + 1;
                uint64_t randVal = (seed >> 16) & 0xFFFFFFFF;

                if constexpr (std::is_integral_v<T>) {
                    T range = b - a + 1;
                    return a + (randVal % range);
                }
                else if constexpr (std::is_floating_point_v<T>) {
                    double norm = static_cast<double>(randVal) / static_cast<double>(0xFFFFFFFF);
                    return static_cast<T>(a + norm * (b - a));
                }
                else {
                    DIE << "Unsupported type in RndTypedFunctor";
                }
                });
        }
    };


};