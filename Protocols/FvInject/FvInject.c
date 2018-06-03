/** @file
  Firmware volume driver that overrides the EFI_FIRMWARE_VOLUME_PROTOCOL
  and injects images for boot.efi/FileVault 2.

  Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>
  Portions copyright (C) 2005-2018, Apple Inc. All rights reserved.<BR>
  Portions copyright (C) 2006-2014 Intel Corporation. All rights reserved.<BR>
  Portions copyright (C) 2016-2018 Alex James. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution. The full text of the license may be
  found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER
  EXPRESS OR IMPLIED.
**/

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <Framework/FirmwareVolumeImageFormat.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolume.h>

#include <Guid/AppleFirmwareVolume.h>

#include "FvInject.h"

EFI_STATUS
EFIAPI
FvInjectGetVolumeAttributes (
  IN  EFI_FIRMWARE_VOLUME_PROTOCOL *This,
  OUT FRAMEWORK_EFI_FV_ATTRIBUTES  *Attributes
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvInjectSetVolumeAttributes (
  IN     EFI_FIRMWARE_VOLUME_PROTOCOL *This,
  IN OUT FRAMEWORK_EFI_FV_ATTRIBUTES  *Attributes
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvInjectReadFile (
  IN     EFI_FIRMWARE_VOLUME_PROTOCOL *This,
  IN     EFI_GUID                     *NameGuid,
  IN OUT VOID                         **Buffer,
  IN OUT UINTN                        *BufferSize,
  OUT    EFI_FV_FILETYPE              *FoundType,
  OUT    EFI_FV_FILE_ATTRIBUTES       *FileAttributes,
  OUT    UINT32                       *AuthenticationStatus
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvInjectReadSection (
  IN     EFI_FIRMWARE_VOLUME_PROTOCOL *This,
  IN     EFI_GUID                     *NameGuid,
  IN     EFI_SECTION_TYPE             SectionType,
  IN     UINTN                        SectionInstance,
  IN OUT VOID                         **Buffer,
  IN OUT UINTN                        *BufferSize,
  OUT    UINT32                       *AuthenticationStatus
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (!Buffer || !BufferSize || !AuthenticationStatus) {
    return EFI_INVALID_PARAMETER;
  }

  if (CompareGuid (NameGuid, &gAppleArrowCursorImageGuid)) {
    *BufferSize = sizeof (mAppleArrowCursorImage);
    Status = gBS->AllocatePool (EfiBootServicesData, *BufferSize, (VOID **)Buffer);
    if (!EFI_ERROR (Status)) {
      gBS->CopyMem (*Buffer, &mAppleArrowCursorImage, *BufferSize);
    }

    *AuthenticationStatus = 0;
    return Status;

  } else if (CompareGuid (NameGuid, &gAppleArrowCursor2xImageGuid)) {
    *BufferSize = sizeof (mAppleArrowCursor2xImage);
    Status = gBS->AllocatePool (EfiBootServicesData, *BufferSize, (VOID **)Buffer);
    if (!EFI_ERROR (Status)) {
      gBS->CopyMem (*Buffer, &mAppleArrowCursor2xImage, *BufferSize);
    }

    *AuthenticationStatus = 0;
    return Status;

  } else if (CompareGuid (NameGuid, &gAppleImageListGuid)) {
    *BufferSize = sizeof (mAppleImageTable);
    Status = gBS->AllocatePool (EfiBootServicesData, *BufferSize, (VOID **)Buffer);
    if (!EFI_ERROR (Status)) {
      gBS->CopyMem (*Buffer, &mAppleImageTable, *BufferSize);
    }

    *AuthenticationStatus = 0;
    return Status;
  }

  *AuthenticationStatus = EFI_LOCAL_AUTH_STATUS_TEST_FAILED;
  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FvInjectWriteFile (
  IN EFI_FIRMWARE_VOLUME_PROTOCOL     *This,
  IN UINT32                           NumberOfFiles,
  IN FRAMEWORK_EFI_FV_WRITE_POLICY    WritePolicy,
  IN FRAMEWORK_EFI_FV_WRITE_FILE_DATA *FileData
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FvInjectGetNextFile (
  IN     EFI_FIRMWARE_VOLUME_PROTOCOL *This,
  IN OUT VOID                         *Key,
  IN OUT EFI_FV_FILETYPE              *FileType,
  OUT    EFI_GUID                     *NameGuid,
  OUT    EFI_FV_FILE_ATTRIBUTES       *Attributes,
  OUT    UINTN                        *Size
  )
{
  return EFI_SUCCESS;
}

EFI_FIRMWARE_VOLUME_PROTOCOL mFirmwareVolume = {
  FvInjectGetVolumeAttributes,
  FvInjectSetVolumeAttributes,
  FvInjectReadFile,
  FvInjectReadSection,
  FvInjectWriteFile,
  FvInjectGetNextFile,
  16,
  NULL
};

EFI_STATUS
EFIAPI
FvInjectMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_FIRMWARE_VOLUME_PROTOCOL *ExistingFirmwareVolume;
  EFI_STATUS                   Status;

  Status = gBS->LocateProtocol (
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  (VOID **)&ExistingFirmwareVolume
                  );

  if (EFI_ERROR (Status)) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gEfiFirmwareVolumeProtocolGuid,
                    &mFirmwareVolume,
                    NULL
                    );
  } else {
    ExistingFirmwareVolume->GetVolumeAttributes = FvInjectGetVolumeAttributes;
    ExistingFirmwareVolume->SetVolumeAttributes = FvInjectSetVolumeAttributes;
    ExistingFirmwareVolume->ReadFile            = FvInjectReadFile;
    ExistingFirmwareVolume->ReadSection         = FvInjectReadSection;
    ExistingFirmwareVolume->WriteFile           = FvInjectWriteFile;
    ExistingFirmwareVolume->GetNextFile         = FvInjectGetNextFile;
  }

  return Status;
}
