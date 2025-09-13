#pragma once
#include <cstdint>
#include <memory>
#include "../Logger.h"
#include "Types.h"
#include <functional>
#include <random>

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

    template<typename T>
    static BaseValue* makeValue(ValueType type, T value) {
        BaseValue* val = new BaseValue(type, getTypeSize(type));
        std::memcpy(val->data, &value, sizeof(T)); // Copy the raw data
        return val;
    }

    static void print(const BaseValue *val)
    {
        if (val == nullptr)
            return;
        for (size_t i = 0; i < val->length; i++) {
            printf("0x%02X ", val->data[i]);
        }
        printf("\n");
    }


    template<typename T>
    static T getValue(const BaseValue* value) {
        // Verify the length matches the expected type size
        if (value->length != getTypeSize(value->type_index))
            DIE << "Type mismatch: stored data doesn't match requested type!";

        switch (value->type_index) {
        case vt_bool: {
            bool v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_uint8_t: {
            uint8_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_uint16_t: {
            uint16_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_uint32_t: {
            uint32_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_label:
        case vt_uint64_t: {
            uint64_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_int8_t: {
            int8_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_int16_t: {
            int16_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_int32_t: {
            int32_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_int64_t: {
            int64_t v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_float_t: {
            float v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        case vt_double_t: {
            double v{};
            std::memcpy(&v, value->data, sizeof(v));
            return static_cast<T>(v);
        }
        default:
            DIE << "Unsupported type in getValue";
        }
        //if (value->length != sizeof(T))
        //{
        //    DIE << "Templated type does not match the underlying datatype";
        //    //instead of this error it should somehow make it work
        //    //if this is the data we request, itll just pad it out to make it work?
        //    //T is the type we want
        //    //value->data is the data that needs to be made to work with T
        //    //value->type_index is an enum representing the datatype
        //}

        //// Create a temporary variable to store the value
        //T result;
        //// Safely copy the data into the result variable
        //std::memcpy(&result, value->data, sizeof(T));
        //return result;
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


    template<typename T1, typename T2, typename Operation>
    static BaseValue* compute(const ValueType return_type, const BaseValue* var1, const BaseValue* var2, Operation operation)
    {
        T1 val1 = getValue<T1>(var1);
        T2 val2 = getValue<T2>(var2);
        return operation(return_type, val1, val2);
    }

#define CASE_OP(RET, VT2, T1, T2, OP) case VT2: return compute<T1, T2>(RET, v1, v2, OP);

#define SWITCH_SECOND(RET, T1, OP)                        \
    switch (v2->type_index) {                        \
        CASE_OP(RET, vt_uint8_t,  T1, uint8_t,  OP);      \
        CASE_OP(RET, vt_uint16_t, T1, uint16_t, OP);      \
        CASE_OP(RET, vt_uint32_t, T1, uint32_t, OP);      \
        CASE_OP(RET, vt_uint64_t, T1, uint64_t, OP);      \
        CASE_OP(RET, vt_int8_t,   T1, int8_t,   OP);      \
        CASE_OP(RET, vt_int16_t,  T1, int16_t,  OP);      \
        CASE_OP(RET, vt_int32_t,  T1, int32_t,  OP);      \
        CASE_OP(RET, vt_int64_t,  T1, int64_t,  OP);      \
        CASE_OP(RET, vt_float_t,  T1, float,    OP);      \
        CASE_OP(RET, vt_double_t, T1, double,   OP);      \
        default: DIE << "Unsupported rhs type";      \
    }

    template<typename Operation>
    static BaseValue* computeNumbers(const BaseValue* v1, const BaseValue* v2, Operation op)
    {
        switch (v1->type_index) {
        case vt_uint8_t:  SWITCH_SECOND(vt_uint8_t, uint8_t, op); break;
        case vt_uint16_t: SWITCH_SECOND(vt_uint16_t, uint16_t, op); break;
        case vt_uint32_t: SWITCH_SECOND(vt_uint32_t, uint32_t, op); break;
        case vt_uint64_t: SWITCH_SECOND(vt_uint64_t, uint64_t, op); break;
        case vt_int8_t:   SWITCH_SECOND(vt_int8_t, int8_t, op); break;
        case vt_int16_t:  SWITCH_SECOND(vt_int16_t, int16_t, op); break;
        case vt_int32_t:  SWITCH_SECOND(vt_int32_t, int32_t, op); break;
        case vt_int64_t:  SWITCH_SECOND(vt_int64_t, int64_t, op); break;
        case vt_float_t:  SWITCH_SECOND(vt_float_t, float, op); break;
        case vt_double_t: SWITCH_SECOND(vt_double_t, double, op); break;
        default: DIE << "Unsupported lhs type";
        }
    }

#pragma region BasicOparations
    struct EQCompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a == b);
        }
    };
    struct NEQCompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a != b);
        }
    };
    struct GTCompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a > b);
        }
    };
    struct GTECompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a >= b);
        }
    };
    struct LTCompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a < b);
        }
    };
    struct LTECompOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(vt_bool, a <= b);
        }
    };



    struct AddOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(ret_type, a + b);
        }
    };
    struct SubOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(ret_type, a - b);
        }
    };
    struct MulOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(ret_type, a * b);
        }
    };
    struct DivOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            return makeValue(ret_type, a / b);
        }
    };

