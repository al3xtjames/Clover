/** @file
  Copyright (C) 2005 - 2016, Apple Inc. All rights reserved.<BR>
  Portions Copyright (C) 2016 vit9696. All rights reserved.
  Portions Copyright (C) 2014 - 2016, CupertinoNet. All rights reserved.<BR>

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
**/

#ifndef APPLE_USER_INTERFACE_THEME_H_
#define APPLE_USER_INTERFACE_THEME_H_

// APPLE_USER_INTERFACE_THEME_PROTOCOL_GUID
#define APPLE_USER_INTERFACE_THEME_PROTOCOL_GUID          \
  { 0xD5B0AC65, 0x9A2D, 0x4D2A,                           \
    { 0xBB, 0xD6, 0xE8, 0x71, 0xA9, 0x5E, 0x04, 0x35 } }

typedef struct APPLE_USER_INTERFACE_THEME_PROTOCOL APPLE_USER_INTERFACE_THEME_PROTOCOL;

// USER_INTERFACE_THEME_GET_COLOR
typedef
EFI_STATUS
(EFIAPI *USER_INTERFACE_THEME_GET_COLOR)(
  IN OUT UINT32  *Color
);

// APPLE_USER_INTERFACE_THEME_PROTOCOL
struct APPLE_USER_INTERFACE_THEME_PROTOCOL {
  UINT64                         Revision;  ///<
  USER_INTERFACE_THEME_GET_COLOR GetColor;  ///<
};

// gAppleUserInterfaceThemeProtocolGuid
extern EFI_GUID gAppleUserInterfaceThemeProtocolGuid;

#endif // APPLE_USER_INTERFACE_THEME_H_
