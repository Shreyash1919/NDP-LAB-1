#ifndef CRC_H
#define CRC_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// Define CRC polynomial constants
#define CRC12_POLY 0x080F         // CRC-12 Polynomial (X^12 + X^11 + X^3 + X^2 + X + 1)
#define CRC16_POLY 0x8005         // CRC-16 Polynomial (X^16 + X^15 + X^2 + X + 1)
#define CRC_CCITT_POLY 0x11021    // CRC-CCITT Polynomial (X^16 + X^12 + X^5 + 1)

// Function prototypes for CRC calculation
uint16_t crc12(uint8_t *data, size_t length);
uint16_t crc16(uint8_t *data, size_t length);
uint16_t crc_ccitt(uint8_t *data, size_t length);

// Function to calculate CRC based on selected type
uint16_t calculate_crc(char *data, size_t length, int crc_type);

// Function to append CRC to data (in hex format)
void append_crc(char *data, size_t length, int crc_type);

#endif // CRC_H
