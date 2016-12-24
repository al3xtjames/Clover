/** @file
  Copyright (C) 2005-2016, Apple Inc. All rights reserved.
  Portions copyright (C) 2016 Sergey Slice. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made
  available under the terms and conditions of the BSD License which
  accompanies this distribution. The full text of the license may be
  found at http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS"
  BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER
  EXPRESS OR IMPLIED.
**/

#ifndef APPLE_GRAPHICS_CONFIGURATION_H_
#define APPLE_GRAPHICS_CONFIGURATION_H_

// APPLE_GRAPHICS_CONFIGURATION_PROTOCOL_GUID
#define APPLE_GRAPHICS_CONFIGURATION_PROTOCOL_GUID        \
  { 0x03622D6D, 0x362A, 0x4E47,                           \
    { 0x97, 0x10, 0xC2, 0x38, 0xB2, 0x37, 0x55, 0xC1 } }

typedef struct APPLE_GRAPHICS_CONFIGURATION_PROTOCOL APPLE_GRAPHICS_CONFIGURATION_PROTOCOL;

// RESTORE_CONFIGURATION
typedef
EFI_STATUS
(EFIAPI *GRAPHICS_CONFIG_RESTORE_CONFIGURATION)(
  IN APPLE_GRAPHICS_CONFIGURATION_PROTOCOL *This,
  IN UINT32                                Ukn1,
  IN UINT32                                Ukn2,
  IN VOID                                  *Ukn3,
  IN VOID                                  *Ukn4,
  IN VOID                                  *Ukn5
  );

// APPLE_GRAPHICS_CONFIGURATION_PROTOCOL
struct APPLE_GRAPHICS_CONFIGURATION_PROTOCOL
{
  UINT64                                Revision;             ///<
  GRAPHICS_CONFIG_RESTORE_CONFIGURATION RestoreConfiguration; ///<
};

// gAppleGraphicsConfigurationProtocolGuid
extern EFI_GUID gAppleGraphicsConfigurationProtocolGuid;

#endif // APPLE_GRAPHICS_CONFIGURATION_H_
