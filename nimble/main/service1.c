#include "service1.h"
#include <esp_err.h>
#include <host/ble_hs.h>
#include "common.h"

static const char service1_model_name[] = "Arrow II 146/437-10BP";
static const char service1_manuf_name[] = "Arrow Antenna";
static const char maxpower[] = "10W";
static const char maxpower_name[] = "Maximum power";

static int service1_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
  if (ctxt->chr != NULL) {
    if (ble_uuid_cmp(ctxt->chr->uuid, BLE_UUID16_DECLARE(0x2A24)) == 0) {
      int rc = os_mbuf_append(ctxt->om, &service1_model_name, sizeof(service1_model_name));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    if (ble_uuid_cmp(ctxt->chr->uuid, BLE_UUID16_DECLARE(0x2A29)) == 0) {
      int rc = os_mbuf_append(ctxt->om, &service1_manuf_name, sizeof(service1_manuf_name));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    if (ble_uuid_cmp(ctxt->chr->uuid, BLE_UUID128_DECLARE(0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6, 0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6)) == 0) {
      int rc = os_mbuf_append(ctxt->om, &maxpower, sizeof(maxpower));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
  }
  if (ctxt->dsc != NULL) {
    if (ble_uuid_cmp(ctxt->dsc->uuid, BLE_UUID16_DECLARE(0x2901)) == 0) {
      int rc = os_mbuf_append(ctxt->om, &maxpower_name, sizeof(maxpower_name));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    if (ble_uuid_cmp(ctxt->dsc->uuid, BLE_UUID16_DECLARE(0x2904)) == 0) {
      int rc = os_mbuf_append(ctxt->om, &utf8_string_format, sizeof(utf8_string_format));
      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
  }
  return 0;
}

static const struct ble_gatt_svc_def service1_items[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID128_DECLARE(0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0, 0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0),
        .characteristics = (struct ble_gatt_chr_def[])
            {{
                 .uuid = BLE_UUID16_DECLARE(0x2A24),
                 .access_cb = service1_access,
                 .flags = BLE_GATT_CHR_F_READ
             },
             {
                 .uuid = BLE_UUID16_DECLARE(0x2A29),
                 .access_cb = service1_access,
                 .flags = BLE_GATT_CHR_F_READ
             },
             {
                 .uuid = BLE_UUID128_DECLARE(0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6, 0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6),
                 .access_cb = service1_access,
                 .flags = BLE_GATT_CHR_F_READ,
                 .descriptors = (struct ble_gatt_dsc_def[])
                     {{
                          .uuid = BLE_UUID16_DECLARE(0x2901),
                          .att_flags = BLE_ATT_F_READ,
                          .access_cb = service1_access,
                      },
                      {
                          .uuid = BLE_UUID16_DECLARE(0x2904),
                          .att_flags = BLE_ATT_F_READ,
                          .access_cb = service1_access,
                      },
                      {
                          0
                      }
                     },
             },
             {
                 0
             }
            },
    },
    {
        0, /* No more services. */
    },
};

void service1_init(void) {
  ESP_ERROR_CHECK(ble_gatts_count_cfg(service1_items));
  ESP_ERROR_CHECK(ble_gatts_add_svcs(service1_items));
}