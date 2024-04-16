//
// Created by Andrey Rodionov on 29/03/2024.
//

#ifndef NF_DEMO_COMMON_H
#define NF_DEMO_COMMON_H

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

typedef struct {
  uint16_t type;
  esp_bt_uuid_t uuid;
  uint16_t handle;
  esp_attr_value_t value;
  esp_attr_control_t control;
  esp_gatt_srvc_id_t id;
  uint16_t service_handle;
  uint16_t char_handle;
  esp_gatt_char_prop_t property;

  void (*read_callback)(esp_ble_gatts_cb_param_t *);

  void (*write_callback)(void *item, esp_ble_gatts_cb_param_t *);
} ble_item;

typedef struct {
  size_t item_count;
  size_t current_item;
  ble_item *items;
} service;

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

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;

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

static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

#endif //NF_DEMO_COMMON_H
