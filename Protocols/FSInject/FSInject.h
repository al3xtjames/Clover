/** @file

  Definitions for private EFI_SIMPLE_FILE_SYSTEM_PROTOCOL and
  EFI_FILE_PROTOCOL implementations in FSInject.

  Copyright (C) 2012 Damir Mazar. All rights reserved.<BR>
**/

#ifndef FS_INJECT_PRIVATE_H_
#define FS_INJECT_PRIVATE_H_

// FSI_SIMPLE_FILE_SYSTEM_PROTOCOL_SIGNATURE
/// Signature for FSI_SIMPLE_FILE_SYSTEM_PROTOCOL
#define FSI_SIMPLE_FILE_SYSTEM_PROTOCOL_SIGNATURE  SIGNATURE_32 ('f', 'i', 'f', 's')

// FSI_FROM_SIMPLE_FILE_SYSTEM
/// Macro to access FSI_SIMPLE_FILE_SYSTEM_PROTOCOL from public EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
#define FSI_FROM_SIMPLE_FILE_SYSTEM(Protocol)            \
          CR (Protocol,                                  \
              FSI_SIMPLE_FILE_SYSTEM_PROTOCOL,           \
              FS,                                        \
              FSI_SIMPLE_FILE_SYSTEM_PROTOCOL_SIGNATURE  \
              )

// FSI_SIMPLE_FILE_SYSTEM_PROTOCOL
/// FSInjection EFI_SIMPLE_FILE_SYSTEM_PROTOCOL private structure.
typedef struct {
  UINT64                          Signature;       ///< Identification signature
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL FS;              ///< Public EFI_SIMPLE_FILE_SYSTEM_PROTOCOL

  EFI_HANDLE                      *TgtHandle;      ///< Target handle we are attached to
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *TgtFS;          ///< Target FS we are replacing
  CHAR16                          *TgtDir;         ///< Target dir where injection will be done

  EFI_HANDLE                      *SrcHandle;      ///< Handle where injection dir is
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SrcFS;          ///< FS with injection dir we are replacing
  CHAR16                          *SrcDir;         ///< Injection dir that contains files that will be injected into TgtDir

  FSI_STRING_LIST                 *Blacklist;      ///< Linked list of file names to be blocked on target volume
  FSI_STRING_LIST                 *ForceLoadKexts; ///< Linked list of kext plists
} FSI_SIMPLE_FILE_SYSTEM_PROTOCOL;

// FSI_FILE_PROTOCOL_SIGNATURE
/// Signature for FSI_FILE_PROTOCOL.
#define FSI_FILE_PROTOCOL_SIGNATURE  SIGNATURE_32('f', 'i', 'f', 'p')

// FSI_FROM_FILE_PROTOCOL
/// Macro to access FSI_FILE_PROTOCOL from public EFI_FILE_PROTOCOL
#define FSI_FROM_FILE_PROTOCOL(Protocol)   \
          CR (Protocol,                    \
              FSI_FILE_PROTOCOL,           \
              FP,                          \
              FSI_FILE_PROTOCOL_SIGNATURE  \
              )

// FSI_FILE_PROTOCOL
/// FSInjection EFI_FILE_PROTOCOL private structure.
typedef struct {
  UINT64                          Signature; ///< Identification signature

  EFI_FILE_PROTOCOL               FP;        ///< Public EFI_FILE_PROTOCOL

  FSI_SIMPLE_FILE_SYSTEM_PROTOCOL *FSI_FS;   ///< FS protocol that created this structure
  CHAR16                          *FName;    ///< File name this structure represents
  BOOLEAN                         IsDir;     ///< True after GetInfo if this file is directory
  EFI_FILE_PROTOCOL               *TgtFP;    ///< Target EFI_FILE_PROTOCOL
  EFI_FILE_PROTOCOL               *SrcFP;    ///< EFI_FILE_PROTOCOL from injection volume
  BOOLEAN                         FromTgt;   ///< TRUE if file is opened from original target volume, FALSE if from injection volume
} FSI_FILE_PROTOCOL;

#endif // FS_INJECT_PRIVATE_H_
