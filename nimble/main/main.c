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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_bt.h>
#include <time.h>

#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>
#include <host/ble_gap.h>
#include <host/util/util.h>

#include "service1.h"
#include "service2.h"
#include "common.h"

#include "sdkconfig.h"

#define GATTS_TAG "NF-DEMO"
#define TEST_DEVICE_NAME "NF-3bc4e"


TaskHandle_t handle_interrupt;


typedef struct {
  bool active;
  uint16_t conn_id;
  bool la_notify_enabled;
  uint16_t mtu;
} client_info_t;

typedef struct {
  client_info_t client[CONFIG_BT_NIMBLE_MAX_CONNECTIONS];
} global_app;

global_app app;

extern void bleprph_advertise();

//void service2_la_callback(esp_ble_gatts_cb_param_t *param) {
//  esp_gatt_rsp_t rsp;
//  memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
//  rsp.attr_value.handle = param->read.handle;
//  float la = (float) rand() / 100;
//  rsp.attr_value.len = sizeof(la);
//  memcpy(rsp.attr_value.value, &la, rsp.attr_value.len);
//  esp_ble_gatts_send_response(app.gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
//}

//void service2_la_graph_write_callback(void *payload, esp_ble_gatts_cb_param_t *param) {
//  if (param->write.len < 1) {
//    ESP_LOGI(GATTS_TAG, "write len %d", param->write.len);
//    return;
//  }
//  if (param->write.is_prep) {
//    ESP_LOGI(GATTS_TAG, "write len %d", param->write.is_prep);
//    return;
//  }
//  ble_item *item = (ble_item *) payload;
//  uint8_t request_type = param->write.value[0];
//
//  data_point_t *graph_to_send = NULL;
//  size_t to_send_bytes = 0;
//  switch (request_type) {
//    case DAY: {
//      graph_to_send = (data_point_t *) daily;
//      to_send_bytes = sizeof(daily);
//      break;
//    }
//    case MONTH: {
//      graph_to_send = (data_point_t *) monthly;
//      to_send_bytes = sizeof(monthly);
//      break;
//    }
//    case YEAR: {
//      graph_to_send = (data_point_t *) yearly;
//      to_send_bytes = sizeof(yearly);
//      break;
//    }
//    default:
//      return;
//  }
//  uint16_t mtu = 0;
//  for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
//    if (app.client[i].conn_id == param->write.conn_id) {
//      mtu = app.client[i].mtu;
//      break;
//    }
//  }
//  if (mtu == 0) {
//    return;
//  }
//  size_t points = to_send_bytes / sizeof(data_point_t);
//  size_t data_point_size = sizeof(data_point_t);
//  size_t rounded_mtu = ((mtu - 3) / data_point_size) * data_point_size + 3; //1 is for protocol, 1 is for message type and 1 for number of points
//  size_t number_of_batches = to_send_bytes / rounded_mtu;
//  if ((to_send_bytes % rounded_mtu) != 0) {
//    number_of_batches++;
//  }
//  size_t number_of_points_in_batch = (rounded_mtu - 3) / data_point_size;
//  ESP_LOGI(GATTS_TAG, "sending batches. points: %zu mtu: %d rounded mtu: %d number of batches: %zu", points, mtu, rounded_mtu, number_of_batches);
//  temp_buffer[0] = PROTOCOL_VERSION;
//  temp_buffer[1] = NOTIFICATION_GRAPH;
//  for (size_t i = 0; i < number_of_batches; i++) {
//    uint8_t current_number_of_points = (uint8_t) number_of_points_in_batch;
//    if (((i + 1) * number_of_points_in_batch) > points) {
//      current_number_of_points = (uint8_t) (points - i * number_of_points_in_batch);
//    }
//    ESP_LOGI(GATTS_TAG, "sending %zu", current_number_of_points);
//    temp_buffer[2] = current_number_of_points;
//    memcpy(temp_buffer + 3, graph_to_send + i * number_of_points_in_batch, current_number_of_points * data_point_size);
//    esp_ble_gatts_send_indicate(app.gatts_if, param->write.conn_id, item->char_handle, current_number_of_points * data_point_size + 3, temp_buffer, false);
//  }
//  temp_buffer[1] = NOTIFICATION_END_OF_GRAPH;
//  esp_ble_gatts_send_indicate(app.gatts_if, param->write.conn_id, item->char_handle, 2, temp_buffer, false);
//}

