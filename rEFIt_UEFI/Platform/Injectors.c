/*
 * Copyright (C) 2010 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */
/*
 Slice 2011,
 some more adoptations for Apple's OS
 */

/*******************************************************************************
 *   Header Files                                                               *
 *******************************************************************************/
#include "Platform.h"
#include <Protocol/KeyboardInfo.h>

/*
EFI_GUID gAppleScreenInfoProtocolGuid = {
	0xe316e100, 0x0751, 0x4c49, {0x90, 0x56, 0x48, 0x6c, 0x7e, 0x47, 0x29, 0x03}
}; */
// gEfiKeyboardInfoProtocolGuid
// {0xE82A0A1E, 0x0E4D, 0x45AC, {0xA6, 0xDC, 0x2A, 0xE0, 0x58, 0x00, 0xD3, 0x11}}

extern EFI_GUID gAppleScreenInfoProtocolGuid;

UINT32 mPropSize = 0;
UINT8* mProperties = NULL;
CHAR8* gDeviceProperties = NULL;

UINT32 cPropSize = 0;
UINT8* cProperties = NULL;
CHAR8* cDeviceProperties = NULL;
CHAR8* BootOSName = NULL;

UINT16 KeyboardVendor = 0x05ac; //Apple inc.
UINT16 KeyboardProduct = 0x021d; //iMac aluminium

typedef	EFI_STATUS (EFIAPI *EFI_SCREEN_INFO_FUNCTION)(
                                                      VOID* This,
                                                      UINT64* baseAddress,
                                                      UINT64* frameBufferSize,
                                                      UINT32* byterPerRow,
                                                      UINT32* Width,
                                                      UINT32* Height,
                                                      UINT32* colorDepth
                                                      );

typedef struct {
	EFI_SCREEN_INFO_FUNCTION GetScreenInfo;
} EFI_INTERFACE_SCREEN_INFO;

EFI_STATUS EFIAPI GetScreenInfo(VOID* This, UINT64* baseAddress, UINT64* frameBufferSize,
                         UINT32* bpr, UINT32* w, UINT32* h, UINT32* colorDepth)
{
  /*
   * This implementation has no sense
   * as this protocol will be called only if GOP is absent. Somehow as a replacement
   * There will be a sense to find bootargs->frame_address
   * and fill other values
   * but we always have GOP ;)
   */
	EFI_GRAPHICS_OUTPUT_PROTOCOL	*mGraphicsOutput=NULL;
	EFI_STATUS						Status;

	Status = gBS->HandleProtocol (gST->ConsoleOutHandle,
                                &gEfiGraphicsOutputProtocolGuid,
                                (VOID **) &mGraphicsOutput);
	if(EFI_ERROR(Status))
		return EFI_UNSUPPORTED;
  //this print never occured so this procedure is redundant
//	Print(L"GetScreenInfo called with args: %lx %lx %lx %lx %lx %lx\n",
//        baseAddress, frameBufferSize, bpr, w, h, colorDepth);
	*frameBufferSize = (UINT64)mGraphicsOutput->Mode->FrameBufferSize;
	*baseAddress = (UINT64)mGraphicsOutput->Mode->FrameBufferBase;
	*w = (UINT32)mGraphicsOutput->Mode->Info->HorizontalResolution;
	*h = (UINT32)mGraphicsOutput->Mode->Info->VerticalResolution;
	*colorDepth = 32;
	*bpr = (UINT32)(mGraphicsOutput->Mode->Info->PixelsPerScanLine*32) >> 3;
//	Print(L"  Screen info: FBsize=%lx FBaddr=%lx w=%d h=%d\n",
//      *frameBufferSize, *baseAddress, *w, *h);
//  PauseForKey(L"--- press any key ---\n");
	return EFI_SUCCESS;
}

EFI_INTERFACE_SCREEN_INFO mScreenInfo=
{
	GetScreenInfo
};

EFI_STATUS
EFIAPI
UsbKbGetKeyboardDeviceInfo (
                            OUT UINT16  *IdVendor,
                            OUT UINT16  *IdProduct,
                            OUT UINT8   *CountryCode
                            )
{
  if (IdVendor) {
    *IdVendor    = KeyboardVendor;
  }
  if (IdProduct) {
    *IdProduct   = KeyboardProduct;
  }
  if (CountryCode) {
    *CountryCode = 0;
  }

  return EFI_SUCCESS;
}

EFI_KEYBOARD_INFO_PROTOCOL mKeyboardInfo = {
  UsbKbGetKeyboardDeviceInfo
};

EFI_STATUS
SetPrivateVarProto(VOID)
{
  EFI_STATUS  Status;
  //This must be independent install
  // optional protocols
  Status = gBS->InstallMultipleProtocolInterfaces (&gImageHandle,
                                                    &gAppleScreenInfoProtocolGuid,
                                                    &mScreenInfo,
                                                    &gEfiKeyboardInfoProtocolGuid,
                                                    &mKeyboardInfo,
                                                    NULL
                                                    );

  return Status;
}
