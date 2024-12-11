#pragma once

#include "Logger.h"

enum ByteTag {
	IMMEDIATE_UINT8 = 0,
	IMMEDIATE_UINT16 = 1,
	IMMEDIATE_UINT24 = 2,
	IMMEDIATE_UINT32 = 4,
	//HEAP_INT128,
	//HEAP_ARRAY,
	//HEAP_STRING
};

class ByteTranslator
{
public:
	static uint8_t GetByteCount(const ByteTag tag)
	{
		switch (tag)
		{
		case IMMEDIATE_UINT8:
			return 1;
		case IMMEDIATE_UINT16:
			return 2;
		case IMMEDIATE_UINT24:
			return 3;
		case IMMEDIATE_UINT32:
			return 4;
		default:
			DIE << "Unknow byteTag:" << NUM(tag);
			break;
		}
	}
	static ByteTag GetByteTag(const uint8_t count)
	{
		switch (count)
		{
		case 1:
			return IMMEDIATE_UINT8;
		case 2:
			return IMMEDIATE_UINT16;
		case 3:
			return IMMEDIATE_UINT24;
		case 4:
			return IMMEDIATE_UINT32;
		default:
			DIE << "Unknow byteTag:" << NUM(count);
			break;
		}
	}

	// Get 8-bit value (just return the first byte)
	static uint8_t Get8(uint8_t* array, ByteTag tag) {
		return array[0];
	}

	// Get 16-bit value (combine two bytes)
	static uint16_t Get16(uint8_t* array, ByteTag tag) {
		return (static_cast<uint16_t>(array[0]) << 8) | array[1];
	}

	// Get 24-bit value (combine three bytes)
	static uint32_t Get24(uint8_t* array, ByteTag tag) {
		return (static_cast<uint32_t>(array[0]) << 16) | (static_cast<uint32_t>(array[1]) << 8) | array[2];
	}

	// Get 32-bit value (combine four bytes)
	static uint32_t Get32(uint8_t* array, ByteTag tag) {
		return (static_cast<uint32_t>(array[0]) << 24) | (static_cast<uint32_t>(array[1]) << 16) |
			(static_cast<uint32_t>(array[2]) << 8) | array[3];
	}

	static uint8_t* Set8(uint8_t value, ByteTag tag) {
		uint8_t* array = new uint8_t[1];
		array[0] = value;
		return array;
	}

	// Reverse: Convert 16-bit to byte array
	static uint8_t* Set16(uint16_t value, ByteTag tag) {
		uint8_t* array = new uint8_t[2];
		array[0] = static_cast<uint8_t>(value >> 8);  // MSB
		array[1] = static_cast<uint8_t>(value & 0xFF);  // LSB
		return array;
	}

	// Reverse: Convert 24-bit to byte array
	static uint8_t* Set24(uint32_t value, ByteTag tag) {
		uint8_t* array = new uint8_t[3];
		array[0] = static_cast<uint8_t>(value >> 16);  // MSB
		array[1] = static_cast<uint8_t>((value >> 8) & 0xFF);  // Middle byte
		array[2] = static_cast<uint8_t>(value & 0xFF);  // LSB
		return array;
	}

	// Reverse: Convert 32-bit to byte array
	static uint8_t* Set32(uint32_t value, ByteTag tag) {
		uint8_t* array = new uint8_t[4];
		array[0] = static_cast<uint8_t>(value >> 24);  // MSB
		array[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
		array[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
		array[3] = static_cast<uint8_t>(value & 0xFF);  // LSB
		return array;
	}
};