/** @file
  Driver that installs the APPLE_USER_INTERFACE_THEME_PROTOCOL.

  Copyright (C) 2005 - 2015, Apple Inc.  All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitly or explicitly permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Guid/AppleNvram.h>

#include <Protocol/AppleUserInterfaceTheme.h>

// COLOR_BLACK
#define COLOR_BLACK  0x000000

// mCurrentColor
STATIC UINT32 mCurrentColor;

// UserInterfaceThemeGetColor
EFI_STATUS
EFIAPI
UserInterfaceThemeGetColor (
  IN OUT UINT32  *Color
  )
{
  if (Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Color = mCurrentColor;
  return EFI_SUCCESS;
}

// mUserInterfaceThemeProtocol
STATIC APPLE_USER_INTERFACE_THEME_PROTOCOL mUserInterfaceThemeProtocol = {
  0x01,
  UserInterfaceThemeGetColor
};

EFI_STATUS
EFIAPI
UserInterfaceThemeDriverMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  ) // start
{
  EFI_STATUS Status;

  UINT32     Color;
  UINTN      DataSize;
  VOID       *Interface;

  mCurrentColor = COLOR_BLACK;

  DataSize = 0;
  Status = gRT->GetVariable (
                  L"DefaultBackgroundColor",
                  &gAppleVendorVariableGuid,
                  0,
                  &DataSize,
                  &Color);

  if (!EFI_ERROR (Status)) {
    mCurrentColor = Color;
  }

  Status = gBS->LocateProtocol (
                  &gAppleUserInterfaceThemeProtocolGuid,
                  NULL,
                  &Interface
                  );

  if (EFI_ERROR (Status)) {
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gAppleUserInterfaceThemeProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    (VOID **)&mUserInterfaceThemeProtocol
                    );

    Status = EFI_SUCCESS;
  } else {
    Status = EFI_ALREADY_STARTED;
  }

  return Status;
}
