/** @file
  SMC helper driver that installs APPLE_SMC_IO_PROTOCOL for storing
  and retrieving SMC keys without the presence of a physical SMC.

  Copyright (C) 2016 Sergey Slice. All rights reserved.<BR>

  Have no idea about licenses. Keep copyright and use with any modifications.
**/

#include <Library/BaseMemoryLib.h>
#include <Library/MemLogLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/AppleSmcIo.h>

#define DBG(...) MemLog (FALSE, 0, __VA_ARGS__)

#define SMC_HELPER_SIGNATURE SIGNATURE_64 ('E', 'm', 'u', 'l', 'a', 't', 'o', 'r')

typedef struct SMC_STACK SMC_STACK;

struct SMC_STACK {
  SMC_STACK          *Next;
  SMC_KEY            Key;
  SMC_KEY_TYPE       Type;
  SMC_DATA_SIZE      DataLen;
  SMC_KEY_ATTRIBUTES Attributes;
  SMC_DATA           *Data;
};

SMC_STACK *mSmcStack = NULL;

STATIC
CHAR8 *
GetSmcKeyStr (
  IN SMC_KEY Key
)
{
  CHAR8 *SmcKeyString;
  SmcKeyString = AllocatePool (5);

  SmcKeyString[4] = '\0';
  SmcKeyString[3] = Key & 0xFF;
  SmcKeyString[2] = (Key >> 8) & 0xFF;
  SmcKeyString[1] = (Key >> 16) & 0xFF;
  SmcKeyString[0] = (Key >> 24) & 0xFF;

  return SmcKeyString;
}