//void service2_la_write_callback(void *payload, esp_ble_gatts_cb_param_t *param) {
//  if (param->write.len < 1) {
//    ESP_LOGI(GATTS_TAG, "write len %d", param->write.len);
//    return;
//  }
//  if (param->write.is_prep) {
//    ESP_LOGI(GATTS_TAG, "write len %d", param->write.is_prep);
//    return;
//  }
//  ble_item *item = (ble_item *) payload;
//  uint8_t descr_value = param->write.value[0];
//  if (descr_value == 0x01) {
//    for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
//      if (app.client[i].conn_id == param->write.conn_id) {
//        app.client[i].la_char_handle = item->char_handle;
//        app.client[i].la_notify_enabled = true;
//        ESP_LOGI(GATTS_TAG, "notify enable conn %d char %d", app.client[i].conn_id, app.client[i].la_char_handle);
//        break;
//      }
//    }
//  } else if (descr_value == 0x02) {
//    ESP_LOGI(GATTS_TAG, "indicate enable");
//  } else if (descr_value == 0x00) {
//    for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
//      if (app.client[i].conn_id == param->write.conn_id) {
//        app.client[i].la_notify_enabled = false;
//        ESP_LOGI(GATTS_TAG, "notify disabled conn %d char %d", app.client[i].conn_id, app.client[i].la_char_handle);
//        break;
//      }
//    }
//  } else {
//    ESP_LOGE(GATTS_TAG, "unknown descr value");
//        esp_log_buffer_hex(GATTS_TAG, param->write.value, param->write.len);
//  }
//}

bool has_la_notifications_enabled() {
  for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
    if (app.client[i].la_notify_enabled) {
      return true;
    }
  }
  return false;
}

void handle_interrupt_task(void *arg) {
// Block for 500ms.
  const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
  size_t notification_buffer_length = 2 + sizeof(float);
  uint8_t *notification_buffer = malloc(sizeof(uint8_t) * notification_buffer_length);
  for (;;) {
    if (has_la_notifications_enabled()) {
      // somewhat expensive calculation
      float la = (float) rand() / 100;
      notification_buffer[0] = PROTOCOL_VERSION;
      notification_buffer[1] = NOTIFICATION_REALTIME;
      memcpy(notification_buffer + 1, &la, sizeof(la));
      for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
        if (!app.client[i].la_notify_enabled) {
          continue;
        }
        struct os_mbuf *txom = ble_hs_mbuf_from_flat(notification_buffer, notification_buffer_length);
        ble_gatts_notify_custom(app.client[i].conn_id, service2_la_val_handle, txom);
      }
    }
    vTaskDelay(xDelay);
  }
}

static int gatts_event_handler(struct ble_gap_event *event, void *arg) {
  switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
      ESP_LOGI(GATTS_TAG, "connection %s; conn_handle=%d status=%d ", event->connect.status == 0 ? "established" : "failed", event->connect.conn_handle, event->connect.status);
      // search first not active and take it
      for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
        if (app.client[i].active) {
          continue;
        }
        app.client[i].active = true;
        app.client[i].conn_id = event->connect.conn_handle;
        break;
      }
      bleprph_advertise();
      return 0;
    case BLE_GAP_EVENT_DISCONNECT:
      ESP_LOGI(GATTS_TAG, "disconnect; conn_handle=%d reason=%d", event->disconnect.conn.conn_handle, event->disconnect.reason);
      for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
        if (app.client[i].conn_id != event->disconnect.conn.conn_handle) {
          continue;
        }
        app.client[i].active = false;
        break;
      }
      return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
      ESP_LOGI(GATTS_TAG, "connection updated; conn_handle=%d status=%d ", event->conn_update.conn_handle, event->conn_update.status);
      return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
      ESP_LOGI(GATTS_TAG, "advertise complete; reason=%d", event->adv_complete.reason);
      bleprph_advertise();
      return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
      //ESP_LOGI(GATTS_TAG, "notify_tx event; conn_handle=%d attr_handle=%d status=%d is_indication=%d", event->notify_tx.conn_handle, event->notify_tx.attr_handle, event->notify_tx.status, event->notify_tx.indication);
      return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
      ESP_LOGI(GATTS_TAG, "subscribe event; conn_handle=%d attr_handle=%d reason=%d prevn=%d curn=%d previ=%d curi=%d", event->subscribe.conn_handle, event->subscribe.attr_handle, event->subscribe.reason, event->subscribe.prev_notify, event->subscribe.cur_notify, event->subscribe.prev_indicate,
               event->subscribe.cur_indicate);
      if (event->subscribe.attr_handle == service2_la_val_handle) {
        for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
          if (event->subscribe.cur_notify) {
            if (app.client[i].conn_id == event->subscribe.conn_handle) {
              app.client[i].la_notify_enabled = true;
              break;
            }
          } else {
            if (app.client[i].conn_id == event->subscribe.conn_handle) {
              app.client[i].la_notify_enabled = false;
              break;
            }
          }
        }
      }
      return 0;

    case BLE_GAP_EVENT_MTU:
      ESP_LOGI(GATTS_TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d", event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
      for (int i = 0; i < CONFIG_BT_NIMBLE_MAX_CONNECTIONS; i++) {
        if (app.client[i].conn_id != event->mtu.conn_handle) {
          continue;
        }
        app.client[i].mtu = event->mtu.value;
        break;
      }
      return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
    case BLE_GAP_EVENT_REPEAT_PAIRING:
    case BLE_GAP_EVENT_PASSKEY_ACTION:
      return 0;
  }

  return 0;
}

