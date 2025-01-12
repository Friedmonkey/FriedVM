#pragma once
#include <cstdint>
#include <memory>
#include "../Logger.h"

enum ValueType
{
	//vt_bool,
	//vt_char,
	vt_uint8_t,
	vt_uint16_t,
	vt_uint32_t,
	vt_uint64_t,

	vt_int8_t,
	vt_int16_t,
	vt_int32_t,
	vt_int64_t,

	vt_float_t,
	vt_double_t,
};
class BaseValue
{
//public:
//	BaseValue();

public:
uint32_t index_value; //either index or value, not sure if we even need this yet


virtual bool IsNumberType() const = 0;
virtual ValueType GetValueType() const = 0;

virtual void GetDataFromValue(uint8_t **data, size_t *length) = 0;
virtual void SetValueFromData(uint8_t *data, size_t length) = 0;
};