EFI_STATUS
EFIAPI
SmcHelperReadValue (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  IN  SMC_KEY               Key,
  IN  SMC_DATA_SIZE         Size,
  OUT SMC_DATA              *Value
  )
{
  SMC_STACK *TmpStack = mSmcStack;
  INTN Len;
  CHAR8 *SmcKeyString;

  if (!Value || !Size) {
    return EFI_BUFFER_TOO_SMALL;
  }

  SmcKeyString = GetSmcKeyStr (Key);
  DBG ("Reading SMC key %x (%a), Size = %d\n", Key, SmcKeyString, Size);
  FreePool (SmcKeyString);

  while (TmpStack) {
    if (TmpStack->Key == Key) {
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
SmcHelperWriteValue (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  IN  SMC_KEY               Key,
  IN  UINT32                Size,
  OUT SMC_DATA              *Value
  )
{
  SMC_STACK *TmpStack = mSmcStack;
  UINTN Len;

  //First find existing key
  while (TmpStack) {
    if (TmpStack->Key == Key) {
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
  TmpStack->Key = Key;
  TmpStack->DataLen = Size;
  TmpStack->Attributes = SMC_KEY_ATTRIBUTE_WRITE | SMC_KEY_ATTRIBUTE_READ;
  TmpStack->Data = AllocateCopyPool (Size, Value);
  TmpStack->Type = SmcKeyTypeFlag;
  mSmcStack = TmpStack;

  return EFI_SUCCESS;
}

// SmcIoSmcGetKeyCount
EFI_STATUS
EFIAPI
SmcHelperGetKeyCount (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  OUT UINT32                *Count
  )
{
  UINT32 Index = 0;
  UINT32 *Big  = Count;
  SMC_STACK *TmpStack = mSmcStack;

  if (!Count) {
    return EFI_INVALID_PARAMETER;
  }

  while (TmpStack) {
    Index++;
    TmpStack = TmpStack->Next;
  }

  // Take Big endian into account
  *Big++ = Index >> 24;
  *Big++ = Index >> 16;
  *Big++ = Index >> 8;
  *Big++ = Index >> 0;

  return EFI_SUCCESS;
}

#if 0
EFI_STATUS
EFIAPI
SmcHelperMakeKey (
  IN  CHAR8   *Name,
  OUT SMC_KEY *Key
  )
{
  if (!Key || !Name) {
    return EFI_INVALID_PARAMETER;
  }

  *Key = SMC_MAKE_IDENTIFIER (Name[0], Name[1], Name[2], Name[3]);

  return EFI_SUCCESS;
}
#endif

EFI_STATUS
EFIAPI
SmcHelperAddKey (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN SMC_KEY               Key,
  IN SMC_DATA_SIZE         Size,
  IN SMC_KEY_TYPE          Type,
  IN SMC_KEY_ATTRIBUTES    Attributes
  )
{
  SMC_STACK *TmpStack;

  TmpStack = AllocatePool (sizeof (SMC_STACK));
  TmpStack->Next = mSmcStack;
  TmpStack->Key = Key;
  TmpStack->DataLen = Size;
  TmpStack->Attributes = Attributes;
  TmpStack->Data = AllocatePool (Size);
  TmpStack->Type = Type;
  mSmcStack = TmpStack;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperGetKeyFromIndex (
  IN  APPLE_SMC_IO_PROTOCOL  *This,
  IN  SMC_INDEX              Index,
  OUT SMC_KEY                *Key
  )
{
  UINT32 Num = 0;
  SMC_STACK *TmpStack = mSmcStack;

  if (!Key) {
    return EFI_INVALID_PARAMETER;
  }

  while (TmpStack) {
    if (Num == Index) {
      *Key = TmpStack->Key;
      return EFI_SUCCESS;
    }

    Num++;
    TmpStack = TmpStack->Next;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
SmcHelperGetKeyInfo (
  IN  APPLE_SMC_IO_PROTOCOL *This,
  IN  SMC_KEY               Key,
  OUT SMC_DATA_SIZE         *Size,
  OUT SMC_KEY_TYPE          *Type,
  OUT SMC_KEY_ATTRIBUTES    *Attributes
  )
{
  UINT32 Num = 0;
  SMC_STACK *TmpStack = mSmcStack;

  if (!Size || !Type || !Attributes) {
    return EFI_INVALID_PARAMETER;
  }

  while (TmpStack) {
    if (Key == TmpStack->Key) {
      *Size = TmpStack->DataLen;
      *Type = TmpStack->Type;
      *Attributes = TmpStack->Attributes;
      return EFI_SUCCESS;
    }

    Num++;
    TmpStack = TmpStack->Next;
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
SmcHelperReset (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINT32                Mode
  )
{
  SMC_STACK *TmpStack = mSmcStack;

  if (Mode) {
    while (TmpStack) {
      FreePool (TmpStack->Data);
      mSmcStack = TmpStack->Next;
      FreePool (TmpStack);
      TmpStack = mSmcStack;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperFlashType (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINT32                Type
  )
{
  return EFI_SUCCESS; // EFI_NOT_IMPLEMENTED;
}

EFI_STATUS
EFIAPI
SmcHelperFlashWrite (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINT32                Unknown,
  IN UINT32                Size,
  IN SMC_DATA              *Data
  )
{
  return EFI_SUCCESS; // EFI_NOT_IMPLEMENTED;
}

EFI_STATUS
EFIAPI
SmcHelperFlashAuth (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINT32                Size,
  IN SMC_DATA              *Data
  )
{
  return EFI_SUCCESS; // EFI_NOT_IMPLEMENTED;
}

EFI_STATUS
EFIAPI
SmcHelperUnsupported (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
SmcHelperUnknown1 (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperUnknown2 (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINTN                 Ukn1,
  IN UINTN                 Ukn2
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperUnknown3 (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINTN                 Ukn1,
  IN UINTN                 Ukn2
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperUnknown4 (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINTN                 Ukn1
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmcHelperUnknown5 (
  IN APPLE_SMC_IO_PROTOCOL *This,
  IN UINTN                 Ukn1
  )
{
  return EFI_SUCCESS;
}

APPLE_SMC_IO_PROTOCOL mSmcHelper = {
  SMC_HELPER_SIGNATURE,
  SmcHelperReadValue,
  SmcHelperWriteValue,
  SmcHelperGetKeyCount,
  // SmcHelperMakeKey,
  SmcHelperAddKey,
  SmcHelperGetKeyFromIndex,
  SmcHelperGetKeyInfo,
  SmcHelperReset,
  SmcHelperFlashType,
  SmcHelperUnsupported,
  SmcHelperFlashWrite,
  SmcHelperFlashAuth,
  0,
  SMC_PORT_BASE,
  FALSE,
  SmcHelperUnknown1,
  SmcHelperUnknown2,
  SmcHelperUnknown3,
  SmcHelperUnknown4,
  SmcHelperUnknown5
};

/** Entry point for SMC helper.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The operation completed successfully.
  @retval EFI_ALREADY_STARTED  The protocol has already been installed.
**/
EFI_STATUS
EFIAPI
SmcHelperMain (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gAppleSmcIoProtocolGuid,
                  &mSmcHelper,
                  NULL
                  );

  DBG ("Installed APPLE_SMC_IO_PROTOCOL, Status = %r\n", Status);

  return Status;
}
