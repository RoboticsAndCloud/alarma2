#ifndef AW_BT_RFCOMM_H
#define AW_BT_RFCOMM_H


/**
 * Returns NULL if input could be parsed.
 */
typedef uint8_t* (*bt_rfcomm_parser)(uint8_t* data, uint16_t len, uint16_t* out_len);


void bt_rfcomm_init(const char* dev_name, bt_rfcomm_parser);


#endif
