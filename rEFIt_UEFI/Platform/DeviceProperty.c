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
GetUINT8Str (
  OUT CHAR8 *String,
  IN  UINT8 *Integer
  )
{
  AsciiSPrint (String, 5, "<%02X>", *Integer);
}

STATIC
VOID
GetUINT16Str (
  OUT CHAR8  *String,
  IN  UINT16 *Integer
  )
{
  AsciiSPrint (
    String,
    8,
    "<%02X %02X>",
    *Integer & 0xFF,
    (*Integer >> (8)) & 0xFF
  );
}

STATIC
VOID
GetUINT32Str (
  OUT CHAR8  *String,
  IN  UINT32 *Integer
  )
{
  AsciiSPrint (
    String,
    14,
    "<%02X %02X %02X %02X>",
    *Integer & 0xFF,
    (*Integer >> (8))  & 0xFF,
    (*Integer >> (16)) & 0xFF,
    (*Integer >> (24)) & 0xFF
  );
}

STATIC
VOID
GetUINT64Str (
  OUT CHAR8  *String,
  IN  UINT64 *Integer
  )
{
  AsciiSPrint (
    String,
    26,
    "<%02llX %02llX %02llX %02llX %02llX %02llX %02llX %02llX>",
    *Integer & 0xFF,
    (*Integer >> (8))  & 0xFF,
    (*Integer >> (16)) & 0xFF,
    (*Integer >> (24)) & 0xFF,
    (*Integer >> (32)) & 0xFF,
    (*Integer >> (40)) & 0xFF,
    (*Integer >> (48)) & 0xFF,
    (*Integer >> (56)) & 0xFF
  );
}

STATIC
VOID
GetCHAR8Str (
  OUT CHAR8 *DestString,
  IN  CHAR8 *SrcString
  )
{
  if (SrcString == NULL) {
    AsciiSPrint (DestString, 14, "<null string>");
  } else {
    AsciiSPrint (DestString, AsciiStrLen (SrcString) + 5, "<\"%a\">", SrcString);
  }
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
  CHAR8 PropertyValueString[Property->Size];

  switch (Property->Type) {
    case DEVICE_PROPERTY_UINT8:
    {
      GetUINT8Str (PropertyValueString, (UINT8 *)Property->Value);
      break;
    }

    case DEVICE_PROPERTY_UINT16:
    {
      GetUINT16Str (PropertyValueString, (UINT16 *)Property->Value);
      break;
    }

    case DEVICE_PROPERTY_UINT32:
    {
      GetUINT32Str (PropertyValueString, (UINT32 *)Property->Value);
      break;
    }

    case DEVICE_PROPERTY_UINT64:
    {
      GetUINT64Str (PropertyValueString, (UINT64 *)Property->Value);
      break;
    }

    case DEVICE_PROPERTY_CHAR8:
    {
      GetCHAR8Str (PropertyValueString, (CHAR8 *)Property->Value);
      break;
    }
  }

  MsgLog (
    " - Injected %s = %a, Status = %r\n",
    Property->Name,
    PropertyValueString,
    Status
  );

  FreePool (PropertyValueString);
}
