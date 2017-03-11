/** @file
  Device property injector for High Definition Audio (HDA) controllers.

  Copyright (C) 2016-2017 Alex James. All rights reserved.<BR>
  Portions copyright (C) 2008-2009 Jasmin Fazlic. All rights reserved.<BR>
  Portions copyright (C) 2008-2009 iNDi. All rights reserved.<BR>
  Portions copyright (C) 2012 Sergey Slice. All rights reserved.<BR>
  Portions copyright (C) 2012 Damir Mazar. All rights reserved.<BR>

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

typedef struct {
  UINT32 VendorId;
  UINT32 DeviceId;
  CHAR8  *Name;
  UINT8  Flags;
} HDA_CONTROLLER;

#define FLAG_HDMI_AUDIO  (1 << 1)
#define FLAG_INJECT_BOTH (1 << 2)

STATIC CONST HDA_CONTROLLER mHdaControllerTable[] = {
  { 0x1002, 0xAAA0, "AMD Tahiti HDMI/DP Audio Controller" },
  { 0x1002, 0xAAB0, "AMD Cape Verde/Pitcairn HDMI/DP Audio Controller" },
  { 0x1002, 0xAAC0, "AMD Tobago HDMI/DP Audio Controller" },
  { 0x1002, 0xAAC8, "AMD Hawaii HDMI/DP Audio Controller" },
  { 0x1002, 0xAAD8, "AMD Tonga HDMI/DP Audio Controller" },
  { 0x1002, 0xAAE8, "AMD Fiji HDMI/DP Audio Controller" },
  { 0x10DE, 0x0E0A, "NVIDIA GK104 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0E0B, "NVIDIA GK106 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0E0F, "NVIDIA GK208 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0E1A, "NVIDIA GK110 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0E1B, "NVIDIA GK107 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0FB0, "NVIDIA GM200 HDMI/DP Audio Controller" },
  { 0x10DE, 0x0FBB, "NVIDIA GM204 HDMI/DP Audio Controller" },
  { 0x10DE, 0x10F0, "NVIDIA GP104 HDMI/DP Audio Controller" },
  { 0x8086, 0x1C20, "Intel 6 Series/C200 Series Chipset Family HDA Controller", FLAG_INJECT_BOTH },
  { 0x8086, 0x1D20, "Intel C600 Series/X79 Chipset Family HDA Controller" },
  { 0x8086, 0x1E20, "Intel 7 Series/C210 Series Chipset Family HDA Controller", FLAG_INJECT_BOTH },
  { 0x8086, 0x8C20, "Intel 8 Series/C220 Series Chipset Family HDA Controller" },
  { 0x8086, 0x8C21, "Intel 8 Series/C220 Series Chipset Family HDA Controller" },
  { 0x8086, 0x8CA0, "Intel 9 Series Chipset Family HDA Controller" },
  { 0x8086, 0x8D20, "Intel C610 Series/X99 Chipset Family HDA Controller" },
  { 0x8086, 0x8D21, "Intel C610 Series/X99 Chipset Family HDA Controller" },
  { 0x8086, 0xA170, "Intel 100 Series/C230 Series Chipset Family HDA Controller", FLAG_INJECT_BOTH },
  { 0, 0, NULL, 0 }
};

STATIC DEVICE_PROPERTY mHdaPropertyTable[] = {
  { L"hda-gfx",               DEVICE_PROPERTY_CHAR8,  "onboard-0", 10, FLAG_HDMI_AUDIO },
  { L"layout-id",             DEVICE_PROPERTY_UINT32, &gSettings.HDALayoutId, 4 },
  { L"MaximumBootBeepVolume", DEVICE_PROPERTY_UINT8,  0, 1 },
  { L"PinConfigurations",     DEVICE_PROPERTY_UINT8,  0, 1 },
  { L"platformFamily",        DEVICE_PROPERTY_UINT8,  0, 1 },
  { NULL, 0, 0 }
};

/** Retrieves the name of a High Definition Audio (HDA) controller.

  @param[in] VendorId  The PCI vendor ID of the HDA controller.
  @param[in] DeviceId  The PCI device ID of the HDA controller.

  @return                           The name of the HDA controller is returned.
  @return "Unknown HDA Controller"  A matching name for the HDA controller could not
                                    be found.
**/
CHAR8 *
GetHdaControllerName (
  IN UINT32 VendorId,
  IN UINT32 DeviceId
  )
{
  INTN Index;

  for (Index = 0; mHdaControllerTable[Index].VendorId != 0; ++Index) {
    if (
      mHdaControllerTable[Index].VendorId == VendorId &&
      mHdaControllerTable[Index].DeviceId == DeviceId
      ) {
      return mHdaControllerTable[Index].Name;
    }
  }

  switch (VendorId) {
    case 0x1002:
    {
      return "Unknown AMD HDA Controller";
      break;
    }

    case 0x10DE:
    {
      return "Unknown NVIDIA HDA Controller";
      break;
    }

    case 0x8086:
    {
      return "Unknown Intel HDA Controller";
      break;
    }

    default:
    {
      return "Unknown HDA Controller";
      break;
    }
  }
}

