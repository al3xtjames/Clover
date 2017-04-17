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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

#include <IndustryStandard/AppleHid.h>

#include <Protocol/AppleEvent.h>
#include <Protocol/AppleKeyMapAggregator.h>
#include <Protocol/AppleKeyMapDatabase.h>

#include "AppleKeyMapAggregator.h"

extern EFI_GUID gAppleEventProtocolGuid;

STATIC APPLE_EVENT_PROTOCOL mAppleEventProtocol = {
  1,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};

// KeyMapGetKeyStrokesByIndex
APPLE_KEY_STROKES_INFO *
KeyMapGetKeyStrokesByIndex (
  IN LIST_ENTRY  *List,
  IN UINTN       Index
  ) // sub_459
{
  APPLE_KEY_STROKES_INFO *KeyStrokesInfo;

  KeyStrokesInfo = APPLE_KEY_STROKES_INFO_FROM_LIST_ENTRY (
                     GetFirstNode (List)
                     );

  while (KeyStrokesInfo->Hdr.Index != Index) {
    if (IsNull (List, &KeyStrokesInfo->Hdr.This)) {
      KeyStrokesInfo = NULL;

      break;
    }

    KeyStrokesInfo = APPLE_KEY_STROKES_INFO_FROM_LIST_ENTRY (
                       GetNextNode (List, &KeyStrokesInfo->Hdr.This)
                       );
  }

  return KeyStrokesInfo;
}

// KeyMapMinSort
VOID
KeyMapMinSort (
  IN OUT UINT16  *Operand,
  IN     UINTN   NumberOfChilds
  ) // sub_72C
{
  UINTN  NoRemainingChilds;
  UINTN  Index;
  UINTN  NoRemainingChilds2;
  UINT16 *OperandPtr;
  UINT16 FirstChild;

  if (!Operand || !NumberOfChilds) {
    return;
  }

  if (Operand != NULL) {
    ++Operand;
    NoRemainingChilds = (NumberOfChilds - 1);
    Index             = 1;

    do {
      NoRemainingChilds2 = NoRemainingChilds;
      OperandPtr         = Operand;

      if (Index < NumberOfChilds) {
        do {
          FirstChild = Operand[-1];

          if (FirstChild > *OperandPtr) {
            Operand[-1] = *OperandPtr;
            *OperandPtr = FirstChild;
          }

          ++OperandPtr;
          --NoRemainingChilds2;
        } while (NoRemainingChilds2 > 0);
      }

      ++Index;
      ++Operand;
    } while ((NoRemainingChilds--) > 0);
  }
}

// KeyMapCreateKeyStrokesBuffer
/** Creates a new key set with a given number of keys allocated.  The index
    within the database is returned.

  @param[in]  This           A pointer to the protocol instance.
  @param[in]  KeyBufferSize  The amount of keys to allocate for the key set.
  @param[out] Index          The assigned index of the created key set.

  @return                       Returned is the status of the operation.
  @retval EFI_SUCCESS           A key set with the given number of keys
                                allocated has been created.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval other                 An error returned by a sub-operation.
**/
EFI_STATUS
EFIAPI
KeyMapCreateKeyStrokesBuffer (
  IN  APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN  UINTN                            KeyBufferSize,
  OUT UINTN                            *Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  UINTN                    BufferSize;
  APPLE_KEY                *Memory;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  if (!This || !Index) {
    return EFI_INVALID_PARAMETER;
  }

  Aggregator = APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_DATABASE (This);

  if (Aggregator->KeyBuffer != NULL) {
    gBS->FreePool ((VOID *)Aggregator->KeyBuffer);
  }

  BufferSize                 = (Aggregator->KeyBuffersSize + KeyBufferSize);
  Aggregator->KeyBuffersSize = BufferSize;
  Memory                     = AllocateZeroPool (BufferSize);
  Aggregator->KeyBuffer      = Memory;
  Status                     = EFI_OUT_OF_RESOURCES;

  if (Memory != NULL) {
    KeyStrokesInfo = AllocateZeroPool (
                       sizeof (APPLE_KEY_STROKES_INFO)
                         + (KeyBufferSize * sizeof (APPLE_KEY))
                       );

    Status         = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo != NULL) {
      KeyStrokesInfo->Hdr.Signature     = APPLE_KEY_STROKES_INFO_SIGNATURE;
      KeyStrokesInfo->Hdr.KeyBufferSize = KeyBufferSize;
      KeyStrokesInfo->Hdr.Index         = Aggregator->NextKeyStrokeIndex;
      ++Aggregator->NextKeyStrokeIndex;

      InsertTailList (
        &Aggregator->KeyStrokesInfoList,
        &KeyStrokesInfo->Hdr.This
        );

      Status = EFI_SUCCESS;
      *Index = KeyStrokesInfo->Hdr.Index;
    }
  }

  return Status;
}

