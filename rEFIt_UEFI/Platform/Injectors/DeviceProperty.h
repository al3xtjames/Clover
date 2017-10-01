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

#ifndef DEVICE_PROPERTY_H_
#define DEVICE_PROPERTY_H_

typedef enum {
  DevicePropertyUint,
  DevicePropertyChar8
} DEVICE_PROPERTY_TYPE;

enum {
  DevicePropertyWidthUint8  = 1,
  DevicePropertyWidthUint16 = 2,
  DevicePropertyWidthUint32 = 4,
  DevicePropertyWidthUint64 = 8
};

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
  );

#endif // DEVICE_PROPERTY_H_