#pragma endregion

    struct RNDOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
            using R = decltype(a + b);

            static std::random_device rd; // non-deterministic seed
            static std::mt19937 gen(rd()); // mersenne twister engine

            R result;

            if constexpr (std::is_integral<T1>::value && std::is_integral<T2>::value) {
                std::uniform_int_distribution<R> dist(a, b); // inclusive
                result = dist(gen);
            }
            else {
                double da = static_cast<double>(a);
                double db = static_cast<double>(b);
                std::uniform_real_distribution<double> dist(da, db);
                result = static_cast<R>(dist(gen));
            }

            return makeValue(ret_type, result);
        }
    };

    struct StringParseOperation {
        template <typename T1, typename T2>
        BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {


            return makeValue(ret_type, a + b);
        }
    };

    //struct RNDOperation {
    //    template <typename T1, typename T2>
    //    BaseValue* operator()(ValueType ret_type, T1 a, T2 b) const {
    //        using std::is_integral;
    //        using R = decltype(a + b); // promote types if needed

    //        R result;

    //        if constexpr (is_integral<T1>::value && is_integral<T2>::value) {
    //            // integer version
    //            result = static_cast<R>(rand() % (b - a + 1) + a); // inclusive
    //        }
    //        else {
    //            // floating point version
    //            double da = static_cast<double>(a);
    //            double db = static_cast<double>(b);
    //            result = static_cast<R>(da + (db - da) * (rand() / (RAND_MAX + 1.0)));
    //        }

    //        return makeValue(ret_type, result);
    //    }
    //};

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
    //template<typename T>
    //static BaseValue* HandleTyped(const BaseValue& v1, const BaseValue& v2, std::function<T(T,T)> operation) {
    //    // Ensure the size of T matches the type size of v1
    //    if (sizeof(T) != getTypeSize(v1.type_index)) {
    //        DIE << "Size mismatch between T and BaseValue type";
    //    }

    //    // Now, we assume the data in v1 and v2 is of the correct length and type
    //    // Cast the data from uint8_t* to T* (the target type, e.g., uint32_t*)
    //    T* data1 = reinterpret_cast<T*>(v1.data);
    //    T* data2 = nullptr;
    //    T temp;  // keeps the converted value alive

    //    if (v1.type_index == v2.type_index)
    //    {
    //        data2 = reinterpret_cast<T*>(v2.data);
    //    }
    //    else
    //    {
    //        temp = BaseValue::getValue<T>(&v2); // copy into a safe owned place
    //        data2 = &temp;
    //    }



    //    // Perform the addition
    //    T result = operation(*data1, *data2);

    //    return makeValue<T>(v1.type_index, result);
    //    //return new BaseValue<T>(v1.type_index, result);
    //    //BaseValue *blah = makeValue<T>(v1.type_index, result);

    //    //return blah;
    //    //// Create a new BaseValue to store the result
    //    //BaseValue resultVal(v1.type_index, 1);  // Only one element in the result
    //    //std::memcpy(resultVal.data, &result, sizeof(T));  // Copy the result back into the data

    //    //return resultVal;
    //}



    //template<typename T>
    //static BaseValue AddTyped(const BaseValue& v1, const BaseValue& v2)
    //{
    //    return HandleTyped<T>(v1, v2, [](T a, T b) {
    //        return a + b;
    //    });
    //}
    static BaseValue* ParseStringString(const BaseValue* var1, BaseValue* result)
    {
        //BaseValue* result = new BaseValue(vt_string, 0);
        BaseValue::ExecuteSingleTypedTemplate(BaseValue::ParseStringTypedFunctor(), var1, result);
        return result;
    }
    static struct ParseStringTypedFunctor {
        template <typename T>
        void operator()(const BaseValue* v1, BaseValue* outsideResult) const {
            //outsideResult will have the TARGET type and a fallback value
            //v1 will be a string
            //TODO: check vt.type_index for vt_string

            //we need to convert these and swap and mangle them a bit to get what we need:
            
            //v1->data = outsideResult->data;
            //outsideResult->data = v1->data;
            // 
            //v1->length = outsideResult->length;
            //outsideResult->length = v1->length;
            //TODO: swap v1 and outsideResult's data array and length (prob using temp value)


            // if we did all this then:
            //T var - will be the outsideResult (basivly we use it to get the type and a fallback value)
            //BaseValue* result - will be the input string (we smuggle it)
            //BaseValue* result - will also be the output of the T var (we overwrite its data [and length])

            HandleSingleTyped<T>(outsideResult, *v1, [](T var, BaseValue* result) {

                //T will be the target type
                //result.type_index will be the correct type index for our requested type
                //despite that result->data will just the the input string (we smuggle it)
                //result->length will be correct too
                std::string str;
                std::memcpy(str, result->data, result->length);
                //we stored our string so we can now remove data and length and fix them for our type_index

                T resultNum = var; //var is the default/fallback value as well!


                //now we need to do some logic to convert our string to T resultNum
                //TODO: convert str to resultNum

                result->length = BaseValue::getTypeSize(result->type_index);
                delete[] result->data;
                result->data = new uint8_t[result->length]; // or malloc if you want C-style
                //and then turn resultNum into a byte array and store it in BaseValue* result
                //TODO:set/fill result->data with the raw bytes of resultNum


            });
        }
    };

    static BaseValue* ToString(const BaseValue* var1) 
    {
        BaseValue* result = new BaseValue(vt_string, 0);
        BaseValue::ExecuteSingleTypedTemplate(BaseValue::ToStringTypedFunctor(), var1, result);
        return result;
    }
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

    template<typename T>
    static void HandleSingleTyped(BaseValue* result, const BaseValue& v1, std::function<void(T, BaseValue*)> operation) {
        if (sizeof(T) != getTypeSize(v1.type_index)) {
            DIE << "Size mismatch between T and BaseValue type";
        }
        T* data1 = reinterpret_cast<T*>(v1.data);

        operation(*data1, result);

        //return makeValue<T>(v1.type_index, result);
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
        //case vt_string:  func.template operator()<std::string>(v1, result); break;
        default:
            DIE << "Unsupported type in ExecuteTyped";
        }

        return result;
    }

    ////BaseValue ExecuteTyped(Func func, const BaseValue& v1, const BaseValue& v2);
    //template <typename Func>
    //static BaseValue* ExecuteTyped(Func func, const BaseValue* v1, const BaseValue* v2){ //, BaseValue* result) {
    //    if (!v1 || !v2) {
    //        DIE << "Null pointer passed to ExecuteTyped";
    //        //return nullptr;
    //    }


    //    // Dispatch based on type
    //    switch (v1->type_index) {
    //    case vt_uint8_t:   return func.template operator()<uint8_t>(v1, v2); break;
    //    case vt_uint16_t:  return func.template operator()<uint16_t>(v1, v2); break;
    //    case vt_uint32_t:  return func.template operator()<uint32_t>(v1, v2); break;
    //    case vt_uint64_t:  return func.template operator()<uint64_t>(v1, v2); break;
    //    case vt_int8_t:    return func.template operator()<int8_t>(v1, v2); break;
    //    case vt_int16_t:   return func.template operator()<int16_t>(v1, v2); break;
    //    case vt_int32_t:   return func.template operator()<int32_t>(v1, v2); break;
    //    case vt_int64_t:   return func.template operator()<int64_t>(v1, v2); break;
    //    case vt_float_t:   return func.template operator()<float>(v1, v2); break;
    //    case vt_double_t:  return func.template operator()<double>(v1, v2); break;
    //    default:
    //        DIE << "Unsupported type in ExecuteTyped";
    //    }

    //    return nullptr;
    //}

    //static struct AddTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a + b;
    //            });
    //    }
    //};
    //static struct SubTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a - b;
    //            });
    //    }
    //};
    //static struct MulTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a * b;
    //            });
    //    }
    //};
    //static struct DivTypedFunctor {
    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return HandleTyped<T>(*v1, *v2, [](T a, T b) {
    //            return a / b;
    //            });
    //    }
    //};
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
    //static struct RndTypedFunctor {
    //    mutable uint64_t seed = 0xDEADBABE12345678; // you can make it settable if u want :)

    //    template <typename T>
    //    BaseValue* operator()(const BaseValue* v1, const BaseValue* v2) const {
    //        return BaseValue::HandleTyped<T>(*v1, *v2, [this](T a, T b) -> T {
    //            if (a > b) std::swap(a, b);

    //            // === custom LCG RNG ===
    //            seed = seed * 6364136223846793005ULL + 1;
    //            uint64_t randVal = (seed >> 16) & 0xFFFFFFFF;

    //            if constexpr (std::is_integral_v<T>) {
    //                T range = b - a + 1;
    //                return a + (randVal % range);
    //            }
    //            else if constexpr (std::is_floating_point_v<T>) {
    //                double norm = static_cast<double>(randVal) / static_cast<double>(0xFFFFFFFF);
    //                return static_cast<T>(a + norm * (b - a));
    //            }
    //            else {
    //                DIE << "Unsupported type in RndTypedFunctor";
    //            }
    //            });
    //    }
    //};


};