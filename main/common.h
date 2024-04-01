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

enum {
  TYPE_PRIMARY_SERVICE,
  TYPE_CHARACTERISTIC,
  TYPE_DESCRIPTION
};

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t utf8_string_format = 0x19;
static const uint8_t float_format = 0x14;

static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

#endif //NF_DEMO_COMMON_H
