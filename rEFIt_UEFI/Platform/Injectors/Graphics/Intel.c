/** @file
  Device property injector for Intel IGPUs.

  TODO: Show default fake ID/platform ID in the Clover Graphics Injector Menu.
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
  UINT32 FakeId;
  CHAR8  *Name;
  UINT32 DefaultPlatformId;
  UINT8  Flags;
} INTEL_IGPU;

#define FLAG_SANDY_BRIDGE  (1 << 1)

STATIC UINT32 mGraphicOptions = 0x0C;

STATIC CONST INTEL_IGPU mIntelGraphicsDeviceTable[] = {
  // Sandy Bridge Server GT2
  { 0x010A, 0x01168086, "Intel HD Graphics P3000", 0x00030010, FLAG_SANDY_BRIDGE },
  // Sandy Bridge GT2
  { 0x0112, 0x01168086, "Intel HD Graphics 3000",  0x00030010, FLAG_SANDY_BRIDGE },
  // Sandy Bridge Mobile GT2
  { 0x0116, 0x00000000, "Intel HD Graphics 3000",  0x00000000, FLAG_SANDY_BRIDGE },
  // Sandy Bridge Desktop GT2
  { 0x0122, 0x01268086, "Intel HD Graphics 3000",  0x00030010, FLAG_SANDY_BRIDGE },
  // Sandy Bridge Mobile GT2
  { 0x0126, 0x00000000, "Intel HD Graphics 3000",  0x00000000, FLAG_SANDY_BRIDGE },
  // Ivy Bridge GT2
  { 0x0162, 0x00000000, "Intel HD Graphics 4000",  0x0166000B },
  // Ivy Bridge Mobile GT2
  { 0x0166, 0x00000000, "Intel HD Graphics 4000",  0x01660001 },
  // Ivy Bridge GT2
  { 0x016A, 0x01628086, "Intel HD Graphics P4000", 0x0166000A },
  // Haswell Desktop GT2
  { 0x0412, 0x00000000, "Intel HD Graphics 4600",  0x0D220003 },
  { 0, 0, NULL, 0 }
};

STATIC DEVICE_PROPERTY mIntelGraphicsPropertyTable[] = {
  { L"AAPL,ig-platform-id",  DEVICE_PROPERTY_UINT32, 0, 4 },
  { L"AAPL,snb-platform-id", DEVICE_PROPERTY_UINT32, 0, 4, FLAG_SANDY_BRIDGE },
  { L"device-id",            DEVICE_PROPERTY_UINT32, 0, 4 },
  { L"graphic-options",      DEVICE_PROPERTY_UINT32, &mGraphicOptions, 4 },
  { L"hda-gfx",              DEVICE_PROPERTY_CHAR8,  "onboard-1", 10 },
  { L"model",                DEVICE_PROPERTY_CHAR8,  "Unknown Intel Graphics Device", 30 },
  { NULL, 0, 0, 0 }
};

CHAR8 *
GetIntelGraphicsName (
  IN UINT16 DeviceId
  )
{
  INTN Index;

  for (Index = 0; mIntelGraphicsDeviceTable[Index].DeviceId != 0; ++Index) {
    if (mIntelGraphicsDeviceTable[Index].DeviceId == DeviceId) {
      return mIntelGraphicsDeviceTable[Index].Name;
    }
  }

  return "Unknown Intel Graphics Device";
}

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

EFI_STATUS
InjectIntelGraphicsProperties (
  IN PCI_TYPE00               *IntelGraphicsDev,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  CONST INTEL_IGPU *DeviceTableEntry;
  UINT32           FakeDeviceId;
  INTN             Index;
  EFI_STATUS       Status;

  DeviceTableEntry = GetIntelGraphicsDeviceTableEntry (
                       IntelGraphicsDev->Hdr.DeviceId
                       );

  MsgLog (
    "%a [8086:%04X] :: %s\n",
    GetIntelGraphicsName (IntelGraphicsDev->Hdr.DeviceId),
    IntelGraphicsDev->Hdr.DeviceId,
    FileDevicePathToStr (DevicePath)
    );

  // Do not inject properties for unknown Intel IGPUs.
  // TODO: Make this behavior more flexible. If the user specifies a platform
  // ID, injection should still work, even if the GPU isn't in the device table.
  if (DeviceTableEntry == NULL) {
    MsgLog (" - Unsupported Intel IGPU, disabling device property injection");
    return EFI_NOT_FOUND;
  }

  // Do not inject properties if the EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL
  // is missing.
  if (gEfiDppDbProtocol == NULL) {
    MsgLog (" - EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found, disabling device property injection");
    return EFI_PROTOCOL_ERROR;
  }

  // Initialize the device property table.
  /// Use the default platform ID if none is specified by the user.
  if (!gSettings.IgPlatform) {
    gSettings.IgPlatform = DeviceTableEntry->DefaultPlatformId;
  }

  /// Set the platform ID.
  mIntelGraphicsPropertyTable[0].Value = &gSettings.IgPlatform;
  mIntelGraphicsPropertyTable[1].Value = &gSettings.IgPlatform;

  /// Use the default fake device ID if none is specified by the user.
  if (!gSettings.FakeIntel) {
    gSettings.FakeIntel = DeviceTableEntry->FakeId;
  }

  /// Set the fake device ID.
  FakeDeviceId = gSettings.FakeIntel >> 16;
  mIntelGraphicsPropertyTable[2].Value = &FakeDeviceId;

  /// Set the model property.
  ZeroMem (mIntelGraphicsPropertyTable[5].Value, 30);
  mIntelGraphicsPropertyTable[5].Size = AsciiStrLen (DeviceTableEntry->Name) + 1;
  AsciiStrCpyS (
    mIntelGraphicsPropertyTable[5].Value,
    mIntelGraphicsPropertyTable[5].Size,
    DeviceTableEntry->Name
    );

  // Inject the device properties in the property table.
  for (Index = 0; mIntelGraphicsPropertyTable[Index].Name != NULL; ++Index) {
    /// Skip AAPL,ig-platform-id for Sandy Bridge.
    if ((DeviceTableEntry->Flags & FLAG_SANDY_BRIDGE) && Index == 0) {
      continue;
    /// Skip AAPL,snb-platform-id for newer IGPUs.
    } else if ((mIntelGraphicsPropertyTable[Index].Flags & FLAG_SANDY_BRIDGE) && Index == 1) {
      continue;
    }

    /// Skip device-id if a fake device ID isn't set.
    if (!gSettings.FakeIntel && Index == 2) {
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
