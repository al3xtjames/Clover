/** @file
  Helpers for device property injection.

  Copyright (C) 2016-2017 Alex James. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution. The full text of the license may be
  found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER
  EXPRESS OR IMPLIED.
**/

#include <Platform.h>

#include "DeviceProperty.h"

STATIC
VOID
LogUint8Property (
  OUT CHAR8 *LogString,
  IN  UINT8 *PropertyValue
  )
{
  AsciiSPrint (LogString, 5, "<%02X>", *PropertyValue);
}

STATIC
VOID
LogUint16Property (
  OUT CHAR8  *LogString,
  IN  UINT16 *PropertyValue
  )
{
  AsciiSPrint (
    LogString,
    8,
    "<%02X %02X>",
    *PropertyValue & 0xFF,
    (*PropertyValue >> (8)) & 0xFF
    );
}

STATIC
VOID
LogUint32Property (
  OUT CHAR8  *LogString,
  IN  UINT32 *PropertyValue
  )
{
  AsciiSPrint (
    LogString,
    14,
    "<%02X %02X %02X %02X>",
    *PropertyValue & 0xFF,
    (*PropertyValue >> (8))  & 0xFF,
    (*PropertyValue >> (16)) & 0xFF,
    (*PropertyValue >> (24)) & 0xFF
  );
}

STATIC
VOID
LogUint64Property (
  OUT CHAR8  *LogString,
  IN  UINT64 *PropertyValue
  )
{
  AsciiSPrint (
    LogString,
    26,
    "<%02llX %02llX %02llX %02llX %02llX %02llX %02llX %02llX>",
    *PropertyValue & 0xFF,
    (*PropertyValue >> (8))  & 0xFF,
    (*PropertyValue >> (16)) & 0xFF,
    (*PropertyValue >> (24)) & 0xFF,
    (*PropertyValue >> (32)) & 0xFF,
    (*PropertyValue >> (40)) & 0xFF,
    (*PropertyValue >> (48)) & 0xFF,
    (*PropertyValue >> (56)) & 0xFF
  );
}

STATIC
VOID
LogChar8Property (
  OUT CHAR8 *LogString,
  IN  CHAR8 *PropertyValue
  )
{
  AsciiSPrint (
    LogString,
    AsciiStrLen (PropertyValue) + 5,
    "<\"%a\">",
    PropertyValue
    );
}

/** Logs a formatted string of the device property injection status.

  @param[in] Property  A pointer to the device property.
  @param[in] Status    The status of the device property injection.
**/
VOID
LogInjectionStatus (
  IN DEVICE_PROPERTY *Property,
  IN EFI_STATUS      Status
  )
{
  CHAR8 PropertyValueString[Property->Size + 10];

  switch (Property->Type) {
    case DevicePropertyUint8:
    {
      LogUint8Property (PropertyValueString, (UINT8 *)Property->Value);
      break;
    }

    case DevicePropertyUint16:
    {
      LogUint16Property (PropertyValueString, (UINT16 *)Property->Value);
      break;
    }

    case DevicePropertyUint32:
    {
      LogUint32Property (PropertyValueString, (UINT32 *)Property->Value);
      break;
    }

    case DevicePropertyUint64:
    {
      LogUint64Property (PropertyValueString, (UINT64 *)Property->Value);
      break;
    }

    case DevicePropertyChar8:
    {
      LogChar8Property (PropertyValueString, (CHAR8 *)Property->Value);
      break;
    }
  }

  MsgLog (
    " - Injected %s = %a, Status = %r\n",
    Property->Name,
    PropertyValueString,
    Status
  );
}
