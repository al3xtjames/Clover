/** @file
  Copyright (C) 2005 - 2016, Apple Inc.  All rights reserved.
  Portions Copyright (C) 2014 - 2016, CupertinoNet.  All rights reserved.<BR>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
**/
#ifndef APPLE_SMC_H_
#define APPLE_SMC_H_

#define SMC_MAKE_IDENTIFIER(A, B, C, D)  \
  (((A) << 24) | ((B) << 16) | ((C) << 8) | (D))

// PMIO

#define SMC_PORT_BASE            0x0300
#define SMC_PORT_LENGTH          0x0020

#define SMC_PORT_OFFSET_DATA     0x00
#define SMC_PORT_OFFSET_COMMAND  0x04
#define SMC_PORT_OFFSET_STATUS   SMC_PORT_OFFSET_COMMAND
#define SMC_PORT_OFFSET_RESULT   0x1E
#define	SMC_PORT_OFFSET_DEBUG    0x1F

// MMIO

#define	SMC_MMIO_BASE_ADDRESS  0xFEF00000
#define	SMC_MMIO_LENGTH        0x00010000

#define SMC_MMIO_DATA_VARIABLE  0x00
#define SMC_MMIO_DATA_FIXED     0x78

// MMIO offsets

#define SMC_MMIO_OFFSET_KEY             0x00
#define SMC_MMIO_OFFSET_KEY_TYPE        SMC_MMIO_OFFSET_KEY
#define SMC_MMIO_OFFSET_SMC_MODE        SMC_MMIO_OFFSET_KEY
#define SMC_MMIO_OFFSET_DATA_SIZE       0x05
#define SMC_MMIO_OFFSET_KEY_ATTRIBUTES  0x06
#define SMC_MMIO_OFFSET_COMMAND         0x07
#define SMC_MMIO_OFFSET_RESULT          SMC_MMIO_OFFSET_COMMAND

// Read addresses

#define SMC_MMIO_READ_KEY  \
  (SMC_MMIO_DATA_VARIABLE + SMC_MMIO_OFFSET_KEY)

#define SMC_MMIO_READ_KEY_TYPE  \
  (SMC_MMIO_DATA_VARIABLE + SMC_MMIO_OFFSET_KEY_TYPE)

#define SMC_MMIO_READ_DATA_SIZE  \
  (SMC_MMIO_DATA_VARIABLE + SMC_MMIO_OFFSET_DATA_SIZE)

#define SMC_MMIO_READ_KEY_ATTRIBUTES  \
  (SMC_MMIO_DATA_VARIABLE + SMC_MMIO_OFFSET_KEY_ATTRIBUTES)

#define SMC_MMIO_READ_RESULT  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_RESULT)

#define SMC_MMIO_READ_UNKNOWN1    0x4004
#define SMC_MMIO_READ_KEY_STATUS  0x4005

// Write addresses

#define SMC_MMIO_WRITE_MODE  \
  (SMC_MMIO_DATA_VARIABLE + SMC_MMIO_OFFSET_SMC_MODE)

#define SMC_MMIO_WRITE_KEY  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_KEY)

#define SMC_MMIO_WRITE_INDEX  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_KEY_TYPE)

#define SMC_MMIO_WRITE_DATA_SIZE  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_DATA_SIZE)

#define SMC_MMIO_WRITE_KEY_ATTRIBUTES  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_KEY_ATTRIBUTES)

#define SMC_MMIO_WRITE_COMMAND  \
  (SMC_MMIO_DATA_FIXED + SMC_MMIO_OFFSET_COMMAND)

typedef UINT32 SMC_ADDRESS;

// Modes

#define SMC_MODE_APPCODE  'A'
#define SMC_MODE_UPDATE   'U'
#define SMC_MODE_BASE     'B'

// SMC_MODE
typedef CHAR8 *SMC_MODE;

enum {
  SmcResetModeMaster  = 0,
  SmcResetModeAppCode = 1,
  SmcResetModeUpdate  = 2,
  SmcResetModeBase    = 3
};