void bleprph_advertise() {
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  const char *name;
  int rc;

  /**
   *  Set the advertisement data included in our advertisements:
   *     o Flags (indicates advertisement type and other general info).
   *     o Advertising tx power.
   *     o Device name.
   *     o 16-bit service UUIDs (alert notifications).
   */

  memset(&fields, 0, sizeof fields);

  /* Advertise two flags:
   *     o Discoverability in forthcoming advertisement (general)
   *     o BLE-only (BR/EDR unsupported).
   */
  fields.flags = BLE_HS_ADV_F_DISC_GEN |
                 BLE_HS_ADV_F_BREDR_UNSUP;

  /* Indicate that the TX power level field should be included; have the
   * stack fill this value automatically.  This is done by assigning the
   * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
   */
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  name = ble_svc_gap_device_name();
  fields.name = (uint8_t *) name;
  fields.name_len = strlen(name);
  fields.name_is_complete = 1;
  fields.uuids16 = NULL;
  fields.num_uuids16 = 0;
  fields.uuids16_is_complete = 1;

  rc = ble_gap_adv_set_fields(&fields);
  if (rc != 0) {
    MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
    return;
  }

  /* Begin advertising. */
  memset(&adv_params, 0, sizeof adv_params);
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &adv_params, gatts_event_handler, NULL);
  if (rc != 0) {
    MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
    return;
  }
}

void ble_client_host_task(void *param) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

static void ble_client_on_reset(int reason) {
  ESP_LOGE(GATTS_TAG, "resetting state. reason: %d", reason);
}

static void ble_client_on_sync(void) {
  esp_err_t code = ble_hs_util_ensure_addr(0);
  if (code != ESP_OK) {
    ESP_LOGE(GATTS_TAG, "unable to ensure address: %d", code);
  }
  bleprph_advertise();
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
  char buf[BLE_UUID_STR_LEN];

  switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
      MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                  ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                  ctxt->svc.handle);
      break;

    case BLE_GATT_REGISTER_OP_CHR:
      MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                         "def_handle=%d val_handle=%d\n",
                  ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                  ctxt->chr.def_handle,
                  ctxt->chr.val_handle);
      break;

    case BLE_GATT_REGISTER_OP_DSC:
      MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                  ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                  ctxt->dsc.handle);
      break;

    default:
      assert(0);
      break;
  }
}

void app_main(void) {
  srand(time(NULL));
  esp_err_t ret;

  // Initialize NVS.
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ble_hs_cfg.reset_cb = ble_client_on_reset;
  ble_hs_cfg.sync_cb = ble_client_on_sync;
  ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;

  ESP_ERROR_CHECK(nimble_port_init());

  service1_init();
  service2_init();

  ESP_ERROR_CHECK((esp_err_t) ble_svc_gap_device_name_set(TEST_DEVICE_NAME));
  nimble_port_freertos_init(ble_client_host_task);

  BaseType_t task_code = xTaskCreatePinnedToCore(handle_interrupt_task, "handle interrupt", 8196, NULL, 2, &handle_interrupt, xPortGetCoreID());
  if (task_code != pdPASS) {
    return;
  }

  uint8_t uuid[] = {0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0, 0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0};
  ESP_LOGI(GATTS_TAG, "https://nfinterface.com/scan?name=%s&serviceUuid=%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", TEST_DEVICE_NAME, uuid[15], uuid[14], uuid[13], uuid[12], uuid[11], uuid[10], uuid[9], uuid[8], uuid[7], uuid[6], uuid[5], uuid[4], uuid[3], uuid[2],
           uuid[1], uuid[0]);
  uint8_t uuid2[] = {0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc, 0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc};
  ESP_LOGI(GATTS_TAG, "https://nfinterface.com/scan?name=%s&serviceUuid=%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", TEST_DEVICE_NAME, uuid2[15], uuid2[14], uuid2[13], uuid2[12], uuid2[11], uuid2[10], uuid2[9], uuid2[8], uuid2[7], uuid2[6], uuid2[5], uuid2[4], uuid2[3],
           uuid2[2],
           uuid2[1], uuid2[0]);
}
