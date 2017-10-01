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

#include <Injectors/DeviceProperty.h>

STATIC
VOID
LogUintProperty (
  IN UINT8 *PropertyValue,
  IN UINTN Size
  )
{
  INTN  Index;
  UINTN NumberOfSpaces = Size - 1;

  // Integer properties are shown in the IORegistry like this:
  // "AAPL,ig-platform-id" = <00 00 1b 59>
  MsgLog ("<");
  for (Index = 0; Index < Size; ++Index) {
    MsgLog ("%02x", *(PropertyValue + Index));
    if (Index < NumberOfSpaces) {
      MsgLog (" ");
    }
  }

  MsgLog (">");
}

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
  if (Value == NULL || Size == 0) {
    return;
  }

  MsgLog (" - Injected %s = ", Name);

  switch (Type) {
    case DevicePropertyUint:
      LogUintProperty ((UINT8 *)Value, Size);
      break;

    case DevicePropertyChar8:
      MsgLog ("<\"%a\">", (CHAR8 *)Value);
      break;
  }

  MsgLog (", Status = %r\n", Status);
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

  if (DevicePath == NULL || Name == NULL || Value == NULL) {
    return EFI_INVALID_PARAMETER;
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