// KeyMapRemoveKeyStrokesBuffer
/** Removes a key set specified by its index from the database.

  @param[in] This   A pointer to the protocol instance.
  @param[in] Index  The index of the key set to remove.

  @return                Returned is the status of the operation.
  @retval EFI_SUCCESS    The specified key set has been removed.
  @retval EFI_NOT_FOUND  No key set could be found for the given index.
  @retval other          An error returned by a sub-operation.
**/
EFI_STATUS
EFIAPI
KeyMapRemoveKeyStrokesBuffer (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  if (!This) {
    return EFI_INVALID_PARAMETER;
  }

  Aggregator     = APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_DATABASE (This);
  KeyStrokesInfo = KeyMapGetKeyStrokesByIndex (
                     &Aggregator->KeyStrokesInfoList, Index
                     );

  Status         = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Aggregator->KeyBuffersSize -= KeyStrokesInfo->Hdr.KeyBufferSize;

    RemoveEntryList (&KeyStrokesInfo->Hdr.This);
    gBS->FreePool ((VOID *)KeyStrokesInfo);

    Status = EFI_SUCCESS;
  }

  return Status;
}

// KeyMapSetKeyStrokeBufferKeysI
/** Sets the keys of a key set specified by its index to the given Keys Buffer.

  @param[in] This          A pointer to the protocol instance.
  @param[in] Index         The index of the key set to edit.
  @param[in] Modifiers     The key modifiers manipulating the given keys.
  @param[in] NumberOfKeys  The number of keys contained in Keys.
  @param[in] Keys          An array of keys to add to the specified key set.

  @return                       Returned is the status of the operation.
  @retval EFI_SUCCESS           The given keys were set for the specified key
                                set.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval EFI_NOT_FOUND         No key set could be found for the given index.
  @retval other                 An error returned by a sub-operation.
**/
//->SetKeyStrokeBufferKeys => Index=3000, Modifiers=0, NoKeys=0, Keys={5518, 0}, Status=Success
EFI_STATUS
EFIAPI
KeyMapSetKeyStrokeBufferKeys (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index,
  IN APPLE_MODIFIER_MAP               Modifiers,
  IN UINTN                            NumberOfKeys,
  IN APPLE_KEY                        *Keys
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  if (!This || !Keys) {
    return EFI_INVALID_PARAMETER;
  }

  Aggregator     = APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_DATABASE (This);
  KeyStrokesInfo = KeyMapGetKeyStrokesByIndex (&Aggregator->KeyStrokesInfoList, Index);
  Status         = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Status = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo->Hdr.KeyBufferSize >= NumberOfKeys) {
      KeyStrokesInfo->Hdr.NumberOfKeys = NumberOfKeys;
      KeyStrokesInfo->Hdr.Modifiers    = Modifiers;

      CopyMem (
        (VOID *)&KeyStrokesInfo->Keys,
        (VOID *)Keys,
        (NumberOfKeys * sizeof(APPLE_KEY))
        );

      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
GetKeyStrokes (
  IN  APPLE_KEY_MAP_AGGREGATOR_PROTOCOL  *This,
  OUT UINT16                             *ModifyFlags,
  OUT UINTN                              *PressedKeyCount,
  OUT APPLE_KEY                          *Keys
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;
  APPLE_MODIFIER_MAP       DbModifiers;
  BOOLEAN                  Result;
  UINTN                    DbNoKeyStrokes;
  UINTN                    Index;
  UINTN                    Index2;
  APPLE_KEY                Key;

  if (!This || !ModifyFlags || !PressedKeyCount) {
    return EFI_INVALID_PARAMETER;
  }

  Aggregator     = APPLE_KEY_MAP_AGGREGATOR_PRIVATE_FROM_AGGREGATOR (This);
  KeyStrokesInfo = APPLE_KEY_STROKES_INFO_FROM_LIST_ENTRY (
                     GetFirstNode (&Aggregator->KeyStrokesInfoList)
                     );

  Result         = IsNull (
                     &Aggregator->KeyStrokesInfoList, &KeyStrokesInfo->Hdr.This
                     );

  if (Result) {
    *PressedKeyCount  = 0;
    DbNoKeyStrokes    = 0;
    DbModifiers       = 0;
  } else {
    DbModifiers       = 0;
    DbNoKeyStrokes    = 0;

    do {
      DbModifiers |= KeyStrokesInfo->Hdr.Modifiers;
      if (KeyStrokesInfo->Hdr.NumberOfKeys > 0) {
        Index = 0;
        do {
          Key = (&KeyStrokesInfo->Keys)[Index];
          ++Index;
          for (Index2 = 0; Index2 < DbNoKeyStrokes; ++Index2) {
            if (Aggregator->KeyBuffer[Index2] == Key) {
              break;
            }
          }
          if (DbNoKeyStrokes == Index2) {
            Aggregator->KeyBuffer[DbNoKeyStrokes] = Key;
            ++DbNoKeyStrokes;
          }
        } while (Index < KeyStrokesInfo->Hdr.NumberOfKeys);
      }

      KeyStrokesInfo = APPLE_KEY_STROKES_INFO_FROM_LIST_ENTRY (
                         GetNextNode (
                           &Aggregator->KeyStrokesInfoList,
                           &KeyStrokesInfo->Hdr.This
                           )
                         );

      Result         = IsNull (
                         &Aggregator->KeyStrokesInfoList,
                         &KeyStrokesInfo->Hdr.This
                         );
    } while (!Result);

    Result  = (DbNoKeyStrokes > *PressedKeyCount);
    *PressedKeyCount = DbNoKeyStrokes;
    Status  = EFI_BUFFER_TOO_SMALL;

    if (Result) {
      return Status;
    }
  }

  *ModifyFlags = DbModifiers;
  *PressedKeyCount    = DbNoKeyStrokes;
  Status     = EFI_SUCCESS;

  if (Keys != NULL) {
    gBS->CopyMem ((VOID *)Keys,
           (VOID *)Aggregator->KeyBuffer,
           (DbNoKeyStrokes * sizeof(APPLE_KEY))
           );
  }

  return Status;
}

#define DB_KEYS_NUM 8

EFI_STATUS
EFIAPI
ContainsKeyStrokes (
  IN     APPLE_KEY_MAP_AGGREGATOR_PROTOCOL  *This,
  IN     UINT16                             ModifyFlags,
  IN     UINTN                              PressedKeyCount,
  IN OUT APPLE_KEY                          *Keys,
  IN     BOOLEAN                            ExactMatch
  )
{
  EFI_STATUS         Status;

  UINTN              DbNoKeys;
  APPLE_KEY          DbKeys[DB_KEYS_NUM];
  APPLE_MODIFIER_MAP DbModifiers;
  INTN               Result;
  UINTN              Index;
  UINTN              DbIndex;

  if (!This || !Keys) {
    return EFI_INVALID_PARAMETER;
  }

  if (!PressedKeyCount) {
    return EFI_NOT_FOUND;
  }

  DbNoKeys = DB_KEYS_NUM;
  Status   = This->GetKeyStrokes (This, &DbModifiers, &DbNoKeys, DbKeys);

  if (!EFI_ERROR (Status)) {
    if (ExactMatch) {
      Status = EFI_NOT_FOUND;

      if ((DbModifiers == ModifyFlags) && (DbNoKeys == PressedKeyCount)) {
        KeyMapMinSort ((UINT16 *)Keys, PressedKeyCount);
        KeyMapMinSort ((UINT16 *)DbKeys, DbNoKeys);

        Result = CompareMem (
                   (VOID *)Keys,
                   (VOID *)DbKeys,
                   (PressedKeyCount * sizeof (APPLE_KEY))
                   );

        if (Result == 0) {
          Status = EFI_SUCCESS;
        }
      }
    } else {
      Status = EFI_NOT_FOUND;

      if ((DbModifiers & ModifyFlags) == ModifyFlags) {
        for (Index = 0; Index < PressedKeyCount; ++Index) {
          for (DbIndex = 0; DbIndex < DbNoKeys; ++DbIndex) {
            if (Keys[Index] == DbKeys[DbIndex]) {
              break;
            }
          }
          if (DbNoKeys == DbIndex) {
            break;
          }
          Status = EFI_SUCCESS;
        }
      }
    }
  }
  return Status;
}

// AggregatorEntryPoint
/**

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS          The entry point is executed successfully.
  @retval EFI_ALREADY_STARTED  The protocol has already been installed.
**/
EFI_STATUS
EFIAPI
AggregatorEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  ) // start
{
  EFI_STATUS               Status;

  UINTN                    NumberHandles;
  EFI_HANDLE               *Buffer;
  APPLE_KEY_MAP_AGGREGATOR *Aggregator;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gAppleKeyMapDatabaseProtocolGuid,
                  NULL,
                  &NumberHandles,
                  &Buffer
                  );

  if (!EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;

    if (Buffer != NULL) {
      gBS->FreePool ((VOID *)Buffer);
    }
  } else {
    Aggregator                                          = AllocateZeroPool (sizeof (APPLE_KEY_MAP_AGGREGATOR));
    Aggregator->Signature                               = APPLE_KEY_MAP_AGGREGATOR_SIGNATURE;
    Aggregator->NextKeyStrokeIndex                      = 3000;
    Aggregator->DatabaseProtocol.Revision               = APPLE_KEY_MAP_PROTOCOLS_REVISION;
    Aggregator->DatabaseProtocol.CreateKeyStrokesBuffer = KeyMapCreateKeyStrokesBuffer;
    Aggregator->DatabaseProtocol.RemoveKeyStrokesBuffer = KeyMapRemoveKeyStrokesBuffer;
    Aggregator->DatabaseProtocol.SetKeyStrokeBufferKeys = KeyMapSetKeyStrokeBufferKeys;
    Aggregator->AggregatorProtocol.Revision             = APPLE_KEY_MAP_PROTOCOLS_REVISION;
    Aggregator->AggregatorProtocol.GetKeyStrokes        = GetKeyStrokes;
    Aggregator->AggregatorProtocol.ContainsKeyStrokes   = ContainsKeyStrokes;
    InitializeListHead (&Aggregator->KeyStrokesInfoList);

    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gAppleKeyMapDatabaseProtocolGuid,
                    (VOID *)&Aggregator->DatabaseProtocol,
                    &gAppleKeyMapAggregatorProtocolGuid,
                    (VOID *)&Aggregator->AggregatorProtocol,
                    &gAppleEventProtocolGuid,
                    &mAppleEventProtocol,
                    NULL
                    );
/*
    if (!EFI_ERROR (Status)) {
      Status = gBS->InstallProtocolInterface(ImageHandle, &gAppleEventProtocolGuid, EFI_NATIVE_INTERFACE, &mAppleEventProtocol);
    }
 */
  }

  return Status;
}
