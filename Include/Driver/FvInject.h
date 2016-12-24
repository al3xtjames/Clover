/** @file
  Firmware volume driver that overrides the EFI_FIRMWARE_VOLUME_PROTOCOL
  and injects images for boot.efi/FileVault 2.

  Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>
  Portions copyright (C) 2005-2016, Apple Inc. All rights reserved.<BR>
  Portions copyright (C) 2006-2014 Intel Corporation. All rights reserved.<BR>
  Portions copyright (C) 2016 Alex James. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution. The full text of the license may be
  found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER
  EXPRESS OR IMPLIED.
 **/

#ifndef FV_INJECT_DRV_H_
#define FV_INJECT_DRV_H_

EFI_STATUS
EFIAPI
FvInjectMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif // FV_INJECT_DRV_H_
