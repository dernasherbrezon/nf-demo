#ifndef NF_DEMO_SERVICE1_H
#define NF_DEMO_SERVICE1_H

#include "common.h"

enum {
  SERVICE1,
  SERVICE1_MODEL,
  SERVICE1_MANUFACTURER,
  SERVICE1_MAXPOWER,
  SERVICE1_MAXPOWER_USER_DESCR,
  SERVICE1_MAXPOWER_TYPE,
  SERVICE1_TOTAL
};

static const char service1_model_name[] = "Arrow II 146/437-10BP";
static const char service1_manuf_name[] = "Arrow Antenna";
static const char maxpower[] = "10W";
static const char maxpower_name[] = "Maximum power";

ble_item service1_items[SERVICE1_TOTAL] = {
    [SERVICE1] = {
        .type = TYPE_PRIMARY_SERVICE,
        .id = {
            .is_primary = true,
            .id = {
                .inst_id = 0,
                .uuid = {
                    .len = ESP_UUID_LEN_128,
                    .uuid.uuid128 = {0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0, 0x92, 0xcc, 0xc7, 0x45, 0xf0, 0x72, 0x49, 0xd0}
                }
            }
        }
    },
    [SERVICE1_MODEL] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2A24
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ,
        .value = {
            .attr_value = (uint8_t *) service1_model_name,
            .attr_len = strlen(service1_model_name),
            .attr_max_len = strlen(service1_model_name),
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }, [SERVICE1_MANUFACTURER] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2A29
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ,
        .value = {
            .attr_value = (uint8_t *) service1_manuf_name,
            .attr_len = strlen(service1_manuf_name),
            .attr_max_len = strlen(service1_manuf_name)
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }, [SERVICE1_MAXPOWER] = {
        .type = TYPE_CHARACTERISTIC,
        .uuid = {
            .len = ESP_UUID_LEN_128,
            .uuid.uuid128 = {0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6, 0xe0, 0xa, 0x30, 0x1e, 0x6c, 0xe9, 0x41, 0xa6}
        },
        .property = ESP_GATT_CHAR_PROP_BIT_READ,
        .value = {
            .attr_value = (uint8_t *) maxpower,
            .attr_len = strlen(maxpower),
            .attr_max_len = 400
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }, [SERVICE1_MAXPOWER_USER_DESCR] = {
        .type = TYPE_DESCRIPTION,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2901
        },
        .value = {
            .attr_len = strlen(maxpower_name),
            .attr_max_len = strlen(maxpower_name),
            .attr_value = (uint8_t *) maxpower_name
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    },
    [SERVICE1_MAXPOWER_TYPE] = {
        .type = TYPE_DESCRIPTION,
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid.uuid16 = 0x2904
        },
        .value = {
            .attr_len = sizeof(utf8_string_format),
            .attr_max_len = sizeof(utf8_string_format),
            .attr_value = (uint8_t *) &utf8_string_format
        },
        .control = {
            .auto_rsp = ESP_GATT_AUTO_RSP
        }
    }
};

service service1 = {
    .item_count = SERVICE1_TOTAL,
    .current_item = 0,
    .items = service1_items,
};

#endif //NF_DEMO_SERVICE1_H