STATIC
CONST
HDA_CONTROLLER *
GetHdaController (
  IN UINT32 VendorId,
  IN UINT32 DeviceId
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

/** Injects the necessary device properties for High Definition Audio (HDA) controllers.

  @param[in] HdaController  A pointer to the PCI type of the HDA controller.
  @param[in] DevicePath     The device path of the HDA controller.
  @param[in] IsHdmiAudio    Specifies whether the HDA controller is a GPU
                            HDMI/DP audio controller.

  @return                       The status of the HDA device injection is returned.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval EFI_SUCCESS           The operation completed successfully.
**/
EFI_STATUS
InjectHdaProperties (
  IN PCI_TYPE00               *HdaControllerDev,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN BOOLEAN                  IsHdmiAudio
  )
{
  CONST HDA_CONTROLLER *HdaController;
  CHAR8                HdaGfxString[10];
  STATIC UINTN         HdmiControllerCount = 0;
  INTN                 Index;
  EFI_STATUS           Status;

  HdaController = GetHdaController (
                    HdaControllerDev->Hdr.VendorId,
                    HdaControllerDev->Hdr.DeviceId
                    );

  MsgLog (
    "%a [%04X:%04X] :: %s\n",
    GetHdaControllerName (HdaControllerDev->Hdr.VendorId, HdaControllerDev->Hdr.DeviceId),
    HdaControllerDev->Hdr.VendorId,
    HdaControllerDev->Hdr.DeviceId,
    FileDevicePathToStr (DevicePath)
    );

  // TODO: Remove once an implementation is added to Clover
  if (gEfiDppDbProtocol == NULL) {
    return EFI_PROTOCOL_ERROR;
  }

  //
  // Some chipset HDA controllers (Intel 6/7/100 Series) are also HDMI/DP audio
  // controllers. Inject HDMI audio properties for these controllers.
  //
  if ((HdaController->Flags & FLAG_INJECT_BOTH) || IsHdmiAudio) {
    ++HdmiControllerCount;
    AsciiSPrint (HdaGfxString, 10, "onboard-%d", HdmiControllerCount);
    AsciiStrCpyS (mHdaPropertyTable[0].Value, 10, HdaGfxString);
  }

  FreePool (HdaGfxString);

  for (Index = 0; mHdaPropertyTable[Index].Name != NULL; ++Index) {
    //
    // Only inject properties with FLAG_HDMI_AUDIO flag for GPU HDMI/DP audio controllers.
    //
    if ((((IsHdmiAudio) && !(mHdaPropertyTable[Index].Flags & FLAG_HDMI_AUDIO)) ||
     ((!IsHdmiAudio) && (mHdaPropertyTable[Index].Flags & FLAG_HDMI_AUDIO))) &&
     !(HdaController->Flags & FLAG_INJECT_BOTH)) {
      continue;
    }

    Status = gEfiDppDbProtocol->SetProperty (
                                  gEfiDppDbProtocol,
                                  DevicePath,
                                  mHdaPropertyTable[Index].Name,
                                  mHdaPropertyTable[Index].Value,
                                  mHdaPropertyTable[Index].Size
                                  );

    LogInjectionStatus (&mHdaPropertyTable[Index], Status);
  }

  return Status;
}
