/** @file
  SMC helper driver that installs APPLE_SMC_IO_PROTOCOL for storing
  and retrieving SMC keys without the presence of a physical SMC.

  Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>

  Have no idea about licenses. Keep copyright and use with any modifications.
**/

#ifndef SMC_HELPER_DRV_H_
#define SMC_HELPER_DRV_H_

EFI_STATUS
EFIAPI
SmcHelperMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif // SMC_HELPER_DRV_H_
