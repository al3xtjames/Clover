/** @file
  Created by Slice on 26.10.16. Based on CupertinoNet.
  Copyright (C) 2005 - 2015, Apple Inc.  All rights reserved.<BR>

  This program and the accompanying materials have not been licensed.
  Neither is its usage, its redistribution, in source or binary form,
  licensed, nor implicitely or explicitely permitted, except when
  required by applicable law.

  Unless required by applicable law or agreed to in writing, software
  distributed is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
  OR CONDITIONS OF ANY KIND, either express or implied.
**/

#ifndef APPLE_KEY_MAP_AGGREGATOR_PRIVATE_H_
#define APPLE_KEY_MAP_AGGREGATOR_PRIVATE_H_

#define APPLE_KEY_MAP_PROTOCOLS_REVISION   0x010000
#define APPLE_KEY_MAP_AGGREGATOR_SIGNATURE SIGNATURE_32 ('K', 'e', 'y', 'A')

#define APPLE_KEY_STROKES_INFO_SIGNATURE   SIGNATURE_32 ('K', 'e', 'y', 'S')

#define APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_AGGREGATOR(This) \
  CR (                                                              \
    (This),                                                         \
    APPLE_KEY_MAP_AGGREGATOR,                                       \
    AggregatorProtocol,                                             \
    APPLE_KEY_MAP_AGGREGATOR_SIGNATURE                              \
  )

#define APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_DATABASE(This)   \
  CR (                                                              \
    (This),                                                         \
    APPLE_KEY_MAP_AGGREGATOR,                                       \
    DatabaseProtocol,                                               \
    APPLE_KEY_MAP_AGGREGATOR_SIGNATURE                              \
  )

#define APPLE_KEY_STROKES_INFO_FROM_LIST_ENTRY(Entry)  \
((APPLE_KEY_STROKES_INFO *)(                         \
  CR (                                               \
    (Entry),                                         \
    APPLE_KEY_STROKES_INFO_HDR,                      \
    This,                                            \
    APPLE_KEY_STROKES_INFO_SIGNATURE                 \
  )                                                \
))

// APPLE_KEY_STROKES_INFO_HDR
typedef struct {
  UINTN              Signature;      ///<
  LIST_ENTRY         This;           ///<
  UINTN              Index;          ///<
  UINTN              KeyBufferSize;  ///<
  UINTN              NumberOfKeys;   ///<
  APPLE_MODIFIER_MAP Modifiers;      ///<
} APPLE_KEY_STROKES_INFO_HDR;

// APPLE_KEY_STROKES_INFO
typedef struct {
  APPLE_KEY_STROKES_INFO_HDR Hdr;   ///<
  APPLE_KEY                  Keys;  ///<
} APPLE_KEY_STROKES_INFO;

// APPLE_KEY_MAP_AGGREGATOR
typedef struct {
  UINTN                             Signature;           ///<0
  UINTN                             NextKeyStrokeIndex;  ///<0x08
  APPLE_KEY                         *KeyBuffer;          ///<0x10
  UINTN                             KeyBuffersSize;      ///<0x18
  LIST_ENTRY                        KeyStrokesInfoList;  ///<0x20
  APPLE_KEY_MAP_DATABASE_PROTOCOL   DatabaseProtocol;    ///<0x30 size=8*4
  APPLE_KEY_MAP_AGGREGATOR_PROTOCOL AggregatorProtocol;  ///<0x50 size=8*3
} APPLE_KEY_MAP_AGGREGATOR;                              ///size=0x68

#endif // APPLE_KEY_MAP_AGGREGATOR_PRIVATE_H_
