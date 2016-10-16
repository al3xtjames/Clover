/** @file
  Apple UEFI protocols.
**/

#include "Platform.h"

//
// Legacy device property variables for Settings.c.
//
UINT32 mPropSize = 0;
UINT8  *mProperties = NULL;
CHAR8  *gDeviceProperties = NULL;

UINT32 cPropSize = 0;
UINT8  *cProperties = NULL;
CHAR8  *cDeviceProperties = NULL;

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

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathPropertyDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&gEfiDppDbProtocol
                  );

  if (EFI_ERROR (Status)) {
    MsgLog ("Installing EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL");
    // TODO: Install the AppleModulePkg implementation for the protocol
    // EfiDevicePathPropertyDatabaseMain ();
  }

  // TODO: Do the same for the other protocols

  return Status;
}
