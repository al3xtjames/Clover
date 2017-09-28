/** @file
  Helpers for device property injection.

  Copyright (C) 2016-2017 Alex James (TheRacerMaster). All rights reserved.<BR>

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

#include <Platform.h>

#include "DeviceProperty.h"

// TODO: Consolidate LogUintProperty functions

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
STATIC
VOID
LogInjectionStatus (
  IN CHAR16               *Name,
  IN DEVICE_PROPERTY_TYPE Type,
  IN VOID                 *Value,
  IN UINTN                Size,
  IN EFI_STATUS           Status
  )
{
  CHAR8 PropertyValueString[Size + 10];

  switch (Type) {
    case DevicePropertyUint8:
    {
      LogUint8Property (PropertyValueString, (UINT8 *)Value);
      break;
    }

    case DevicePropertyUint16:
    {
      LogUint16Property (PropertyValueString, (UINT16 *)Value);
      break;
    }

    case DevicePropertyUint32:
    {
      LogUint32Property (PropertyValueString, (UINT32 *)Value);
      break;
    }

    case DevicePropertyUint64:
    {
      LogUint64Property (PropertyValueString, (UINT64 *)Value);
      break;
    }

    case DevicePropertyChar8:
    {
      LogChar8Property (PropertyValueString, (CHAR8 *)Value);
      break;
    }
  }

  MsgLog (
    " - Injected %s = %a, Status = %r\n",
    Name,
    PropertyValueString,
    Status
  );
}

/** Injects a single device property.

  @param[in] DevicePath  The device path.
  @param[in] Name        The Unicode name of the device property.
  @param[in] Type        The type of the device property.
  @param[in] Value       The value of the device property.
  @param[in] Size        The size of the device property.

  @return                The status of setting the device property is
                         returned.
  @retval EFI_NOT_FOUND  The EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL
                         was not found.
  @retval EFI_SUCCESS    The operation completed successfully.
**/
EFI_STATUS
InjectDeviceProperty (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN CHAR16                   *Name,
  IN DEVICE_PROPERTY_TYPE     Type,
  IN VOID                     *Value,
  IN UINTN                    Size
  )
{
  EFI_STATUS Status;

  if (gEfiDppDbProtocol == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = gEfiDppDbProtocol->SetProperty (
                                gEfiDppDbProtocol,
                                DevicePath,
                                Name,
                                Value,
                                Size
                                );

  LogInjectionStatus (
    Name,
    Type,
    Value,
    Size,
    Status
    );

  return Status;
}
