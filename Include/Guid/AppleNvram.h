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

#ifndef APPLE_NVRAM_H_
#define APPLE_NVRAM_H_

// APPLE_VENDOR_VARIABLE_GUID
/// The Apple Vendor Variable-vendor GUID.
#define APPLE_VENDOR_VARIABLE_GUID                        \
  { 0x4D1EDE05, 0x38C7, 0x4A6A,                           \
    { 0x9C, 0xC6, 0x4B, 0xCC, 0xA8, 0xB3, 0x8C, 0x14 } }

// APPLE_BOOT_VARIABLE_GUID
/// The Apple boot Variable-vendor GUID.
#define APPLE_BOOT_VARIABLE_GUID                          \
  { 0x7C436110, 0xAB2A, 0x4BBB,                           \
    { 0xA8, 0x80, 0xFE, 0x41, 0x99, 0x5C, 0x9F, 0x82 } }

// APPLE_CORE_STORAGE_GUID
/// The Apple Core Storage Variable-vendor GUID.
#define APPLE_CORE_STORAGE_VARIABLE_GUID                  \
  { 0x8D63D4FE, 0xBD3C, 0x4AAD,                           \
    { 0x88, 0x1D, 0x86, 0xFD, 0x97, 0x4B, 0xC1, 0xDF } }

// APPLE_EFI_NVRAM_TRB_SECURE_GUID
/// The Apple TRB Staging Command Variable-vendor GUID.
#define APPLE_TRB_STAGING_COMMAND_VARIABLE_GUID           \
  { 0x5D62B28D, 0x6ED2, 0x40B4,                           \
    { 0xA5, 0x60, 0x6C, 0xD7, 0x9B, 0x93, 0xD3, 0x66 } }

// APPLE_WIRELESS_NETWORK_VARIABLE_GUID
/// The Apple WiFi Network Variable-vendor GUID.
#define APPLE_WIRELESS_NETWORK_VARIABLE_GUID              \
  { 0x36C28AB5, 0x6566, 0x4C50,                           \
    { 0x9E, 0xBD, 0xCB, 0xB9, 0x20, 0xF8, 0x38, 0x43 } }

// APPLE_PERSONALIZATION_VARIABLE_GUID
/// The Apple Personalization Variable-vendor GUID.
#define APPLE_PERSONALIZATION_VARIABLE_GUID               \
  { 0xFA4CE28D, 0xB62F, 0x4C99,                           \
    { 0x9C, 0xC3, 0x68, 0x15, 0x68, 0x6E, 0x30, 0xF9 } }

// APPLE_NVRAM_RESET_GUID
/// The Apple TRB Secure Variable-vendor GUID.
#define APPLE_TRB_SECURE_VARIABLE_GUID                    \
  { 0xF68DA75E, 0x1B55, 0x4E70,                           \
    { 0xB4, 0x1B, 0xA7, 0xB7, 0xA5, 0xB7, 0x58, 0xEA } }

// APPLE_NETBOOT_VARIABLE_GUID
/// The Apple Netboot Variable-vendor GUID.
#define APPLE_NETBOOT_VARIABLE_GUID                       \
  { 0x37BCBEC7, 0xA645, 0x4215,                           \
    { 0x97, 0x9E, 0xF5, 0xAE, 0x4D, 0x11, 0x5F, 0x13 } }

// gAppleVendorVariableGuid
/// A global variable storing the GUID of the APPLE_VENDOR EFI variable scope.
extern EFI_GUID gAppleVendorVariableGuid;

// gAppleBootVariableGuid
/// A global variable storing the GUID of the APPLE_BOOT EFI variable scope.
extern EFI_GUID gAppleBootVariableGuid;

// gAppleCoreStorageVariableGuid
/// A global variable storing the GUID of the APPLE_CORE_STORAGE EFI variable
/// scope.
extern EFI_GUID gAppleCoreStorageNvramGuid;

// gAppleEfiNvramTrbSecureGuid
/// A global variable storing the GUID of the APPLE_TRB_STAGING_COMMAND EFI
/// variable scope.
extern EFI_GUID gAppleTrbStagingCommandVariableGuid;

// AppleWirelessNetworkNvramGuid
/// A global variable storing the GUID of the APPLE_WIRELESS_NETWORK EFI
/// variable scope.
extern EFI_GUID gAppleWirelessNetworkVariableGuid;

// AppleEfiAppearanceNvramGuid
/// A global variable storing the GUID of the APPLE_PERSONALIZATION EFI
/// variable scope.
extern EFI_GUID gApplePersonalizationVariableGuid;

// gAppleEfiNvramResetGuid
/// A global variable storing the GUID of the APPLE_TRB_SECURE EFI variable
/// scope.
extern EFI_GUID gAppleTrbSecureVariableGuid;

// gAppleNetbootVariableGuid
/// A global variable storing the GUID of the APPLE_NETBOOT EFI variable
/// scope.
extern EFI_GUID gAppleNetbootVariableGuid;

#endif // APPLE_NVRAM_H_
