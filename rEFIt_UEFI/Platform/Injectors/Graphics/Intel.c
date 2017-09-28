/** @file
  Device property injector for Intel HD Graphics devices.

  TODO: Read the DVMT value and log it.

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

typedef struct {
  UINT16 DeviceId;
  UINT32 DefaultFakeId;
  CHAR8  *Name;
  UINT32 DefaultPlatformId;
  UINT8  Flags;
} INTEL_IGPU;

// Intel HD Graphics series flags
enum {
  /// Used for Sandy Bridge IGPUs
  FLAG_SNB_IG = 1,
  /// Used for Skylake IGPUs
  FLAG_SKL_IG = 2,
};

STATIC UINT32 mAaplGfxYTile   = 0x01000000;
STATIC UINT32 mFakeId         = 0x00000000;
STATIC UINT32 mGraphicOptions = 0x0000000C;

// Table of Intel HD Graphics devices.
STATIC CONST INTEL_IGPU mIntelGraphicsDeviceTable[] = {
  // Sandy Bridge Server GT2
  { 0x010A, 0x01168086, "Intel HD Graphics P3000", 0x00030010, FLAG_SNB_IG },
  // Sandy Bridge Desktop GT2
  { 0x0112, 0x01168086, "Intel HD Graphics 3000",  0x00030010, FLAG_SNB_IG },
  // Sandy Bridge Mobile GT2
  { 0x0116, 0x00000000, "Intel HD Graphics 3000",  0x00000000, FLAG_SNB_IG },
  // Sandy Bridge Desktop GT2
  { 0x0122, 0x01268086, "Intel HD Graphics 3000",  0x00030010, FLAG_SNB_IG },
  // Sandy Bridge Mobile GT2
  { 0x0126, 0x00000000, "Intel HD Graphics 3000",  0x00000000, FLAG_SNB_IG },
  // Ivy Bridge Desktop GT2
  { 0x0162, 0x00000000, "Intel HD Graphics 4000",  0x0166000B },
  // Ivy Bridge Mobile GT2
  { 0x0166, 0x00000000, "Intel HD Graphics 4000",  0x01660001 },
  // Ivy Bridge Server GT2
  { 0x016A, 0x01628086, "Intel HD Graphics P4000", 0x0166000B },
  // Haswell Desktop GT2
  { 0x0412, 0x00000000, "Intel HD Graphics 4600",  0x0D220003 },
  // Skylake Mobile GT2
  { 0x191B, 0x00000000, "Intel HD Graphics 530",   0x191B0000, FLAG_SKL_IG },
  // Kaby Lake Mobile GT2
  { 0x591B, 0x00000000, "Intel HD Graphics 630",   0x591B0000, FLAG_SKL_IG },

  { 0, 0, NULL, 0 }
};

STATIC
CONST
INTEL_IGPU *
GetIntelGraphicsDeviceTableEntry (
  IN UINT16 DeviceId
  )
{
  INTN Index;

  for (Index = 0; mIntelGraphicsDeviceTable[Index].DeviceId != 0; ++Index) {
    if (mIntelGraphicsDeviceTable[Index].DeviceId == DeviceId) {
      return &mIntelGraphicsDeviceTable[Index];
    }
  }

  return NULL;
}

/** Retrieves the name of an Intel HD Graphics device.

  @param[in] DeviceId  The PCI device ID of the Intel HD Graphics device.

  @return                                     The name of the Intel HD Graphics
                                              device is returned.
  @return "Unknown Intel HD Graphics Device"  The name for the Intel HD Graphics
                                              device was not found.
**/
CHAR8 *
GetIntelGraphicsName (
  IN UINT16 DeviceId
  )
{
  CONST INTEL_IGPU *TableEntry = GetIntelGraphicsDeviceTableEntry (DeviceId);

  if (TableEntry != NULL) {
    return TableEntry->Name;
  }

  return "Unknown Intel HD Graphics Device";
}

/** Retrieves the default fake ID for an Intel HD Graphics device.

  @param[in] DeviceId  The PCI device ID of the Intel HD Graphics device.

  @return    The default fake ID for the Intel HD Graphics device is returned.
  @return 0  The Intel HD Graphics device does not require a fake ID.
**/
UINT32
GetDefaultIntelFakeId (
  IN UINT16 DeviceId
  )
{
  CONST INTEL_IGPU *TableEntry = GetIntelGraphicsDeviceTableEntry (DeviceId);

  if (TableEntry != NULL) {
    return TableEntry->DefaultFakeId;
  }

  return 0;
}

