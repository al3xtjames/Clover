/** @file
  Copyright (C) 2005 - 2016, Apple Inc.  All rights reserved.
  Portions Copyright (C) 2014 - 2016, CupertinoNet.  All rights reserved.<BR>

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

#ifndef APPLE_SYSTEM_INFO_H_
#define APPLE_SYSTEM_INFO_H_

#include <Guid/DataHubRecords.h>

// APPLE_SYSTEM_INFO_PRODUCER_NAME_GUID
#define APPLE_SYSTEM_INFO_PRODUCER_NAME_GUID              \
  { 0x64517CC8, 0x6561, 0x4051,                           \
    { 0xB0, 0x3C, 0x59, 0x64, 0xB6, 0x0F, 0x4C, 0x7A } }

// APPLE_SYSTEM_INFO_DATA_RECORD
typedef struct {
  EFI_SUBCLASS_TYPE1_HEADER Header;     ///<
  UINT32                    NameSize;   ///<
  UINT32                    ValueSize;  ///<
} APPLE_SYSTEM_INFO_DATA_RECORD;

// gAppleSystemInfoProducerNameGuid
extern EFI_GUID gAppleSystemInfoProducerNameGuid;

#endif // APPLE_SYSTEM_INFO_H_
