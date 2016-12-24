/** @file
  Replaces EFI_SIMPLE_FILE_SYSTEM_PROTOCOL on target volume and
  injects content of specified source folder on source (injection)
  volume into the target folder on the target volume.

  Copyright (C) 2012 Damir Mazar. All rights reserved.<BR>
**/

#ifndef FS_INJECT_H_
#define FS_INJECT_H_

// FSINJECTION_PROTOCOL_GUID
/// The GUID of the FSINJECTION_PROTOCOL.
#define FSINJECTION_PROTOCOL_GUID                         \
  { 0x3F048284, 0x6D4C, 0x11E1,                           \
    { 0xA4, 0xD7, 0x37, 0xE3, 0x48, 0x24, 0x01, 0x9B } }

typedef struct FSINJECTION_PROTOCOL FSINJECTION_PROTOCOL;

// String list entry
typedef struct {
  LIST_ENTRY List;      ///< Must be first in struct
  CHAR16     String[1];
} FSI_STRING_LIST_ENTRY;

typedef FSI_STRING_LIST_ENTRY FSI_STRING_LIST;

// FSINJECTION_INSTALL
/** Overrides the EFI_SIMPLE_FILE_SYSTEM_PROTOCOL with the
    FSI_SIMPLE_FILE_SYSTEM_PROTOCOL.

  @param[in]  TgtHandle       The handle of the target volume.
  @param[in]  TgtDir          The directory on the target volume where the
                              contents of SrcDir will be injcted.
  @param[in]  SrcHandle       The volume where SrcDir exists.
  @param[in]  SrcDir          The directory whose contents will be injected.
  @param[in]  Blacklist       A list of file names that will be blocked on the
                              target volume. The caller should not release
                              allocated list memory after this call.
  @param[in]  ForceLoadKexts  A list of kexts (including paths to Info.plist of
                              each kext) that will be forced to load by changing
                              OSBundleRequired to Root.

  @return                       Returned is the status of the operation.
  @retval EFI_SUCCESS           The FSINJECTION_PROTOCOL was successfully
                                installed.
  @retval EFI_OUT_OF_RESOURCES  The memory necessary to complete the operation
                                could not be allocated.
  @retval other                 An error returned by a sub-operation.
**/
typedef
EFI_STATUS
(EFIAPI *FSINJECTION_INSTALL)(
  IN EFI_HANDLE      TgtHandle,
  IN CHAR16          *TgtDir,
  IN EFI_HANDLE      SrcHandle,
  IN CHAR16          *SrcDir,
  IN FSI_STRING_LIST *Blacklist,
  IN FSI_STRING_LIST *ForceLoadKexts
  );

// FSINJECTION_CREATE_STRING_LIST
/** Creates a new string list, which can be populated with
    FSINJECTION_PROTOCOL.AddStringToList().

  @return       Returned is the created list.
  @retval NULL  The memory necessary to complete the operation could not be
                allocated.
**/
typedef
FSI_STRING_LIST *
(EFIAPI *FSINJECTION_CREATE_STRING_LIST)(
  VOID
  );

// FSINJECTION_ADD_STRING_TO_LIST
/** Adds a string to a string list.

  @param[in]  List    The input string list.
  @param[in]  String  The string to be added to the string list.

  @return       Returned is the updated list.
  @retval NULL  The memory necessary to complete the operation could not be
                allocated.
**/
typedef
FSI_STRING_LIST *
(EFIAPI *FSINJECTION_ADD_STRING_TO_LIST)(
  IN FSI_STRING_LIST *List,
  IN CHAR16          *String
  );

// FSINJECTION_PROTOCOL
struct FSINJECTION_PROTOCOL
{
  FSINJECTION_INSTALL            Install;          ///<
  FSINJECTION_CREATE_STRING_LIST CreateStringList; ///<
  FSINJECTION_ADD_STRING_TO_LIST AddStringToList;  ///<
};

// gFSInjectProtocolGuid
extern EFI_GUID gFSInjectProtocolGuid;

#endif // FS_INJECT_H_