typedef UINT8 SMC_RESET_MODE;

enum {
  SmcFlashTypeAppCode = 1,
  SmcFlashTypeBase    = 2,
  SmcFlashTypeUpdate  = 3,
  SmcFlashTypeEpm     = 4
};

typedef UINT8 SMC_FLASH_TYPE;

enum {
  SmcFlashModeAppCode = SmcResetModeMaster,
  SmcFlashModeUpdate  = SmcResetModeBase,
  SmcFlashModeBase    = SmcResetModeUpdate,
  SmcFlashModeEpm     = SmcResetModeMaster
};

typedef UINT8 SMC_FLASH_MODE;

// Commands

enum {
  SmcCmdReadValue            = 0x10,
  SmcCmdWriteValue           = 0x11,
  SmcCmdGetKeyFromIndex      = 0x12,
  SmcCmdGetKeyInfo           = 0x13,
  SmcCmdReset                = 0x14,
  SmcCmdAttributedWriteValue = 0x15,
  SmcCmdAttributedReadValue  = 0x16,
  SmcCmdUnknown1             = 0x77,
  SmcCmdFlashWrite           = 0xF1,
  SmcCmdFlashAuth            = 0xF2,
  SmcCmdFlashType            = 0xF4,
  SmcCmdFlashWriteMoreData   = 0xF5, // write more data than available at once
  SmcCmdFlashAuthMoreData    = 0xF6  // auth more data than available at once
};

typedef UINT8 SMC_COMMAND;

// Reports

#define SMC_STATUS_AWAITING_DATA  BIT0  ///< Ready to read data.
#define SMC_STATUS_IB_CLOSED      BIT1  /// A write is pending.
#define SMC_STATUS_BUSY           BIT2  ///< Busy processing a command.
#define SMC_STATUS_GOT_COMMAND    BIT3  ///< The last input was a command.
#define SMC_STATUS_UKN_0x16       BIT4
#define SMC_STATUS_KEY_DONE       BIT5
#define SMC_STATUS_UKN_0x40       BIT6  // error
#define SMC_STATUS_UKN_0x80       BIT7  // error

// SMC_STATUS
typedef UINT8 SMC_STATUS;

enum {
  SmcSuccess               = 0,
  SmcError                 = 1,

  SmcCommCollision         = 128,
  SmcSpuriousData          = 129,
  SmcBadCommand            = 130,
  SmcBadParameter          = 131,
  SmcNotFound              = 132,
  SmcNotReadable           = 133,
  SmcNotWritable           = 134,
  SmcKeySizeMismatch       = 135,
  SmcFramingError          = 136,
  SmcBadArgumentError      = 137,

  SmcTimeoutError          = 183,
  SmcKeyIndexRangeError    = 184,

  SmcBadFunctionParameter  = 192,
  SmcEventBufferWrongOrder = 193,
  SmcEventBufferReadError  = 194,
  SmcDeviceAccessError     = 195,
  SmcUnsupportedFeature    = 196,
  SmcSmbAccessError        = 197,

  SmcInvalidSize           = 206
};

#define SMC_ERROR(a) (((UINTN)(a)) > 0)

#define EFI_STATUS_FROM_SMC_RESULT(x)  \
  ((((UINTN)(x)) == SmcSuccess) ? EFI_SUCCESS : EFIERR ((UINTN)(x)))

#define EFI_SMC_SUCCESS                   SmcSuccess
#define EFI_SMC_ERROR                     EFIERR (SmcError)

#define EFI_SMC_COMM_COLLISION            EFIERR (SmcCommCollision)
#define EFI_SMC_SPURIOUS_DATA             EFIERR (SmcSpuriousData)
#define EFI_SMC_BAD_COMMAND               EFIERR (SmcBadCommand)
#define EFI_SMC_BAD_PARAMETER             EFIERR (SmcBadParameter)
#define EFI_SMC_NOT_FOUND                 EFIERR (SmcNotFound)
#define EFI_SMC_NOT_READABLE              EFIERR (SmcNotReadable)
#define EFI_SMC_NOT_WRITABLE              EFIERR (SmcNotWritable)
#define EFI_SMC_KEY_MISMATCH              EFIERR (SmcKeySizeMismatch)
#define EFI_SMC_FRAMING_ERROR             EFIERR (SmcFramingError)
#define EFI_SMC_BAD_ARGUMENT_ERROR        EFIERR (SmcBadArgumentError)

