/** @file
  Driver that installs the APPLE_GRAPHICS_CONFIGURATION_PROTOCOL.

  Copyright (C) 2005 - 2015, Apple Inc.  All rights reserved.<BR>
  Portions copyright (C) 2016 Sergey Slice. All rights reserved.<BR>

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

#include <Protocol/AppleGraphicsConfiguration.h>

EFI_STATUS
RestoreConfiguration (
  IN APPLE_GRAPHICS_CONFIGURATION_PROTOCOL *This,
  IN UINT32                                 Ukn1,
  IN UINT32                                 Ukn2,
  IN VOID                                  *Ukn3,
  IN VOID                                  *Ukn4,
  IN VOID                                  *Ukn5
  )
{
  return EFI_SUCCESS;
}

APPLE_GRAPHICS_CONFIGURATION_PROTOCOL mGraphicsConfiguration = {
  0x01,
  RestoreConfiguration,
};

EFI_STATUS
EFIAPI
AppleGraphicsConfigurationMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID       *Interface;
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (
                  &gAppleGraphicsConfigurationProtocolGuid,
                  NULL,
                  &Interface
                  );

  if (EFI_ERROR (Status)) {
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gAppleGraphicsConfigurationProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    (VOID **)&mGraphicsConfiguration
                    );

    Status = EFI_SUCCESS;
  } else {
    Status = EFI_ALREADY_STARTED;
  }

  return Status;
}