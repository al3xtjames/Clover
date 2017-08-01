/** @file
  Helpers for device property injection.

  Copyright (C) 2016-2017 Alex James. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution. The full text of the license may be
  found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER
  EXPRESS OR IMPLIED.
**/

enum {
  DEVICE_PROPERTY_UINT8 = 0,
  DEVICE_PROPERTY_UINT16,
  DEVICE_PROPERTY_UINT32,
  DEVICE_PROPERTY_UINT64,
  DEVICE_PROPERTY_CHAR8
};

typedef struct {
  CHAR16 *Name;
  UINT8  Type;
  VOID   *Value;
  UINT32 Size;
  UINT8  Flags;
} DEVICE_PROPERTY;

VOID
LogInjectionStatus (
  IN DEVICE_PROPERTY *Property,
  IN EFI_STATUS      Status
  );
