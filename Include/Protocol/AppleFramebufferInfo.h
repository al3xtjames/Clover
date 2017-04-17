/** @file
  Copyright (C) 2005 - 2017, Apple Inc.  All rights reserved.
  Portions Copyright (C) 2014 - 2017, CupertinoNet.  All rights reserved.<BR>

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

#ifndef APPLE_FRAMEBUFFER_INFO_H_
#define APPLE_FRAMEBUFFER_INFO_H_

// APPLE_FRAMEBUFFER_INFO_PROTOCOL_GUID
#define APPLE_FRAMEBUFFER_INFO_PROTOCOL_GUID              \
  { 0xE316E100, 0x0751, 0x4C49,                           \
    { 0x90, 0x56, 0x48, 0x6C, 0x7E, 0x47, 0x29, 0x03 } }

typedef struct APPLE_FRAMEBUFFER_INFO_PROTOCOL APPLE_FRAMEBUFFER_INFO_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *FRAMEBUFFER_INFO_GET_INFO)(
  IN  APPLE_FRAMEBUFFER_INFO_PROTOCOL  *This,
  OUT UINT64                           *BaseAddress,
  OUT UINT64                           *FramebufferSize,
  OUT UINT32                           *BytesPerRow,
  OUT UINT32                           *Width,
  OUT UINT32                           *Height,
  OUT UINT32                           *ColorDepth
  );

// APPLE_FRAMEBUFFER_INFO_PROTOCOL
struct APPLE_FRAMEBUFFER_INFO_PROTOCOL {
  UINT64                     Revision             ///<
  FRAMEBUFFER_INFO_GET_INFO  GetFramebufferInfo;  ///<
};

// gAppleFramebufferInfoProtocolGuid
extern EFI_GUID gAppleFramebufferInfoProtocolGuid;

#endif // APPLE_FRAMEBUFFER_INFO_H_
