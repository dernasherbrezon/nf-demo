#ifndef NF_DEMO_SERVICE2_H
#define NF_DEMO_SERVICE2_H

#include <stdint.h>

void service2_init(void);

extern uint16_t service2_la_val_handle;
//
//ble_item service2_items[SERVICE2_TOTAL] = {
//    [SERVICE2] = {
//        .type = TYPE_PRIMARY_SERVICE,
//        .id = {
//            .is_primary = true,
//            .id = {
//                .inst_id = 0,
//                .uuid = {
//                    .len = ESP_UUID_LEN_128,
//                    .uuid.uuid128 = {0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc, 0xf7, 0x48, 0x3c, 0x67, 0x97, 0x5b, 0x47, 0xfc}
//                }
//            }
//        }
//    },
//    [SERVICE2_MODEL] = {
//        .type = TYPE_CHARACTERISTIC,
//        .uuid = {
//            .len = ESP_UUID_LEN_16,
//            .uuid.uuid16 = 0x2A24
//        },
//        .property = ESP_GATT_CHAR_PROP_BIT_READ,
//        .value = {
//            .attr_value = (uint8_t *) service2_model_name,
//            .attr_len = strlen(service2_model_name),
//            .attr_max_len = strlen(service2_model_name),
//        },
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    }, [SERVICE2_MANUFACTURER] = {
//        .type = TYPE_CHARACTERISTIC,
//        .uuid = {
//            .len = ESP_UUID_LEN_16,
//            .uuid.uuid16 = 0x2A29
//        },
//        .property = ESP_GATT_CHAR_PROP_BIT_READ,
//        .value = {
//            .attr_value = (uint8_t *) service2_manuf_name,
//            .attr_len = strlen(service2_manuf_name),
//            .attr_max_len = strlen(service2_manuf_name)
//        },
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    }, [SERVICE2_LA] = {
//        .type = TYPE_CHARACTERISTIC,
//        .uuid = {
//            .len = ESP_UUID_LEN_128,
//            .uuid.uuid128 = {0x4f, 0x90, 0x7, 0x4e, 0x60, 0xde, 0x40, 0x11, 0x9f, 0xc0, 0x82, 0x3a, 0x47, 0x75, 0x58, 0x16}
//        },
//        .property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
//        .read_callback = service2_la_callback,
//        .value = {
//            .attr_value = NULL,
//            .attr_max_len = 0,
//            .attr_len = 0
//        },
//        .control = {
//            .auto_rsp = ESP_GATT_RSP_BY_APP
//        }
//    }, [SERVICE2_LA_USER_DESCR] = {
//        .type = TYPE_DESCRIPTION,
//        .uuid = {
//            .len = ESP_UUID_LEN_16,
//            .uuid.uuid16 = 0x2901
//        },
//        .value = {
//            .attr_len = strlen(service2_la_name),
//            .attr_max_len = strlen(service2_la_name),
//            .attr_value = (uint8_t *) service2_la_name
//        },
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    },
//    [SERVICE2_LA_TYPE] = {
//        .type = TYPE_DESCRIPTION,
//        .uuid = {
//            .len = ESP_UUID_LEN_16,
//            .uuid.uuid16 = 0x2904
//        },
//        .value = {
//            .attr_len = sizeof(float_format),
//            .attr_max_len = sizeof(float_format),
//            .attr_value = (uint8_t *) &float_format
//        },
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    },
//    [SERVICE2_LA_CCD] = {
//        .type = TYPE_DESCRIPTION,
//        .uuid = {
//            .len = ESP_UUID_LEN_16,
//            .uuid.uuid16 = 0x2902
//        },
//        .value = {
//            .attr_len = sizeof(service2_la_ccd),
//            .attr_max_len = sizeof(service2_la_ccd),
//            .attr_value = service2_la_ccd
//        },
//        .write_callback = service2_la_write_callback,
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    },
//    [SERVICE2_LA_GRAPH] = {
//        .type = TYPE_DESCRIPTION,
//        .uuid = {
//            .len = ESP_UUID_LEN_128,
//            .uuid.uuid128 = {0xee, 0x7f, 0xe4, 0x50, 0x96, 0x73, 0x40, 0x65, 0xee, 0x7f, 0xe4, 0x50, 0x96, 0x73, 0x40, 0x65}
//        },
//        .value = {
//            .attr_len = sizeof(service2_la_ccd),
//            .attr_max_len = sizeof(service2_la_ccd),
//            .attr_value = service2_la_ccd
//        },
//        .write_callback = service2_la_graph_write_callback,
//        .control = {
//            .auto_rsp = ESP_GATT_AUTO_RSP
//        }
//    }
//};
//
//service service2 = {
//    .item_count = SERVICE2_TOTAL,
//    .current_item = 0,
//    .items = service2_items,
//};

#endif //NF_DEMO_SERVICE2_H
