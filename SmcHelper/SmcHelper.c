/** @file
  SMC helper driver that installs APPLE_SMC_IO_PROTOCOL for storing
  and retrieving SMC keys without the presence of a physical SMC.

  Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>
**/

#include <AppleUefi.h>

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemLogLib.h>

#include <Protocol/AppleSmcIo.h>

// DBG_SMC: 0=no debug, 1=serial, 2=console 3=log
// serial requires
// [PcdsFixedAtBuild]
//  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x07
//  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0xFFFFFFFF
// in package DSC file

#define DBG_SMC 3

#if DBG_SMC == 3
#define DBG(...) MemLog (FALSE, 0, __VA_ARGS__)
#elif DBG_SMC == 2
#define DBG(...) AsciiPrint (__VA_ARGS__)
#elif DBG_SMC == 1
#define DBG(...) DebugPrint (1, __VA_ARGS__)
#else
#define DBG(...)
#endif

#define SMC_HELPER_SIGNATURE SIGNATURE_64 ('S', 'M', 'C', 'H', 'E', 'L' ,'P', 'R')

typedef struct _SMC_STACK SMC_STACK;

struct _SMC_STACK {
  SMC_STACK *Next;
  UINT32    Id;
  INTN      DataLen;
  UINT8     *Data;
};

SMC_STACK *mSmcStack = NULL;

STATIC
CHAR8 *
SmcHelperGetSmcKeyStr (
  SMC_KEY Key
  )
{
  CHAR8 *KeyStr;
  KeyStr = AllocatePool(5);

  KeyStr[4] = '\0';
  KeyStr[3] = Key & 0xFF;
  KeyStr[2] = (Key >> 8) & 0xFF;
  KeyStr[1] = (Key >> 16) & 0xFF;
  KeyStr[0] = (Key >> 24) & 0xFF;

  return KeyStr;
}

EFI_STATUS
EFIAPI
SmcHelperReadData (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  IN  SMC_KEY               Key,
  IN  SMC_DATA_SIZE         Size,
  OUT SMC_DATA              *Value)
{
  SMC_STACK *TmpStack = mSmcStack;
  INTN      Len;
  CHAR8     *KeyStr = SmcHelperGetSmcKeyStr (Key);

  DBG ("Reading SMC key %x (%a), Size = %d\n", Key, KeyStr, Size);
  FreePool (KeyStr);

  while (TmpStack) {
    if (TmpStack->Id == Key) {
      Len = MIN (TmpStack->DataLen, Size);
      CopyMem (Value, TmpStack->Data, Len);
      return EFI_SUCCESS;
    } else {
      TmpStack = TmpStack->Next;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
SmcHelperWriteData (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  IN  SMC_KEY               Key,
  IN  UINT32                Size,
  OUT SMC_DATA              *Value
  )
{
  SMC_STACK *TmpStack = mSmcStack;
  INTN      Len;

  // Find existing key
  while (TmpStack) {
    if (TmpStack->Id == Key) {
      Len = MIN (TmpStack->DataLen, Size);
      CopyMem (TmpStack->Data, Value, Len);
      return EFI_SUCCESS;
    } else {
      TmpStack = TmpStack->Next;
    }
  }

  // If not found, create a new stack
  TmpStack = AllocatePool (sizeof (SMC_STACK));
  TmpStack->Next = mSmcStack;
  TmpStack->Id = Key;
  TmpStack->DataLen = Size;
  TmpStack->Data = AllocateCopyPool (Size, Value);
  mSmcStack = TmpStack;

  return EFI_SUCCESS;
}

#if 0
EFI_STATUS
EFIAPI
SmcHelperDumpData (
  IN APPLE_SMC_IO_PROTOCOL* This
  )
{
  INTN Index;
  SMC_STACK *TmpStack = mSmcStack;

  while (TmpStack) {
    CHAR8 *Str = StringId (TmpStack->Id);
    DBG ("found SMC=%x (%a) len=%d data:", TmpStack->Id, Str, TmpStack->DataLen);
    for (Index = 0; Index < TmpStack->DataLen; Index++) {
      DBG ("%02x ", *((UINT8 *)(TmpStack->Data) + Index));
    }

    DBG ("\n");
    FreePool (Str);
    TmpStack = TmpStack->Next;
  }

  return EFI_SUCCESS;
}
#endif

STATIC APPLE_SMC_IO_PROTOCOL mSmcHelper = {
  SMC_HELPER_SIGNATURE,
  SmcHelperReadData,
  SmcHelperWriteData
  // DumpData,
};

/** Entry point for SMC helper.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The operation completed successfully.
  @retval EFI_ALREADY_STARTED  The protocol has already been installed.
**/
EFI_STATUS
EFIAPI
SmcHelperEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS        Status;
  EFI_BOOT_SERVICES *gBS;

  gBS = SystemTable->BootServices;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gAppleSmcIoProtocolGuid,
                  &mSmcHelper,
                  NULL
                  );

  return Status;
}
