#include "./crc.h"

uint16_t crc16(uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    for (size_t i = 0; i < length; i++) {
        crc ^= (data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

uint16_t crc12(uint8_t *data, size_t length) {
    uint16_t crc = 0xFFF;  // Initial value
    for (size_t i = 0; i < length; i++) {
        crc ^= (data[i] << 4);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x800) {
                crc = (crc << 1) ^ CRC12_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc & 0xFFF;  // Mask to 12 bits
}

uint16_t crc_ccitt(uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    for (size_t i = 0; i < length; i++) {
        crc ^= (data[i] << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_CCITT_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

uint16_t calculate_crc(char *data, size_t length, int crc_type) {
    if (crc_type == CRC12_POLY) {
        return crc12((uint8_t *)data, length);
    } else if (crc_type == CRC16_POLY) {
        return crc16((uint8_t *)data, length);
    } else if (crc_type == CRC_CCITT_POLY) {
        return crc_ccitt((uint8_t *)data, length);
    }
    return 0;  // Default to CRC-16 if no valid type is specified
}

void append_crc(char *data, size_t length, int crc_type) {
    uint16_t crc = calculate_crc(data, length, crc_type);
    // Append the CRC value to the data (stored in the last two bytes as hex)
    sprintf(data + length, "%04X", crc);  // Append CRC (CRC-16 as an example)
}