#define EFI_SMC_TIMEOUT_ERROR             EFIERR (SmcTimeoutError)
#define EFI_SMC_KEY_INDEX_RANGE_ERROR     EFIERR (SmcKeyIndexRangeError)

#define EFI_SMC_BAD_FUNCTION_PARAMETER    EFIERR (SmcBadFunctionParameter)
#define EFI_SMC_EVENT_BUFFER_WRONG_ORDER  EFIERR (SmcEventBufferWrongOrder)
#define EFI_SMC_EVENT_BUFFER_READ_ERROR   EFIERR (SmcEventBufferReadError)
#define EFI_SMC_DEVICE_ACCESS_ERROR       EFIERR (SmcDeviceAccessError)
#define EFI_SMC_UNSUPPORTED_FEATURE       EFIERR (SmcUnsupportedFeature)
#define EFI_SMB_ACCESS_ERROR              EFIERR (SmcSmbAccessError)

#define EFI_SMC_INVALID_SIZE              EFIERR (SmcInvalidSize)

// SMC_RESULT
typedef UINT8 SMC_RESULT;

// Key Types

#define SMC_MAKE_KEY_TYPE(A, B, C, D) SMC_MAKE_IDENTIFIER ((A), (B), (C), (D))

enum {
  SmcKeyTypeCh8    = SMC_MAKE_KEY_TYPE ('c', 'h', '8', '*'),
  SmcKeyTypeFlag   = SMC_MAKE_KEY_TYPE ('f', 'l', 'a', 'g'),
  SmcKeyTypeFp1f   = SMC_MAKE_KEY_TYPE ('f', 'p', '1', 'f'),
  SmcKeyTypeFp4c   = SMC_MAKE_KEY_TYPE ('f', 'p', '4', 'c'),
  SmcKeyTypeFp5b   = SMC_MAKE_KEY_TYPE ('f', 'p', '5', 'b'),
  SmcKeyTypeFp6a5b = SMC_MAKE_KEY_TYPE ('f', 'p', '6', 'a'),
  SmcKeyTypeFp79   = SMC_MAKE_KEY_TYPE ('f', 'p', '7', '9'),
  SmcKeyTypeFpa6   = SMC_MAKE_KEY_TYPE ('f', 'p', 'a', '6'),
  SmcKeyTypeFpc4   = SMC_MAKE_KEY_TYPE ('f', 'p', 'c', '4'),
  SmcKeyTypeFpe2   = SMC_MAKE_KEY_TYPE ('f', 'p', 'e', '2'),
  SmcKeyTypeSint8  = SMC_MAKE_KEY_TYPE ('s', 'i', '8', ' '),
  SmcKeyTypeSint16 = SMC_MAKE_KEY_TYPE ('s', 'i', '1', '6'),
  SmcKeyTypeSp1e   = SMC_MAKE_KEY_TYPE ('s', 'p', '1', 'e'),
  SmcKeyTypeSp3c   = SMC_MAKE_KEY_TYPE ('s', 'p', '3', 'c'),
  SmcKeyTypeSp4b   = SMC_MAKE_KEY_TYPE ('s', 'p', '4', 'b'),
  SmcKeyTypeSp5a   = SMC_MAKE_KEY_TYPE ('s', 'p', '5', 'a'),
  SmcKeyTypeSp69   = SMC_MAKE_KEY_TYPE ('s', 'p', '6', '9'),
  SmcKeyTypeSp78   = SMC_MAKE_KEY_TYPE ('s', 'p', '7', '8'),
  SmcKeyTypeSp87   = SMC_MAKE_KEY_TYPE ('s', 'p', '8', '7'),
  SmcKeyTypeSp96   = SMC_MAKE_KEY_TYPE ('s', 'p', '9', '6'),
  SmcKeyTypeSpb4   = SMC_MAKE_KEY_TYPE ('s', 'p', 'b', '4'),
  SmcKeyTypeSpf0   = SMC_MAKE_KEY_TYPE ('s', 'p', 'f', '0'),
  SmcKeyTypeUint8  = SMC_MAKE_KEY_TYPE ('u', 'i', '8', ' '),
  SmcKeyTypeUint16 = SMC_MAKE_KEY_TYPE ('u', 'i', '1', '6'),
  SmcKeyTypeUint32 = SMC_MAKE_KEY_TYPE ('u', 'i', '3', '2'),
  SmcKeyTypeLim    = SMC_MAKE_KEY_TYPE ('{', 'l', 'i', 'm'),
  SmcKeyTypePwm    = SMC_MAKE_KEY_TYPE ('{', 'p', 'w', 'm'),
  SmcKeyTypeAla    = SMC_MAKE_KEY_TYPE ('{', 'a', 'l', 'a'),
  SmcKeyTypeAlc    = SMC_MAKE_KEY_TYPE ('{', 'a', 'l', 'c'),
  SmcKeyTypeAli    = SMC_MAKE_KEY_TYPE ('{', 'a', 'l', 'i'),
  SmcKeyTypeAlr    = SMC_MAKE_KEY_TYPE ('{', 'a', 'l', 'r'),
  SmcKeyTypeAlt    = SMC_MAKE_KEY_TYPE ('{', 'a', 'l', 't')
};

