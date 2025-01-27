#pragma once
#include "BaseValue.h"

// Function to get the size of the element based on the type
size_t BaseValue::getTypeSize(ValueType type) {
    switch (type) {
    //case vt_raw: return 4;
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
    default: DIE << "unhandled type";
    }
}

bool BaseValue::isNumber() const {
    // Return true if the type is one of the numeric types
    return type_index == vt_uint8_t || type_index == vt_uint16_t ||
        type_index == vt_uint32_t || type_index == vt_uint64_t ||
        type_index == vt_int8_t || type_index == vt_int16_t ||
        type_index == vt_int32_t || type_index == vt_int64_t ||
        type_index == vt_float_t || type_index == vt_double_t;
}

std::string BaseValue::toString() const
{
    return std::string("not implemented lol");
}

//// The main Add method to dispatch based on type
//template <typename Func>
//BaseValue BaseValue::ExecuteTyped(Func func,const BaseValue& v1, const BaseValue& v2) {
//    // Validate if both values are numbers
//    if (!v1.isNumber() || !v2.isNumber()) {
//        DIE << "Cannot add non-number values";
//    }
//    //if (v1.type_index != v2.type_index || v1.length != v2.length) {
//    //    // Cast v2 to the same type as v1
//    //    BaseValue casted = v2.Cast(v1.type_index);
//    //    return Add(v1, casted);  // Now they are both of the same type
//    //}
//
//    BaseValue result(v1.type_index, v1.length);
//
//    // Dispatch based on type
//    switch (v1.type_index) {
//    case vt_uint8_t:   result = func<uint8_t>(v1, v2); break;
//    case vt_uint16_t:  result = func<uint16_t>(v1, v2); break;
//    case vt_uint32_t:  result = func<uint32_t>(v1, v2); break;
//    case vt_uint64_t:  result = func<uint64_t>(v1, v2); break;
//    case vt_int8_t:    result = func<int8_t>(v1, v2); break;
//    case vt_int16_t:   result = func<int16_t>(v1, v2); break;
//    case vt_int32_t:   result = func<int32_t>(v1, v2); break;
//    case vt_int64_t:   result = func<int64_t>(v1, v2); break;
//    case vt_float_t:   result = func<float>(v1, v2); break;
//    case vt_double_t:  result = func<double>(v1, v2); break;
//    default:
//        DIE << "Unsupported type in Add";
//    }
//
//    return result;
//}