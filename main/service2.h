#ifndef NF_DEMO_SERVICE2_H
#define NF_DEMO_SERVICE2_H

#include "common.h"

enum {
  SERVICE2,
  SERVICE2_MODEL,
  SERVICE2_MANUFACTURER,
  SERVICE2_LA,
  SERVICE2_LA_USER_DESCR,
  SERVICE2_LA_TYPE,
  SERVICE2_LA_CCD,
  SERVICE2_TOTAL
};

static const char service2_model_name[] = "r2cloud 2.0";
static const char service2_manuf_name[] = "dernasherbrezon";
static const char service2_la_name[] = "Load Average";
uint8_t service2_la_ccd[2] = {0x00, 0x00};

extern void service2_la_callback(esp_ble_gatts_cb_param_t *param);

extern void service2_la_write_callback(void *item, esp_ble_gatts_cb_param_t *param);

ble_item service2_items[SERVICE2_TOTAL] = {
    [SERVICE2] = {
        .type = TYPE_PRIMARY_SERVICE,
        .id = {
            .is_primary = true,
            .id = {
                .inst_id = 0,
                .uuid = {
                    .len = ESP_UUID_LEN_128,
                    .uuid.uuid128 = {0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc, 0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc}
                }
            }
        }
    },
    [SERVICE2_MODEL] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2A24
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ,
        .value = {
            .attr_value = (uint8_t *) service2_model_name,
            .attr_len = strlen(service2_model_name),
            .attr_max_len = strlen(service2_model_name),
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }, [SERVICE2_MANUFACTURER] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2A29
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ,
        .value = {
            .attr_value = (uint8_t *) service2_manuf_name,
            .attr_len = strlen(service2_manuf_name),
            .attr_max_len = strlen(service2_manuf_name)
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }, [SERVICE2_LA] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid.uuid128 = {0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6, 0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6}
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
        .read_callback = service2_la_callback,
        .value = {
            .attr_value = NULL,
            .attr_max_len = 0,
            .attr_len = 0
        },
        .control = {
            .auto_rsp = ESP_GATT_RSP_BY_APP
        }
    }, [SERVICE2_LA_USER_DESCR] = {
        .type = TYPE_DESCRIPTION,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2901
        },
        .value = {
            .attr_len = strlen(service2_la_name),
            .attr_max_len = strlen(service2_la_name),
            .attr_value = (uint8_t *) service2_la_name
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    },
    [SERVICE2_LA_TYPE] = {
        .type = TYPE_DESCRIPTION,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2904
        },
        .value = {
            .attr_len = sizeof(float_format),
            .attr_max_len = sizeof(float_format),
            .attr_value = (uint8_t *) &float_format
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    },
    [SERVICE2_LA_CCD] = {
        .type = TYPE_DESCRIPTION,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2902
        },
        .value = {
            .attr_len = sizeof(service2_la_ccd),
            .attr_max_len = sizeof(service2_la_ccd),
            .attr_value = service2_la_ccd
        },
        .write_callback = service2_la_write_callback,
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }
};

service service2 = {
    .item_count = SERVICE2_TOTAL,
    .current_item = 0,
    .items = service2_items,
};

#endif //NF_DEMO_SERVICE2_H
