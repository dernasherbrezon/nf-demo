/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE GATT server. It can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server demo.
* Client demo will enable gatt_server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include <time.h>

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "service1.h"
#include "service2.h"

#include "sdkconfig.h"

#define GATTS_TAG "NF-DEMO"

#define TEST_DEVICE_NAME            "NF-3bc4e"

#define PREPARE_BUF_MAX_SIZE 1024

static uint8_t adv_config_done = 0;
#define adv_config_flag      (1 << 0)
#define scan_rsp_config_flag (1 << 1)

TaskHandle_t handle_interrupt;


// The length of adv data must be less than 31 bytes
//adv data
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,
    .p_service_uuid = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

typedef struct {
  uint16_t gatts_if;
  uint16_t conn_id;
  size_t item_count;
  size_t current_item;
  uint16_t current_char_handle;
  service *services;
} global_app;

global_app app;

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
      adv_config_done &= (~adv_config_flag);
      if (adv_config_done == 0) {
        esp_ble_gap_start_advertising(&adv_params);
      }
      break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
      adv_config_done &= (~scan_rsp_config_flag);
      if (adv_config_done == 0) {
        esp_ble_gap_start_advertising(&adv_params);
      }
      break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
      //advertising start complete event to indicate advertising start successfully or failed
      if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
        ESP_LOGE(GATTS_TAG, "Advertising start failed");
      }
      break;
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
      if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
        ESP_LOGE(GATTS_TAG, "Advertising stop failed");
      } else {
        ESP_LOGI(GATTS_TAG, "Stop adv successfully");
      }
      break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
      ESP_LOGI(GATTS_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
               param->update_conn_params.status,
               param->update_conn_params.min_int,
               param->update_conn_params.max_int,
               param->update_conn_params.conn_int,
               param->update_conn_params.latency,
               param->update_conn_params.timeout);
      break;
    default:
      break;
  }
}

static void register_item(esp_gatt_if_t gatts_if, service *cur_service, ble_item *cur_item) {
  switch (cur_item->type) {
    case TYPE_PRIMARY_SERVICE: {
      esp_ble_gatts_create_service(gatts_if, &cur_item->id, 2 * cur_service->item_count);
      break;
    }
    case TYPE_CHARACTERISTIC: {
      esp_err_t add_char_ret = esp_ble_gatts_add_char(cur_service->items->handle, &cur_item->uuid,
                                                      ESP_GATT_PERM_READ,
                                                      cur_item->property,
                                                      &cur_item->value, &cur_item->control);
      if (add_char_ret) {
        ESP_LOGE(GATTS_TAG, "add char failed, error code =%x", add_char_ret);
      }
      break;
    }
    case TYPE_DESCRIPTION: {
      esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(cur_service->items->handle, &cur_item->uuid,
                                                             ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, &cur_item->value, &cur_item->control);
      if (add_descr_ret) {
        ESP_LOGE(GATTS_TAG, "add char descr failed, error code =%x", add_descr_ret);
      }
      break;
    }
  }
}

