/** @file
  Implementation of EFI_OS_INFO_PROTOCOL.

  Copyright (C) 2005 - 2015, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitly or explicitly permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#include <Protocol/OSInfo.h>

#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#define EFI_OS_INFO_PROTOCOL_REVISION  0x01
#define OS_INFO_VENDOR_NAME            "Apple Inc."

VOID
EFIAPI
EfiOSInfoOSNameImpl (
  IN CHAR8 *OSName
  )
{
  CHAR8 *BootOSName;

  BootOSName = AllocateCopyPool (AsciiStrLen (OSName) + 1, (VOID *)OSName);

  EfiNamedEventSignal (&gAppleOsLoadedNamedEventGuid);
}

VOID
EFIAPI
EfiOSInfoOSVendorImpl (
  IN CHAR8 *OSVendor
  )
{
  INTN Result;

  if (!OSVendor) {
    return;
  }

  Result = AsciiStrCmp (OSVendor, OS_INFO_VENDOR_NAME);
}

EFI_OS_INFO_PROTOCOL mEfiOSInfo = {
  EFI_OS_INFO_PROTOCOL_REVISION,
  EfiOSInfoOSNameImpl,
  EfiOSInfoOSVendorImpl
};

/** Entry point for EFI_OS_INFO_PROTOCOL implementation.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The operation completed successfully.
  @retval EFI_ALREADY_STARTED  The protocol has already been installed.
**/
EFI_STATUS
EFIAPI
EfiOSInfoMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID       *Interface;
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (
                  &gEfiOSInfoProtocolGuid,
                  NULL,
                  &Interface
                  );

  if (EFI_ERROR (Status)) {
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gEfiOSInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    (VOID **)&mEfiOSInfo
                    );
  } else {
    Status = EFI_ALREADY_STARTED;
  }

  return Status;
}
