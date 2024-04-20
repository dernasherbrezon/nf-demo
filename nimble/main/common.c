#include "common.h"

presentation_format_t utf8_string_format = {
    .gatt_format = 0x19, //utf8
    .exponent = 0x00,
    .gatt_unit = 0x2700, //unitless
    .gatt_namespace = 0x00,
    .gatt_nsdesc = 0x0000
};

presentation_format_t float_format = {
    .gatt_format = 0x14, //4-byte float
    .exponent = 0x00,
    .gatt_unit = 0x2700, //unitless
    .gatt_namespace = 0x00,
    .gatt_nsdesc = 0x0000
};