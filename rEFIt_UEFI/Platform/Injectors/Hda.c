/** @file
  Device property injector for High Definition Audio (HDA) controllers.

  Copyright (C) 2016-2017 Alex James (TheRacerMaster). All rights reserved.<BR>
  Portions copyright (C) 2008-2009 Jasmin Fazlic. All rights reserved.<BR>
  Portions copyright (C) 2008-2009 iNDi. All rights reserved.<BR>
  Portions copyright (C) 2012 Sergey Slice. All rights reserved.<BR>
  Portions copyright (C) 2012 Damir Mazar. All rights reserved.<BR>

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

// HDA controller flags
enum {
  /// Used for normal HDA devices on chipsets
  FLAG_HDA_HDEF = 1,
  /// Used for HDMI/DP audio controllers on PCIe GPUs/IGPUs
  FLAG_HDA_HDMI = 2,
  /// Used for Skylake/Kaby Lake chipset audio controllers
  FLAG_HDA_IDLE = 4
};

typedef struct {
  UINT16      VendorId;
  UINT16      DeviceId;
  CONST CHAR8 *Name;
  UINT8       Flags;
} HDA_CONTROLLER;

STATIC UINT8 mZeroValue = 0x00;

STATIC CONST HDA_CONTROLLER mHdaControllerTable[] = {
  { 0x1002, 0xAAA0, "AMD Tahiti HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x1002, 0xAAB0, "AMD Cape Verde/Pitcairn HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x1002, 0xAAC0, "AMD Tobago HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x1002, 0xAAC8, "AMD Hawaii HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x1002, 0xAAD8, "AMD Tonga HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x1002, 0xAAE8, "AMD Fiji HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0E0A, "NVIDIA GK104 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0E0B, "NVIDIA GK106 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0E0F, "NVIDIA GK208 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0E1A, "NVIDIA GK110 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0E1B, "NVIDIA GK107 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0FB0, "NVIDIA GM200 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x0FBB, "NVIDIA GM204 HDMI/DP Audio Controller", FLAG_HDA_HDMI },
  { 0x10DE, 0x10F0, "NVIDIA GP104 HDMI/DP Audio Controller", FLAG_HDA_HDMI, },
  { 0x8086, 0x1C20, "Intel 6 Series/C200 Series Chipset Family HDA Controller", FLAG_HDA_HDEF|FLAG_HDA_HDMI },
  { 0x8086, 0x1D20, "Intel C600 Series/X79 Chipset Family HDA Controller", FLAG_HDA_HDEF|FLAG_HDA_HDMI },
  { 0x8086, 0x1E20, "Intel 7 Series/C210 Series Chipset Family HDA Controller", FLAG_HDA_HDEF|FLAG_HDA_HDMI },
  { 0x8086, 0x8C20, "Intel 8 Series/C220 Series Chipset Family HDA Controller", FLAG_HDA_HDEF },
  { 0x8086, 0x8C21, "Intel 8 Series/C220 Series Chipset Family HDA Controller", FLAG_HDA_HDEF },
  { 0x8086, 0x8CA0, "Intel 9 Series Chipset Family HDA Controller", FLAG_HDA_HDEF },
  { 0x8086, 0x8D20, "Intel C610 Series/X99 Chipset Family HDA Controller", FLAG_HDA_HDEF },
  { 0x8086, 0x8D21, "Intel C610 Series/X99 Chipset Family HDA Controller", FLAG_HDA_HDEF },
  { 0x8086, 0xA170, "Intel 100 Series/C230 Series Chipset Family HDA Controller", FLAG_HDA_HDEF|FLAG_HDA_HDMI|FLAG_HDA_IDLE },
  { 0x8086, 0xA171, "Intel 100 Series/C230 Series Chipset Family HDA Controller", FLAG_HDA_HDEF|FLAG_HDA_HDMI|FLAG_HDA_IDLE },

  { 0, 0, NULL, 0 }
};

STATIC
CONST
HDA_CONTROLLER *
GetHdaControllerTableEntry (
  IN UINT16 VendorId,
  IN UINT16 DeviceId
  )
{
  INTN Index;

  for (Index = 0; mHdaControllerTable[Index].VendorId != 0; ++Index) {
    if (
     mHdaControllerTable[Index].VendorId == VendorId &&
     mHdaControllerTable[Index].DeviceId == DeviceId
      ) {
      return &mHdaControllerTable[Index];
    }
  }

  return NULL;
}

/** Retrieves the name of a High Definition Audio (HDA) controller.

  @param[in] VendorId  The PCI vendor ID of the HDA controller.
  @param[in] DeviceId  The PCI device ID of the HDA controller.

  @return                           The name of the HDA controller is returned.
  @return "Unknown HDA Controller"  The name of the HDA controller was not found.
**/
CONST
CHAR8 *
GetHdaControllerName (
  IN UINT16 VendorId,
  IN UINT16 DeviceId
  )
{
  CONST HDA_CONTROLLER *TableEntry = GetHdaControllerTableEntry (VendorId, DeviceId);

  if (TableEntry != NULL) {
    return TableEntry->Name;
  }

  switch (VendorId) {
    case 0x1002:
      return "Unknown AMD HDA Controller";
    case 0x10DE:
      return "Unknown NVIDIA HDA Controller";
    case 0x8086:
      return "Unknown Intel HDA Controller";
    default:
      return "Unknown HDA Controller";
  }
}

