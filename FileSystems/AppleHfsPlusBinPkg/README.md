### Apple HFS+ EFI Driver

Due to licensing restrictions, Apple's binary HFS+ EFI driver cannot be included in the Clover repo, installer and binaries. VirtualBox's open-source HFS+ EFI driver (in VBoxFsDxe) is used instead.

If you wish to use Apple's driver when compiling your own binaries, place the extracted HfsPlus.efi driver in the corresponding arch folder in HfsPlusDxe (Ia32 for 32-bit, X64 for 64-bit). Specify `-D USE_APPLE_HFSPLUS_DRIVER` as a build option to include Apple's driver in the binaries.
