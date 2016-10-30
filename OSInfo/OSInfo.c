/** @file
  Implementation of EFI_OS_INFO_PROTOCOL.

  Copyright (C) 2005 - 2015, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitely or explicitely permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#include <AppleCommon.h>

#include APPLE_PROTOCOL_PRODUCER (OSInfo)

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define EFI_OS_INFO_PROTOCOL_REVISION 0x01
#define OS_INFO_VENDOR_NAME           "Apple Inc."

VOID
EFIAPI
OSInfoOSNameImpl (
  IN CHAR8 *OSName
  )
{
  return;
}

VOID
EFIAPI
OSInfoOSVendorImpl (
  IN CHAR8 *OSVendor
  )
{
  INTN Result;

  if (!OSVendor) {
    return;
  }

  Result = AsciiStrCmp (OSVendor, OS_INFO_VENDOR_NAME);

  if (Result == 0) {
    // TODO: Fix this
    // EfiLibNamedEventSignal (&gAppleOsLoadedNamedEventGuid);
  }
}

EFI_OS_INFO_PROTOCOL mEfiOSInfo = {
  EFI_OS_INFO_PROTOCOL_REVISION,
  OSInfoOSNameImpl,
  OSInfoOSVendorImpl
};

/** Entry point for EFI_OS_INFO_PROTOCOL implementation.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The operation completed successfully.
  @retval EFI_ALREADY_STARTED  The protocol has already been installed.
**/
EFI_STATUS
EFIAPI
OSInfoEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiOSInfoProtocolGuid,
                  &mEfiOSInfo,
                  NULL
                  );

  return Status;
}
