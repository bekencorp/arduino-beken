/*
  This file is part of the ArduinoBLE library.
  Copyright (c) 2018 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

// #include <stdint.h>

#ifndef _BLE_PROPERTY_H_
#define _BLE_PROPERTY_H_

enum BLEProperty {
  BLEBroadcast            = 0x01,
  BLERead                 = 0x02,
  BLEWriteWithoutResponse = 0x04,
  BLEWrite                = 0x08,
  BLENotify               = 0x10,
  BLEIndicate             = 0x20,
  BLEAuthSignedWrite      = 1 << 6,
  BLEExtProp              = 1 << 7,
};

enum BLEPermission {
  BLEEncryption         = 1 << 9,
  BLEAuthentication     = 1 << 10,
  BLEAuthorization      = 1 << 11,
  // BLEWriteEncryption        = 1 << 11,
  // BLEWriteAuthentication    = 1 << 12,
  // BLEWriteAuthorization     = 1 << 13,
};

#define    ESP_GATT_CHAR_PROP_BIT_BROADCAST    (1 << 0)
#define    ESP_GATT_CHAR_PROP_BIT_READ         (1 << 1)
#define    ESP_GATT_CHAR_PROP_BIT_WRITE_NR     (1 << 2)
#define    ESP_GATT_CHAR_PROP_BIT_WRITE        (1 << 3)
#define    ESP_GATT_CHAR_PROP_BIT_NOTIFY       (1 << 4)
#define    ESP_GATT_CHAR_PROP_BIT_INDICATE     (1 << 5)
#define    ESP_GATT_CHAR_PROP_BIT_AUTH         (1 << 6)
#define    ESP_GATT_CHAR_PROP_BIT_EXT_PROP     (1 << 7)

#define    ESP_GATT_PERM_READ                  (1 << 0)
#define    ESP_GATT_PERM_READ_ENCRYPTED        (1 << 1)
#define    ESP_GATT_PERM_READ_ENC_MITM         (1 << 2)
#define    ESP_GATT_PERM_WRITE                 (1 << 4)
#define    ESP_GATT_PERM_WRITE_ENCRYPTED       (1 << 5)
#define    ESP_GATT_PERM_WRITE_ENC_MITM        (1 << 6)
#define    ESP_GATT_PERM_WRITE_SIGNED          (1 << 7)
#define    ESP_GATT_PERM_WRITE_SIGNED_MITM     (1 << 8)
#define    ESP_GATT_PERM_READ_AUTHORIZATION    (1 << 9)
#define    ESP_GATT_PERM_WRITE_AUTHORIZATION   (1 << 10)

enum BLE_GATT_PERM_ {
  BLE_GATT_READ       = 1 << 0,
  READ_ENCRYPTED      = 1 << 1,
  READ_ENC_MITM       = 1 << 2,
  BLE_GATT_WRITE      = 1 << 4,
  WRITE_ENCRYPTED     = 1 << 5,
  WRITE_ENC_MITM      = 1 << 6,
  WRITE_SIGNED        = 1 << 7,
  WRITE_SIGNED_MITM   = 1 << 8,
  READ_AUTHORIZATION  = 1 << 9,
  WRITE_AUTHORIZATION = 1 << 10,
};


#endif
