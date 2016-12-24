/** @file
  Implementation of APPLE_IMAGE_CODEC_PROTOCOL.

  Copyright (C) 2005 - 2015, Apple Inc. All rights reserved.<BR>
  Portions Copyright (C) 2012 Tiamo. All rights reserved.<BR>
  Portions Copyright (C) 2014 Damir Mazar. All rights reserved.<BR>
  Portions Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitly or explicitly permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#ifndef APPLE_IMAGE_CODEC_DRV_H_
#define APPLE_IMAGE_CODEC_DRV_H_

EFI_STATUS
EFIAPI
AppleImageCodecMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

#endif // APPLE_IMAGE_CODEC_DRV_H_