/** Retrieves the default platorm ID for an Intel HD Graphics device.

  @param[in] DeviceId  The PCI device ID of the Intel HD Graphics device.

  @return    The default platform ID for the Intel HD Graphics device is returned.
  @return 0  The default platform ID for the Intel HD Graphics device was not
             found.
**/
UINT32
GetDefaultIntelPlatformId (
  IN UINT16 DeviceId
  )
{
  CONST INTEL_IGPU *TableEntry = GetIntelGraphicsDeviceTableEntry (DeviceId);

  if (TableEntry != NULL) {
    return TableEntry->DefaultPlatformId;
  }

  return 0;
}

/** Injects device properties for Intel HD Graphics devices.

  @param[in] IntelGraphicsDev  A pointer to the PCI type of the Intel HD
                               Graphics device.
  @param[in] DevicePath        The device path of the Intel HD Graphics device.

  @return                       The status of the Intel HD Graphics device
                                property injection is returned.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
InjectIntelGraphicsProperties (
  IN PCI_TYPE00               *IntelGraphicsDevice,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  CONST INTEL_IGPU *DeviceTableEntry;

  DeviceTableEntry = GetIntelGraphicsDeviceTableEntry (
                       IntelGraphicsDevice->Hdr.DeviceId
                       );

  MsgLog (
    "%a [8086:%04X] :: %s\n",
    GetIntelGraphicsName (IntelGraphicsDevice->Hdr.DeviceId),
    IntelGraphicsDevice->Hdr.DeviceId,
    FileDevicePathToStr (DevicePath)
    );

  // Do not inject properties if the EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL
  // is missing.
  if (gEfiDppDbProtocol == NULL) {
    MsgLog (" - EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found, disabling device property injection");
    return EFI_PROTOCOL_ERROR;
  }

  // Inject model-specific properties (based off the device table entry).
  if (DeviceTableEntry != NULL) {
    // Inject the AAPL,GfxYTile property for SKL/KBL IGPUs.
    if (DeviceTableEntry->Flags & FLAG_SKL_IG) {
      InjectDeviceProperty (
        DevicePath,
        L"AAPL,GfxYTile",
        DevicePropertyUint32,
        &mAaplGfxYTile,
        DevicePropertyWidthUint32
        );
    }

    // Inject the platform ID (SNB or IG).
    /// gSettings.IgPlatform (and mPlatformId) are already initialized to the
    /// default value in GetDevices if the user does not specify a value.
    if (gSettings.IgPlatform)
    {
      if (DeviceTableEntry->Flags & FLAG_SNB_IG) {
        InjectDeviceProperty (
          DevicePath,
          L"AAPL,snb-platform-id",
          DevicePropertyUint32,
          &gSettings.IgPlatform,
          DevicePropertyWidthUint32
          );
      } else {
        InjectDeviceProperty (
          DevicePath,
          L"AAPL,ig-platform-id",
          DevicePropertyUint32,
          &gSettings.IgPlatform,
          DevicePropertyWidthUint32
          );
      }
    }
  } else {
    // Fall back to generic property values if no device table entry exists.
    if (gSettings.IgPlatform)
    {
      InjectDeviceProperty (
        DevicePath,
        L"AAPL,ig-platform-id",
        DevicePropertyUint32,
        &gSettings.IgPlatform,
        DevicePropertyWidthUint32
        );
    }
  }

  // Inject a EDID override (if specified).
  // This is something AppleGraphicsPolicy should be doing...
  if (gSettings.InjectEDID && gSettings.CustomEDID) {
    gEfiDppDbProtocol->SetProperty (
      gEfiDppDbProtocol,
      DevicePath,
      L"AAPL00,override-no-connect",
      &gSettings.CustomEDID,
      128
      );
  }

  // Inject the fake device ID (if needed).
  /// gSettings.FakeIntel is already initialized to the default value in
  /// GetDevices if the user does not specify a value.
  mFakeId = gSettings.FakeIntel >> 16;
  if (mFakeId) {
    InjectDeviceProperty (
      DevicePath,
      L"device-id",
      DevicePropertyUint32,
      &mFakeId,
      DevicePropertyWidthUint32
      );
  }

  // Inject the graphic-options property.
  InjectDeviceProperty (
    DevicePath,
    L"graphic-options",
    DevicePropertyUint32,
    &mGraphicOptions,
    DevicePropertyWidthUint32
    );

  // Inject the hda-gfx property (if needed).
  if (gSettings.UseIntelHDMI) {
    InjectDeviceProperty (
      DevicePath,
      L"hda-gfx",
      DevicePropertyChar8,
      "onboard-1",
      10
      );
  }

  // Inject the model name.
  InjectDeviceProperty (
    DevicePath,
    L"model",
    DevicePropertyChar8,
    DeviceTableEntry->Name,
    AsciiStrLen (DeviceTableEntry->Name) + 1
    );

  return EFI_SUCCESS;
}