/** Injects device properties for High Definition Audio (HDA) controllers.

  @param[in] HdaController  A pointer to the PCI type of the HDA controller.
  @param[in] DevicePath     The device path of the HDA controller.
  @param[in] IsHdmiAudio    Specifies whether the HDA controller is a GPU
                            HDMI/DP audio controller.

  @return                       The status of the HDA device property injection
                                is returned.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
InjectHdaProperties (
  IN PCI_TYPE00               *HdaDevice,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN BOOLEAN                  IsHdmiAudio
  )
{
  CONST HDA_CONTROLLER *DeviceTableEntry;
  CHAR8                HdaGfxString[10];
  STATIC UINTN         HdmiControllerCount = 1;

  DeviceTableEntry = GetHdaControllerTableEntry (
                       HdaDevice->Hdr.VendorId,
                       HdaDevice->Hdr.DeviceId
                       );

  MsgLog (
    "%a [%04X:%04X] :: %s\n",
    GetHdaControllerName (HdaDevice->Hdr.VendorId, HdaDevice->Hdr.DeviceId),
    HdaDevice->Hdr.VendorId,
    HdaDevice->Hdr.DeviceId,
    FileDevicePathToStr (DevicePath)
    );

  // Do not inject properties if the EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL
  // is missing
  if (gEfiDppDbProtocol == NULL) {
    MsgLog (" - EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found, disabling device property injection");
    return EFI_PROTOCOL_ERROR;
  }

  // Inject controller-specific device properties (based off the device table entry)
  if (DeviceTableEntry != NULL)
  {
    // Inject the hda-gfx property for HDMI/DP audio controllers
    /// IGPU HDMI/DP audio controllers get priority (onboard-1) if UseIntelHDMI
    /// is true (uses the same value as InjectIntel by default)
    /// Don't inject hda-gfx for IGPUs if UseIntelHDMI isn't set, and increment
    /// the onboard-n value for subsequent HDMI/DP audio controllers
    if ((DeviceTableEntry->Flags & FLAG_HDA_HDMI)
        && HdaDevice->Hdr.VendorId == 0x8086 && gSettings.UseIntelHDMI) {
      InjectDeviceProperty (
        DevicePath,
        L"hda-gfx",
        DevicePropertyChar8,
        "onboard-1",
        10
        );
    } else if ((IsHdmiAudio || (DeviceTableEntry->Flags & FLAG_HDA_HDMI))
               && HdaDevice->Hdr.VendorId != 0x8086) {
      if (HdmiControllerCount == 1 && gSettings.UseIntelHDMI) {
        ++HdmiControllerCount;
      }

      AsciiSPrint (HdaGfxString, 10, "onboard-%d", HdmiControllerCount);
      InjectDeviceProperty (
        DevicePath,
        L"hda-gfx",
        DevicePropertyChar8,
        HdaGfxString,
        10
        );

      ++HdmiControllerCount;
    }

    // Inject the hda-idle-support property for SKL/KBL audio controllers
    if (DeviceTableEntry->Flags & FLAG_HDA_IDLE) {
      InjectDeviceProperty (
        DevicePath,
        L"hda-idle-support",
        DevicePropertyChar8,
        "1",
        2
        );
    }

    // Only inject the following properties for chipset HDA controllers
    if (DeviceTableEntry->Flags & FLAG_HDA_HDEF) {
      // Inject the MaximumBootBeepVolume properties
      InjectDeviceProperty (
        DevicePath,
        L"MaximumBootBeepVolume",
        DevicePropertyUint,
        &gSettings.HDALayoutId,
        DevicePropertyWidthUint32
        );

      InjectDeviceProperty (
        DevicePath,
        L"MaximumBootBeepVolumeAlt",
        DevicePropertyUint,
        &gSettings.HDALayoutId,
        DevicePropertyWidthUint32
        );

      // Inject the platformFamily property
      InjectDeviceProperty (
        DevicePath,
        L"platformFamily",
        DevicePropertyUint,
        &mZeroValue,
        DevicePropertyWidthUint8
        );
    }

  // Fall back to generic property values if no device table entry exists
  } else {
    // Inject the hda-gfx property (if needed)
    if (IsHdmiAudio) {
      AsciiSPrint (HdaGfxString, 10, "onboard-%d", HdmiControllerCount);
      InjectDeviceProperty (
        DevicePath,
        L"hda-gfx",
        DevicePropertyChar8,
        HdaGfxString,
        10
        );

      ++HdmiControllerCount;
    }
  }

  // Inject the layout ID
  InjectDeviceProperty (
    DevicePath,
    L"layout-id",
    DevicePropertyUint,
    &gSettings.HDALayoutId,
    DevicePropertyWidthUint32
    );

  // Inject the PinConfigurations property
  InjectDeviceProperty (
    DevicePath,
    L"PinConfigurations",
    DevicePropertyUint,
    &mZeroValue,
    DevicePropertyWidthUint8
    );

  return EFI_SUCCESS;
}
