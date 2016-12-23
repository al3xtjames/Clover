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

#include <Protocol/UgaDraw.h>
#include <Protocol/AppleImageCodec.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include "picopng.h"

//#define DBG(...) AsciiPrint(__VA_ARGS__);
#define DBG(...)

struct EFI_RES_ENTRY {
  CHAR8  Name[64];
  UINT32 DataOffset;
  UINT32 DataLength;
};

struct EFI_RES {
  UINT16 Magic; // 0x200 (BigEndian) or 0x02 (LE)
  UINT16 Num;   // LE
  struct EFI_RES_ENTRY Entries[1]; //NUM - dynamic array
};

//Fool Proof
/*
 02 00 14 00 61 76 61 74 61 72 2E 70 6E 67 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 00 00 00 00 EC 05 00 00 AF 2D 00 00

 61 76 61 74 61 72 40 32 78 2E 70 6E 67 00 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
 9B 33 00 00 32 84 00 00

 Num=20
 1. avatar.png Offset=0x05EC (LittleEndian) Size=0x2DAF Offset+Size=0x339B
 2. avatar@2x.png Offset=0x339C (!) Aligned Size=0x8432 End=0xB7CE
 3. Offset=0xB7CD = 0x339B+0x8432 Not alinged(!!!) File=0xB7CE Aligned
 */

STATIC
EG_IMAGE *
egDecodeAny (
  IN UINT8   *FileData,
  IN UINTN   FileDataLength,
  IN BOOLEAN WantAlpha)
{
  EG_IMAGE *NewImage;

  // Automatic choose format
  NewImage = egDecodePNG (FileData, FileDataLength, WantAlpha);

  if (!NewImage) {
    DBG (" ..png is wrong try to decode icns\n");
    NewImage = egDecodeICNS (FileData, FileDataLength, 128, WantAlpha);
  }

  if (!NewImage) {
    DBG (" ..png and icns is wrong try to decode bmp\n");
    NewImage = egDecodeBMP (FileData, FileDataLength, WantAlpha);
  }

  return NewImage;
}

//
// PNG Image codec protocol instance implementation
//

EFI_STATUS
EFIAPI
RecognizeImageData (
  IN  VOID  *ImageBuffer,
  IN  UINTN ImageSize,
  OUT VOID  **OutBuffer
  )
{
  EG_IMAGE *Image;

  if (!ImageBuffer) {
    return EFI_INVALID_PARAMETER;
  }

  if (*(UINT16 *)ImageBuffer == 0x02) {
    return EFI_SUCCESS; //this is efires image
  }

  DBG ("AppleImageCodec RecognizeImageData: Status = ");
  Image = egDecodeAny ((UINT8 *)ImageBuffer, ImageSize, FALSE);
  if (Image == NULL) {
    DBG ("EFI_UNSUPPORTED\n");
    return EFI_UNSUPPORTED;
  }

  DBG ("EFI_SUCCESS\n");
  DBG ("ImageBuffer=%p, ImageSize=%d\n", ImageBuffer, ImageSize);
  DBG ("Decoded: W=%d, H=%d\n", Image->Width, Image->Height);

  egFreeImage (Image);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetImageDimensions (
  IN VOID   *ImageBuffer,
  IN UINTN  ImageSize,
  IN UINT32 *ImageWidth,
  IN UINT32 *ImageHeight
  )
{
  EG_IMAGE *Image;

  DBG ("AppleImageCodec GetImageDims: Status = ");
  Image = egDecodeAny ((UINT8 *)ImageBuffer, ImageSize, FALSE);
  if (Image == NULL) {
    DBG ("EFI_UNSUPPORTED\n");
    return EFI_UNSUPPORTED;
  }

  *ImageWidth = (UINT32)Image->Width;
  *ImageHeight = (UINT32)Image->Height;

  DBG ("EFI_SUCCESS, Width=%d, Height=%d\n", *ImageWidth, *ImageHeight);
  DBG ("ImageBuffer=%p, ImageSize=%d\n", ImageBuffer, ImageSize);
  DBG ("ImageWidth=%p, ImageHeight=%p\n", ImageWidth, ImageHeight);
  DBG ("Decoded: W=%d, H=%d\n", Image->Width, Image->Height);

  egFreeImage (Image);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
DecodeImageData (
  IN VOID          *ImageBuffer,
  IN UINTN         ImageSize,
  IN EFI_UGA_PIXEL **RawImageData,
  IN UINT64        *RawImageDataSize
  )
{
  EFI_STATUS Status;

  EG_IMAGE *Image;
  INTN     Index;

  // Automatic choose format
  if (!RawImageData || !RawImageDataSize) {
    return EFI_INVALID_PARAMETER;
  }

  DBG ("AppleImageCodec DecodeImageData: Status = ");
  Image = egDecodeAny ((UINT8 *)ImageBuffer, ImageSize, FALSE);
  if (Image == NULL) {
    DBG ("EFI_UNSUPPORTED\n");
    return EFI_UNSUPPORTED;
  }

  *RawImageDataSize = (UINT32)Image->Width * Image->Height * sizeof (EFI_UGA_PIXEL);
  Status = gBS->AllocatePool (EfiBootServicesData, *RawImageDataSize, (VOID **)RawImageData);
  if (!EFI_ERROR (Status)) {
    gBS->CopyMem (*RawImageData, (VOID *)Image->PixelData, *RawImageDataSize);
  }

  DBG ("EFI_SUCCESS, RawImageDataSize=%d\n", *RawImageDataSize);
  DBG ("ImageBuffer=%p, ImageSize=%d\n", ImageBuffer, ImageSize);
  DBG ("Decoded: W=%d, H=%d\n", Image->Width, Image->Height);
  for (Index = 0; Index < 10; Index++) {
    DBG (
      "P%d: r,g,b,a= %x, %x, %x, %x\n",
      Index,
      (*RawImageData)[Index].Red,
      (*RawImageData)[Index].Green,
      (*RawImageData)[Index].Blue,
      (*RawImageData)[Index].Reserved
      );
  }

  egFreeImage (Image);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Unknown1 (
  IN VOID  *ImageBuffer,
  IN UINTN Ukn1,
  IN UINTN Ukn2,
  IN UINTN Ukn3
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Unknown2 (
  IN VOID  *ImageBuffer,
  IN UINTN Ukn1,
  IN UINTN Ukn2,
  IN UINTN Ukn3
  )
{
  return EFI_SUCCESS;
}

STATIC APPLE_IMAGE_CODEC_PROTOCOL mAppleImageCodec = {
  1, // Revision
  0, // FileExtension
  RecognizeImageData,
  GetImageDimensions,
  DecodeImageData,
  Unknown1,
  Unknown2,
};

/** Driver's entry point. Installs our StartImage to detect boot loader start. */
EFI_STATUS
EFIAPI
AppleImageCodecMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS Status;

  //
  // Install instance of Apple image codec protocol for
  // PNG files
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gAppleImageCodecProtocolGuid,
                  &mAppleImageCodec,
                  NULL
                  );

  if (EFI_ERROR (Status)) {
    DBG ("AppleImageCodec: error installing protocol, Status = %r\n", Status);
  }

  return Status;
}
