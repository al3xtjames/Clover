/** @file
  Apple UEFI protocol installer.

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

#include <Driver/AppleGraphicsConfiguration.h>
#include <Driver/EfiOSInfo.h>
#include <Driver/FvInject.h>
#include <Driver/SmcHelper.h>
#include <Driver/UserInterfaceThemeDriver.h>

// INSTALL_PROTOCOL_IMPL
/// Macro to install a protocol implementation.
#define INSTALL_PROTOCOL_IMPL(Protocol, EntryPoint)                  \
          Status = EntryPoint (gImageHandle, gST);                   \
          MsgLog ("Installed %a, Status = %r\n", Protocol, Status);

//
// Pointer to the EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL.
//
EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL *gEfiDppDbProtocol = NULL;

/** Installs the Apple UEFI protocols.

  @retval EFI_SUCCESS  The operation completed successfully.
**/
EFI_STATUS
InstallAppleProtocols (
  VOID
  )
{
  EFI_STATUS Status;

  // TODO: Remove once an implementation is added to Clover
  Status = gBS->LocateProtocol (
                  &gEfiDevicePathPropertyDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&gEfiDppDbProtocol
                  );

  if (EFI_ERROR (Status)) {
    MsgLog ("EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found. Device property injection will fail.\n");
  }

  INSTALL_PROTOCOL_IMPL ("APPLE_GRAPHICS_CONFIGURATION_PROTOCOL", AppleGraphicsConfigurationMain);
  INSTALL_PROTOCOL_IMPL ("APPLE_SMC_IO_PROTOCOL", SmcHelperMain);
  INSTALL_PROTOCOL_IMPL ("APPLE_USER_INTERFACE_THEME_PROTOCOL", UserInterfaceThemeDriverMain);
  INSTALL_PROTOCOL_IMPL ("EFI_FIRMWARE_VOLUME_PROTOCOL", FvInjectMain);
  INSTALL_PROTOCOL_IMPL ("EFI_OS_INFO_PROTOCOL", EfiOSInfoMain);

  return EFI_SUCCESS;
}