// SMC_KEY_TYPE
typedef UINT32 SMC_KEY_TYPE;

// Key Attributes

#define	SMC_KEY_ATTRIBUTE_PRIVATE   BIT0
#define	SMC_KEY_ATTRIBUTE_UKN_0x02  BIT1
#define	SMC_KEY_ATTRIBUTE_UKN_0x04  BIT2
#define	SMC_KEY_ATTRIBUTE_CONST     BIT3
#define	SMC_KEY_ATTRIBUTE_FUNCTION  BIT4
#define	SMC_KEY_ATTRIBUTE_UKN_0x20  BIT5
#define	SMC_KEY_ATTRIBUTE_WRITE     BIT6
#define	SMC_KEY_ATTRIBUTE_READ      BIT7

// SMC_KEY_ATTRIBUTES
typedef UINT8 SMC_KEY_ATTRIBUTES;


// Data

#define SMC_MAX_DATA_SIZE  (SMC_MMIO_DATA_FIXED - SMC_MMIO_DATA_VARIABLE)

typedef UINT8 SMC_DATA;
typedef UINT8 SMC_DATA_SIZE;

// Keys

// SMC_KEY_IS_VALID_CHAR
#define SMC_KEY_IS_VALID_CHAR(x) (((x) >= 0x20) && ((x) <= 0x7E))

// SMC_MAKE_KEY
#define SMC_MAKE_KEY(A, B, C, D) SMC_MAKE_IDENTIFIER ((A), (B), (C), (D))

#define SMC_KEY_NUM      SMC_MAKE_KEY ('$', 'N', 'u', 'm')
#define SMC_KEY_ADR      SMC_MAKE_KEY ('$', 'A', 'd', 'r')
#define SMC_KEY_NO_KEYS  SMC_MAKE_KEY ('#', 'K', 'e', 'y')
#define SMC_KEY_LDKN     SMC_MAKE_KEY ('L', 'D', 'K', 'N')

typedef UINT32 SMC_KEY;
typedef UINT32 SMC_INDEX;

// Flash data

// SMC_FLASH_SIZE_MAX
#define SMC_FLASH_SIZE_MAX  0x0800

// SMC_FLASH_SIZE
typedef UINT16 SMC_FLASH_SIZE;

#endif // APPLE_SMC_H_
