/** @file
  Device property injector for Intel HD Graphics devices.

  TODO: Read the DVMT value and log it.

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
  FLAG_SKL_IG
};

STATIC UINT32 mAaplGfxYTile   = 0x01000000;
STATIC UINT32 mGraphicOptions = 0x0000000C;
STATIC UINT32 mZeroValue      = 0x00000000;

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

STATIC DEVICE_PROPERTY mIntelGraphicsPropertyTable[] = {
  { L"AAPL,GfxYTile",        DevicePropertyUint32, &mAaplGfxYTile, 4, FLAG_SKL_IG },
  { L"AAPL,ig-platform-id",  DevicePropertyUint32, &mZeroValue, 4 },
  { L"AAPL,snb-platform-id", DevicePropertyUint32, &mZeroValue, 4, FLAG_SNB_IG },
  { L"device-id",            DevicePropertyUint32, &mZeroValue, 4 },
  { L"graphic-options",      DevicePropertyUint32, &mGraphicOptions, 4 },
  { L"hda-gfx",              DevicePropertyChar8,  "onboard-1", 10 },
  { L"model",                DevicePropertyChar8,  "Unknown Intel HD Graphics Device", 33 },
  { NULL, 0, NULL, 0 }
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
  UINT32           FakeDeviceId;
  INTN             Index;
  EFI_STATUS       Status;

  DeviceTableEntry = GetIntelGraphicsDeviceTableEntry (
                       IntelGraphicsDevice->Hdr.DeviceId
                       );

  MsgLog (
    "%a [8086:%04X] :: %s\n",
    GetIntelGraphicsName (IntelGraphicsDevice->Hdr.DeviceId),
    IntelGraphicsDevice->Hdr.DeviceId,
    FileDevicePathToStr (DevicePath)
    );

  // Do not inject properties for unknown Intel HD Graphics devices.
  // TODO: Make this behavior more flexible. If the user specifies a platform
  // ID, injection should still work, even if the GPU isn't in the device table.
  if (DeviceTableEntry == NULL) {
    MsgLog (" - Unsupported Intel HD Graphics device, disabling device property injection");
    return EFI_NOT_FOUND;
  }

  // Do not inject properties if the EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL
  // is missing.
  if (gEfiDppDbProtocol == NULL) {
    MsgLog (" - EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found, disabling device property injection");
    return EFI_PROTOCOL_ERROR;
  }

  // Initialize the device property table.
  /// Set the platform ID.
  mIntelGraphicsPropertyTable[1].Value = (VOID *)&gSettings.IgPlatform;
  mIntelGraphicsPropertyTable[2].Value = (VOID *)&gSettings.IgPlatform;

  /// Use the default fake device ID if none is specified by the user.
  if (!gSettings.FakeIntel) {
    gSettings.FakeIntel = DeviceTableEntry->DefaultFakeId;
  }

  /// Set the fake device ID.
  /// gSettings.FakeIntel is already set to the default in GetDevices.
  FakeDeviceId = gSettings.FakeIntel >> 16;
  mIntelGraphicsPropertyTable[3].Value = (VOID *)&FakeDeviceId;

  /// Set the model property.
  mIntelGraphicsPropertyTable[6].Size = AsciiStrLen (DeviceTableEntry->Name) + 1;
  mIntelGraphicsPropertyTable[6].Value = (VOID *)DeviceTableEntry->Name;

  // Inject the device properties in the property table.
  for (Index = 0; mIntelGraphicsPropertyTable[Index].Name != NULL; ++Index) {
    /// Skip AAPL,GfxYTile for older Intel HD Graphics devices.
    if (!(DeviceTableEntry->Flags & FLAG_SKL_IG) && Index == 0) {
      continue;
    }

    /// Skip AAPL,ig-platform-id for Sandy Bridge HD Graphics devices.
    if (DeviceTableEntry->Flags & FLAG_SNB_IG && Index == 1) {
      continue;
    }

    /// Skip AAPL,snb-platform-id for newer Intel HD Graphics devices.
    if (!(DeviceTableEntry->Flags & FLAG_SNB_IG) && Index == 2) {
      continue;
    }

    /// Skip device-id if a fake device ID isn't set.
    if (!gSettings.FakeIntel && Index == 3) {
      continue;
    }

    Status = gEfiDppDbProtocol->SetProperty (
                                  gEfiDppDbProtocol,
                                  DevicePath,
                                  mIntelGraphicsPropertyTable[Index].Name,
                                  mIntelGraphicsPropertyTable[Index].Value,
                                  mIntelGraphicsPropertyTable[Index].Size
                                  );

    LogInjectionStatus (&mIntelGraphicsPropertyTable[Index], Status);
  }

  return Status;
}
