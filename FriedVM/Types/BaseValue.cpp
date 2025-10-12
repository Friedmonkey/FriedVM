#pragma once
#include "BaseValue.h"

// Function to get the size of the element based on the type
size_t BaseValue::getTypeSize(ValueType type) {
    switch (type) {
    case vt_raw: return 0;
    case vt_string: return 0;
    case vt_interpolated_string: return 0;
    case vt_bool: return 1;
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
    case vt_label: return sizeof(uint64_t);
    case vt_array: return 0;
    default: DIE << "unhandled type";
    }
}

void BaseValue::FillDefaultValue(BaseValue *value) {
    switch (value->type_index) {
        //case vt_raw: return 4;
    case vt_bool: value->data = new uint8_t[1]{ 0x00 }; break;
    case vt_uint8_t: value->data = new uint8_t[1]{ 0x00 }; break;
    case vt_uint16_t: value->data = new uint8_t[2]{ 0x00, 0x00 }; break;
    case vt_uint32_t: value->data = new uint8_t[4]{ 0x00, 0x00, 0x00, 0x00 }; break;
    case vt_uint64_t: value->data = new uint8_t[8]{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; break;
    case vt_int8_t: value->data = new uint8_t[1]{ 0x00 }; break;
    case vt_int16_t: value->data = new uint8_t[2]{ 0x00, 0x00 }; break;
    case vt_int32_t: value->data = new uint8_t[4]{ 0x00, 0x00, 0x00, 0x00 }; break;
    case vt_int64_t: value->data = new uint8_t[8]{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; break;
    case vt_float_t: value->data = new uint8_t[4]{ 0x00, 0x00, 0x00, 0x00 }; break;
    case vt_double_t: value->data = new uint8_t[8]{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; break;
    //case vt_pointer: return 4;
    case vt_string: value->data = new uint8_t[0]; break;
    case vt_interpolated_string: value->data = new uint8_t[0]; break;
    case vt_label: value->data = new uint8_t[8]{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; break; //default label just points to start of binary
    default: DIE << "unhandled type"; break;
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
    if (type_index == vt_string)
    {
        std::string str(reinterpret_cast<char*>(data), length);
        return str;
    }
    BaseValue* strValue = ToString(this);
    std::string str(reinterpret_cast<char*>(strValue->data), strValue->length);
    return str;
}

bool BaseValue::isBoolTrue() const 
{
    if (length == 0)
        return false;
    if (type_index != vt_bool)
        DIE << "Not a bool";

    return (data[0]);
}
bool BaseValue::isTruthy() const {
    if (length == 0)
        return false;

    switch (type_index)
    {
    //case vt_raw:
    //    break;
    //case vt_string:
    //    break;
    //case vt_lazy:
    //    break;
    //case vt_label:
    //    break;
    //case vt_complex_type:
    //    break;
    //case vt_struct:
    //    break;
    //case vt_array:
    //    break;
    //case vt_pointer:
    //    break;
    case vt_bool:
        return (data[0]);
    case vt_uint8_t:
    case vt_uint16_t:
    case vt_uint32_t:
    case vt_uint64_t:
    case vt_int8_t:
    case vt_int16_t:
    case vt_int32_t:
    case vt_int64_t:
    case vt_float_t:
    case vt_double_t:
    {
        auto True = BaseValue::makeValue(vt_bool, true);
        auto out = BaseValue::computeNumbers(True, this, EQCompOperation{});
        if (out->type_index == vt_bool)
            return (out->data[0]);

        return false;
    }
    default:
        DIE << "Unhandled type for truthy check";
        break;
    }
}

