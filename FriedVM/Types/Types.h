#pragma once

enum ValueType
{
    vt_raw =            0x00,
    vt_string =         0x01,   //not that complex but it does store its length

    vt_uint8_t =        0x10,   // none - char, byte, uint8, u8
    vt_uint16_t =       0x12,   // none - ushort, uint16, u16
    vt_uint32_t =       0x13,   // 50u  - uint, uint32, u32
    vt_uint64_t =       0x14,   // 50ul - ulong, uint64, u64
    vt_int8_t =         0x15,   // none - sbyte, int8, i8
    vt_int16_t =        0x16,   // none - short, int16, i16
    vt_int32_t =        0x17,   // 50   - the default - int, int32, i32
    vt_int64_t =        0x18,   // 50l  - long, int64, i64
    vt_float_t =        0x19,   // 50f  - float, float32, f32
    vt_double_t =       0x1A,   // 50d  - double, float64, f64

    vt_label =          0xE0,   // :hello

    //complex types
    vt_constant =       0xFB,   // for constant values (header only) constant<int>
    vt_complex_type =   0xFC,   // only used in header, basicly means Read TypedByte system() usefull if u reuse a certain type
    vt_struct =         0xFD,   // struct { field int age; }
    vt_array =          0xFE,   // array[5]<int>, arr[5]<char> 
    vt_pointer =        0xFF,   // ptr<int>, pointer<int>
};