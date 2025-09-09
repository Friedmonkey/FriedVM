#pragma once

enum ValueType
{
    vt_raw =            0x00,
    vt_string =         0x01,   //not that complex but it does store its length

    //complex types
    vt_lazy =           0x0A,   // for lazy values, gets loaded one time lazy<string>
    vt_constant =       0x0B,   // for constant values (header only) constant<int>
    vt_complex_type =   0x0C,   // only used in header, basicly means Read TypedByte system() usefull if u reuse a certain type
    vt_struct =         0x0D,   // struct { field int age; }
    vt_array =          0x0E,   // array[5]<int>, arr[5]<char> 
    vt_pointer =        0x0F,   // ptr<int>, pointer<int>

    vt_label =          0xE0,   // :hello //will be 0B once const is gone

    vt_bool =           0x10,   //true or false

    vt_uint8_t =        0x20,   // none - char, byte, uint8, u8
    vt_uint16_t =       0x22,   // none - ushort, uint16, u16
    vt_uint32_t =       0x23,   // 50u  - uint, uint32, u32
    vt_uint64_t =       0x24,   // 50ul - ulong, uint64, u64
    vt_int8_t =         0x25,   // none - sbyte, int8, i8
    vt_int16_t =        0x26,   // none - short, int16, i16
    vt_int32_t =        0x27,   // 50   - the default - int, int32, i32
    vt_int64_t =        0x28,   // 50l  - long, int64, i64
    vt_float_t =        0x29,   // 50f  - float, float32, f32
    vt_double_t =       0x2A,   // 50d  - double, float64, f64
};