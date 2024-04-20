#ifndef NF_DEMO_COMMON_H
#define NF_DEMO_COMMON_H

#include <stdint.h>

#define PROTOCOL_VERSION 1

typedef enum {
  NOTIFICATION_REALTIME = 0,
  NOTIFICATION_GRAPH = 1,
  NOTIFICATION_END_OF_GRAPH = 2
} notification_type_t;

typedef struct __attribute__((packed)) {
  uint64_t timestamp;
  float value;
} data_point_t;

typedef enum {
  DAY = 0,
  MONTH = 1,
  YEAR = 2
} graph_interval_t;

enum {
  TYPE_PRIMARY_SERVICE,
  TYPE_CHARACTERISTIC,
  TYPE_DESCRIPTION
};

/**
 * @note See Bluetooth Specification 4.0 (Vol. 3), Part G, Section 3.3.3.5.
 */
typedef struct __attribute__((packed)) {
  /**
   * Format of the value.
   * @see https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf?v=1710672234752
   */
  uint8_t gatt_format;
  /**
   * Exponent for integer data types.
   * Example: if Exponent = -3 and the char value is 3892, the actual value is 3.892
   */
  int8_t exponent;
  /**
   * Unit of the characteristic value.
   *
   * @see https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf?v=1710672234752
   */
  uint16_t gatt_unit;
  /**
   * Namespace of the description field.
   * @note The namespace of the Bluetooth Body is 0x01.
   */
  uint8_t gatt_namespace;
  /**
   *  Space description
   *  @note The value 0x0000 means unknown in the Bluetooth namespace.
   */
  uint16_t gatt_nsdesc;
} presentation_format_t;

extern presentation_format_t utf8_string_format;
extern presentation_format_t float_format;


#endif //NF_DEMO_COMMON_H
