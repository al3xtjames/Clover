/** @file
  Copyright (C) 2005 - 2016, Apple Inc. All rights reserved.<BR>
  Portions Copyright (C) 2012 Tiamo. All rights reserved.<BR>
  Portions Copyright (C) 2014 Damir Mazar. All rights reserved.<BR>
  Portions Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>
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

#ifndef APPLE_IMAGE_CODEC_H_
#define APPLE_IMAGE_CODEC_H_

// APPLE_IMAGE_CODEC_PROTOCOL_GUID
#define APPLE_IMAGE_CODEC_PROTOCOL_GUID                   \
  { 0x0DFCE9F6, 0xC4E3, 0x45EE,                           \
    { 0xA0, 0x6A, 0xA8, 0x61, 0x3B, 0x98, 0xA5, 0x07 } }

typedef struct APPLE_IMAGE_CODEC_PROTOCOL APPLE_IMAGE_CODEC_PROTOCOL;

// IMAGE_CODEC_RECOGNIZE_IMAGE_DATA
typedef
EFI_STATUS
(EFIAPI *IMAGE_CODEC_RECOGNIZE_IMAGE_DATA)(
  IN  VOID   *ImageBuffer,
  IN  UINTN  ImageSize,
  OUT VOID   **OutBuffer
  );

// IMAGE_CODEC_GET_IMAGE_DIMENSIONS
typedef
EFI_STATUS
(EFIAPI *IMAGE_CODEC_GET_IMAGE_DIMENSIONS)(
  IN VOID    *ImageBuffer,
  IN UINTN   ImageSize,
  IN UINT32  *ImageWidth,
  IN UINT32  *ImageHeight
  );

// IMAGE_CODEC_DECODE_IMAGE_DATA
typedef
EFI_STATUS
(EFIAPI *IMAGE_CODEC_DECODE_IMAGE_DATA)(
  IN VOID           *ImageBuffer,
  IN UINTN          ImageSize,
  IN EFI_UGA_PIXEL  **RawImageData,
  IN UINT64         *RawImageDataSize
  );

// IMAGE_CODEC_UNKNOWN_IMAGE_DATA_1
typedef
EFI_STATUS
(EFIAPI *IMAGE_CODEC_UNKNOWN_IMAGE_DATA_1)(
  IN VOID   *ImageBuffer,
  IN UINTN  Ukn1,
  IN UINTN  Ukn2,
  IN UINTN  Ukn33
  );

// IMAGE_CODEC_UNKNOWN_IMAGE_DATA_2
typedef
EFI_STATUS
(EFIAPI *IMAGE_CODEC_UNKNOWN_IMAGE_DATA_2)(
  IN VOID   *ImageBuffer,
  IN UINTN  Ukn1,
  IN UINTN  Ukn2,
  IN UINTN  Ukn3
  );

// APPLE_IMAGE_CODEC_PROTOCOL
struct APPLE_IMAGE_CODEC_PROTOCOL
{
  UINT64                           Revision;            ///<
  UINT64                           FileExtension;       ///<
  IMAGE_CODEC_RECOGNIZE_IMAGE_DATA RecognizeImageData;  ///<
  IMAGE_CODEC_GET_IMAGE_DIMENSIONS GetImageDimensions;  ///<
  IMAGE_CODEC_DECODE_IMAGE_DATA    DecodeImageData;     ///<
  IMAGE_CODEC_UNKNOWN_IMAGE_DATA_1 Unknown1;            ///<
  IMAGE_CODEC_UNKNOWN_IMAGE_DATA_2 Unknown2;            ///<
};

// gAppleImageCodecProtocolGuid
extern EFI_GUID gAppleImageCodecProtocolGuid;

#endif // APPLE_IMAGE_CODEC_H_
