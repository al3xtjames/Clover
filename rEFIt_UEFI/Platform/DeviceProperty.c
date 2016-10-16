#include "Platform.h"

//
// Legacy vars for Settings.c
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

EFI_STATUS
InstallAppleProtocols (VOID)
{
  EFI_STATUS Status;

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathPropertyDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&gEfiDppDbProtocol
                  );

  if (EFI_ERROR (Status)) {
    // TODO: Install the AppleModulePkg implementation for the protocol
    MsgLog ("EFI_DEVICE_PATH_PROPERTY_DATABASE_PROTOCOL not found");
  }

  // TODO: Do the same for the other protocols

  return Status;
}