void service2_la_callback(esp_ble_gatts_cb_param_t *param) {
  esp_gatt_rsp_t rsp;
  memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
  rsp.attr_value.handle = param->read.handle;
  float la = (float) rand() / 100;
  rsp.attr_value.len = sizeof(la);
  memcpy(rsp.attr_value.value, &la, rsp.attr_value.len);
  esp_ble_gatts_send_response(app.gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
}

uint16_t global_conn_id;
uint16_t global_char_handle;
bool notify_enabled = false;

void service2_la_write_callback(void *payload, esp_ble_gatts_cb_param_t *param) {
  if (param->write.len < 1) {
    ESP_LOGI(GATTS_TAG, "write len %d", param->write.len);
    return;
  }
  if (param->write.is_prep) {
    ESP_LOGI(GATTS_TAG, "write len %d", param->write.is_prep);
    return;
  }
  ble_item *item = (ble_item *) payload;
  uint16_t descr_value = param->write.value[0];
  if (descr_value == 0x01) {
    // supported only single connection on a single characteristic
    global_conn_id = param->write.conn_id;
    global_char_handle = item->char_handle;
    ESP_LOGI(GATTS_TAG, "notify enable conn %d char %d", global_conn_id, global_char_handle);
    notify_enabled = true;
  } else if (descr_value == 0x02) {
    ESP_LOGI(GATTS_TAG, "indicate enable");
  } else if (descr_value == 0x00) {
    ESP_LOGI(GATTS_TAG, "notify/indicate disable ");
    notify_enabled = false;
  } else {
    ESP_LOGE(GATTS_TAG, "unknown descr value");
        esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
  }
}

void handle_interrupt_task(void *arg) {
// Block for 500ms.
  const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
  for (;;) {
    if (notify_enabled) {
      float la = (float) rand() / 100;
      esp_ble_gatts_send_indicate(app.gatts_if, global_conn_id, global_char_handle, sizeof(la), (uint8_t *) &la, false);
    }
    vTaskDelay(xDelay);
  }
}

static void register_next(esp_gatt_if_t gatts_if, uint16_t handle) {
  service *cur_service = app.services + app.current_item;
  ble_item *cur_item = cur_service->items + cur_service->current_item;
  cur_item->handle = handle;
  if (cur_item->type == TYPE_CHARACTERISTIC) {
    app.current_char_handle = handle;
  } else if (cur_item->type == TYPE_DESCRIPTION) {
    cur_item->char_handle = app.current_char_handle;
  }
  cur_service->current_item++;
  if (cur_service->current_item >= cur_service->item_count) {
    app.current_item++;
    if (app.current_item >= app.item_count) {
      return;
    }
    cur_service = app.services + app.current_item;
  }
  cur_item = cur_service->items + cur_service->current_item;
  register_item(gatts_if, cur_service, cur_item);
}

static void callback_read_event(esp_ble_gatts_cb_param_t *param) {
  for (size_t i = 0; i < app.item_count; i++) {
    for (size_t j = 0; j < app.services[i].item_count; j++) {
      ble_item *cur_item = &app.services[i].items[j];
      if (cur_item->handle == param->read.handle && cur_item->control.auto_rsp == ESP_GATT_RSP_BY_APP) {
        cur_item->read_callback(param);
      }
    }
  }
}

static void callback_write_event(esp_ble_gatts_cb_param_t *param) {
  for (size_t i = 0; i < app.item_count; i++) {
    for (size_t j = 0; j < app.services[i].item_count; j++) {
      ble_item *cur_item = &app.services[i].items[j];
      if (cur_item->handle == param->write.handle && cur_item->write_callback != NULL) {
        cur_item->write_callback(cur_item, param);
      }
    }
  }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
  switch (event) {
    case ESP_GATTS_REG_EVT: {
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_REG_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
      esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(TEST_DEVICE_NAME);
      if (set_dev_name_ret) {
        ESP_LOGE(GATTS_TAG, "set device name failed, error code = %x", set_dev_name_ret);
      }
      //config adv data
      esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
      if (ret) {
        ESP_LOGE(GATTS_TAG, "config adv data failed, error code = %x", ret);
      }
      adv_config_done |= adv_config_flag;
      //config scan response data
      ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
      if (ret) {
        ESP_LOGE(GATTS_TAG, "config scan response data failed, error code = %x", ret);
      }
      adv_config_done |= scan_rsp_config_flag;
      app.gatts_if = gatts_if;
      service *cur_service = app.services + app.current_item;
      ble_item *cur_item = cur_service->items + cur_service->current_item;
      register_item(gatts_if, cur_service, cur_item);
      break;
    }
    case ESP_GATTS_READ_EVT: {
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_READ_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
      callback_read_event(param);
      break;
    }
    case ESP_GATTS_WRITE_EVT: {
      ESP_LOGI(GATTS_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %" PRIu32 ", handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
      callback_write_event(param);
      break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
      break;
    case ESP_GATTS_MTU_EVT:
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
      break;
    case ESP_GATTS_CREATE_EVT:
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_CREATE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
      esp_ble_gatts_start_service(param->create.service_handle);
      register_next(gatts_if, param->create.service_handle);
      break;
    case ESP_GATTS_ADD_CHAR_EVT: {
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
               param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
      register_next(gatts_if, param->add_char.attr_handle);
      break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT: {
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_ADD_CHAR_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
               param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
      register_next(gatts_if, param->add_char_descr.attr_handle);
      break;
    }
    case ESP_GATTS_START_EVT:
      ESP_LOGI(GATTS_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
      break;
    case ESP_GATTS_CONNECT_EVT: {
      esp_ble_conn_update_params_t conn_params = {0};
      memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
      /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
      conn_params.latency = 0;
      conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
      conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
      conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
               param->connect.conn_id,
               param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
               param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
      app.conn_id = param->connect.conn_id;
      //start sent the update connection parameters to the peer device.
      esp_ble_gap_update_conn_params(&conn_params);
      break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
      esp_ble_gap_start_advertising(&adv_params);
      break;
    case ESP_GATTS_CONF_EVT:
      ESP_LOGI(GATTS_TAG, "ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
      if (param->conf.status != ESP_GATT_OK) {
            esp_log_buffer_hex(GATTS_TAG, param->conf.value, param->conf.len);
      }
      break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
    case ESP_GATTS_STOP_EVT:
    case ESP_GATTS_UNREG_EVT:
    case ESP_GATTS_DELETE_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
      break;
  }
}

void app_main(void) {
  srand(time(NULL));
  app.item_count = 2;
  app.services = malloc(sizeof(service) * app.item_count);
  app.current_item = 0;
  app.current_char_handle = 0;
  app.services[0] = service1;
  app.services[1] = service2;

  esp_err_t ret;

  // Initialize NVS.
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(GATTS_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
    return;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(GATTS_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
    return;
  }
  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(GATTS_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
    return;
  }
  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(GATTS_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
    return;
  }

  ret = esp_ble_gatts_register_callback(gatts_event_handler);
  if (ret) {
    ESP_LOGE(GATTS_TAG, "gatts register error, error code = %x", ret);
    return;
  }
  ret = esp_ble_gap_register_callback(gap_event_handler);
  if (ret) {
    ESP_LOGE(GATTS_TAG, "gap register error, error code = %x", ret);
    return;
  }
  ret = esp_ble_gatts_app_register(0);
  if (ret) {
    ESP_LOGE(GATTS_TAG, "gatts app register error, error code = %x", ret);
    return;
  }
  esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
  if (local_mtu_ret) {
    ESP_LOGE(GATTS_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
  }

  BaseType_t task_code = xTaskCreatePinnedToCore(handle_interrupt_task, "handle interrupt", 8196, NULL, 2, &handle_interrupt, xPortGetCoreID());
  if (task_code != pdPASS) {
    return;
  }
}
