//
//    (C) 2005,2014 Aktiv Co. All rights reserved.
//    Definitions for Guardant API libraries.
//

#ifndef __GRDAPI__H_
#define __GRDAPI__H_

#if defined(_WIN32_WCE)
#define GRD_API             __cdecl
#elif defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#define GRD_API             __stdcall
#elif defined(__GNUC__)
#define GRD_API
#else
#error "This Guardant API supports 32/x64-bit compilers only"
#endif

#if (defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64)) && !defined (__GNUC__)
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef unsigned __int64 QWORD;
typedef void* HANDLE;
#else
#ifdef __cplusplus
// for future use
// #include <cstdint>
// #include <tr1/cstdint>
#include <stdint.h>
#else // __cplusplus
#include <stdint.h>
#endif // __cplusplus
typedef int32_t BOOL;
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
typedef void* HANDLE;
#endif

#define GrdContainerSize    0x4000  // Size of memory allocated for Guardant protected container

//- Demo Codes
#define GrdDC_DEMONVK       0x519175b7Lu  // Demo public code
#define GrdDC_DEMORDO       0x51917645Lu  // Demo private read code
#define GrdDC_DEMOPRF       0x51917603Lu  // Demo private write code
#define GrdDC_DEMOMST       0x5191758cLu  // Demo private master code

//- Dongle Models
#define GrdDM_GS1L          0       // Guardant Stealth         LPT
#define GrdDM_GS1U          1       // Guardant Stealth         USB
#define GrdDM_GF1L          2       // Guardant Fidus           LPT
#define GrdDM_GS2L          3       // Guardant StealthII       LPT
#define GrdDM_GS2U          4       // Guardant StealthII       USB
#define GrdDM_GS3U          5       // Guardant StealthIII      USB
#define GrdDM_GF1U          6       // Guardant Fidus           USB
#define GrdDM_GS3SU         7       // Guardant Sign/Time       USB
#define GrdDM_GCU           8       // Guardant Code            USB
#define GrdDM_GSP           9       // Guardant SP              SOFTWARE
#define GrdDM_Total         10      // Number of different models

//- Dongle Models
#define GrdDM_Stealth1LPT   GrdDM_GS1L  // Guardant Stealth         LPT
#define GrdDM_Stealth1USB   GrdDM_GS1U  // Guardant Stealth         USB
#define GrdDM_FidusLPT      GrdDM_GF1L  // Guardant Fidus           LPT
#define GrdDM_Stealth2LPT   GrdDM_GS2L  // Guardant Stealth II      LPT
#define GrdDM_Stealth2USB   GrdDM_GS2U  // Guardant Stealth II      USB
#define GrdDM_Stealth3USB   GrdDM_GS3U  // Guardant Stealth III     USB
#define GrdDM_FidusUSB      GrdDM_GF1U  // Guardant Fidus           USB
#define GrdDM_SignUSB       GrdDM_GS3SU // Guardant Sign/Time       USB
#define GrdDM_Soft          GrdDM_GSP   // Guardant SP              SOFTWARE
#define GrdDM_CodeUSB       GrdDM_GCU   // Guardant Code            USB

//- Dongle Interfaces
#define GrdDI_LPT           0       // LPT port
#define GrdDI_USB           1       // USB bus
#define GrdDI_SP            2       // Software bus

//- Definitions for use in GrdSetDriverMode () function call
#define GrdDM_GRD_DRV       0       // Guardant driver
#define GrdDM_USB_HID       1       // HID driver
#define GrdDM_WINUSB        2       // WINUSB driver

//- Dongle Types
#define GrdDT_DOS           0x0000  // DOS Stealth Dongle
#define GrdDT_Win           0x0000  // Windows Stealth Dongle
#define GrdDT_LAN           0x0001  // LAN Stealth Dongle
#define GrdDT_Time          0x0002  // Time Stealth Dongle
#define GrdDT_GSII64        0x0008  // Support of Guardant Stealth II/III 64 bit (GSII64) algorithm
#define GrdDT_PI            0x0010  // Support of Guardant Stealth III protected items
#define GrdDT_TRU           0x0020  // Support of Guardant Stealth III remote update
#define GrdDT_RTC           0x0040  // Support of Real Time Clock
#define GrdDT_AES           0x0080  // Support of AES 128 algorithm
#define GrdDT_ECC           0x0100  // Support of ECC 160 algorithm
#define GrdDT_LoadableCode  0x0400  // Support of loadable code
#define GrdDT_Reserved      0x0800  // Reserved for 'feature' functionality
#define GrdDT_MSCRemovable  0x1000  // MSC removable. USB-connector with uSD.

//-- Flags GrdCreateHandle() Mode
#define GrdCHM_SingleThread 0x00000000  // Multi-threading support is disabled
#define GrdCHM_MultiThread  0x00000001  // Multi-threading support is enabled (access synchronization to hGrd is enabled)

//-- Flags for GrdLogin() Licensing Mode
#define GrdLM_PerStation    0x00000000  // Allocate Guardant Net license for each workstation
#define GrdLM_PerHandle     0x00000001  // Allocate Guardant Net license for each logged in protected container
#define GrdLM_PerProcess    0x00000002  // Allocate Guardant Net license for each process (application copy)

//-- Flags for Remote Mode of GrdStartup()/GrdSetFindMode()
//- Dongle SetFindMode Remote mode search flags
#define GrdFMR_Local        1                              // Local dongle
#define GrdFMR_Remote       2                              // Remote (network) dongle

//-- Flags for GrdStartupEx()
//- Remote client settings file path (path to gnclient.ini)
#define GrdRCS_UserDefined  0x00000000   // User defined path,
// szNetworkClientIniPath parameter must contain full path to the filename or directory where the
// remote client settings file will be saved
#define GrdRCS_ProgramData  0x80000001   // User defined path relative to the ProgramData folder,
// szNetworkClientIniPath parameter must contain a relative path to the filename or directory where the
// remote client settings file will be saved
#define GrdRCS_EnvVar       0x80000002   // Detect path use environment variable,
// szNetworkClientIniPath parameter must contain the name of environment variable which points
// to the full pathname or directory where the remote client settings file will be saved

//- SetFindMode dongle model search bits
#define GrdFMM_GS1L         ((DWORD)1 << GrdDM_GS1L)                  // Guardant Stealth     LPT
#define GrdFMM_GS1U         ((DWORD)1 << GrdDM_GS1U)                  // Guardant Stealth     USB
#define GrdFMM_GF1L         ((DWORD)1 << GrdDM_GF1L)                  // Guardant Fidus       LPT
#define GrdFMM_GS2L         ((DWORD)1 << GrdDM_GS2L)                  // Guardant StealthII   LPT
#define GrdFMM_GS2U         ((DWORD)1 << GrdDM_GS2U)                  // Guardant StealthII   USB
#define GrdFMM_GS3U         ((DWORD)1 << GrdDM_GS3U)                  // Guardant StealthIII  USB
#define GrdFMM_GF1U         ((DWORD)1 << GrdDM_GF1U)                  // Guardant Fidus       USB
#define GrdFMM_GS3SU        ((DWORD)1 << GrdDM_GS3SU)                 // Guardant Sign/Time   USB
#define GrdFMM_GCU          ((DWORD)1 << GrdDM_GCU)                   // Guardant Code        USB
#define GrdFMM_GSP          ((DWORD)1 << GrdDM_GSP)                   // Guardant SP          SOFTWARE
#define GrdFMM_GS1          (GrdFMM_GS1L | GrdFMM_GS1U)               // Guardant Stealth I   of any interface
#define GrdFMM_GF           (GrdFMM_GF1L | GrdFMM_GF1U)               // Guardant Fidus   I   of any interface
#define GrdFMM_GS2          (GrdFMM_GS2L | GrdFMM_GS2U)               // Guardant Stealth II  of any interface
#define GrdFMM_GS3          (GrdFMM_GS3U)                             // Guardant Stealth III of any interface
#define GrdFMM_GS3S         (GrdFMM_GS3SU)                            // Guardant Sign/Time of any interface
#define GrdFMM_GC           (GrdFMM_GCU)                              // Guardant Code of any interface
#define GrdFMM_ALL          0                                         // All Guardant Stealth & Fidus family dongles
//- SetFindMode dongle model search bits
#define GrdFMM_Stealth1LPT  ((DWORD)1 << GrdDM_Stealth1LPT)           // Guardant Stealth     LPT
#define GrdFMM_Stealth1USB  ((DWORD)1 << GrdDM_Stealth1USB)           // Guardant Stealth     USB
#define GrdFMM_FidusLPT     ((DWORD)1 << GrdDM_FidusLPT)              // Guardant Fidus       LPT
#define GrdFMM_Stealth2LPT  ((DWORD)1 << GrdDM_Stealth2LPT)           // Guardant StealthII   LPT
#define GrdFMM_Stealth2USB  ((DWORD)1 << GrdDM_Stealth2USB)           // Guardant StealthII   USB
#define GrdFMM_Stealth3USB  ((DWORD)1 << GrdDM_Stealth3USB)           // Guardant StealthIII  USB
#define GrdFMM_FidusUSB     ((DWORD)1 << GrdDM_FidusUSB)              // Guardant Fidus       USB
#define GrdFMM_SignUSB      ((DWORD)1 << GrdDM_SignUSB)               // Guardant Sign/Time   USB
#define GrdFMM_Soft         ((DWORD)1 << GrdDM_Soft)                  // Guardant SP          SOFTWARE
#define GrdFMM_CodeUSB      ((DWORD)1 << GrdDM_CodeUSB)               // Guardant Code        USB
#define GrdFMM_Stealth1     (GrdFMM_Stealth1LPT | GrdFMM_Stealth1USB) // Guardant Stealth I of any interface
#define GrdFMM_Fidus        (GrdFMM_FidusLPT | GrdFMM_FidusUSB)       // Guardant Fidus I of any interface
#define GrdFMM_Stealth2     (GrdFMM_Stealth2LPT | GrdFMM_Stealth2USB) // Guardant Stealth II of any interface
#define GrdFMM_Stealth3     (GrdFMM_Stealth3USB)                      // Guardant Stealth III of any interface
#define GrdFMM_Sign         (GrdFMM_SignUSB)                          // Guardant Sign/Time of any interface
#define GrdFMM_Code         (GrdFMM_CodeUSB)                          // Guardant Code of any interface

//- SetFindMode dongle Interface search bits
#define GrdFMI_LPT          ((DWORD)1 << GrdDI_LPT)        // LPT port
#define GrdFMI_USB          ((DWORD)1 << GrdDI_USB)        // USB bus
#define GrdFMI_SP           ((DWORD)1 << GrdDI_SP)         // Software bus
#define GrdFMI_ALL          0                              // All Guardant Stealth & Fidus interfaces

//-- Definition for use in GrdFind() function call
#define GrdF_First          1       // First call
#define GrdF_Next           0       // All subsequent calls

//-- Flags of find parameters during GrdLogin() or GrdFind()
#define GrdFM_NProg         0x0001  // wDongleNProg == dwProg
#define GrdFM_ID            0x0002  // dwDongleID == dwID
#define GrdFM_SN            0x0004  // Serial Number wDongleSN == dwSN
#define GrdFM_Ver           0x0008  // bDongleVersion >= dwVer
#define GrdFM_Mask          0x0010  // wDongleMask & dwMask == dwMask
#define GrdFM_Type          0x0020  // wDongleType & dwType == dwType

//-- GrdSetWorkMode() dwFlagsWork mode flags
#define GrdWM_UAM           0x00000000  // Enables UAM (User Address Mode) for read/write operations. Default mode
#define GrdWM_SAM           0x00000080  // Enables SAM (System Address Mode) for read/write operations
#define GrdWM_CodeIsString  0x00000100  // Reserved
#define GrdWM_NoRetry       0x00000200  // Disables auto configuration of communication protocol
#define GrdWM_NoFullAccess  0x00000400  // Disables full capture of the parallel port resources
#define GrdWM_OnlyStdLPT1   0x00000800  // Enables search for the dongle in LPT1 only (address 0x378)
#define GrdWM_OnlyStdLPT2   0x00001000  // Enables search for the dongle in LPT2 only (address 0x278)
#define GrdWM_OnlyStdLPT3   0x00002000  // Enables search for the dongle in LPT3 only (address 0x3BC)
#define GrdWM_NoAutoMem32   0x00004000  // Indicates that data segment is different from the standard one
#define GrdWM_UseOldCRC     0x00008000  // Reserved
#define GrdWM_NotStdLPTAddr 0x02000000L

//-- GrdSetWorkMode() dwFlagsMode parameters
#define GrdWMFM_DriverAuto      0x0000  // Use driver automatically. Call the dongle by means of driver if it is installed
#define GrdWMFM_DriverOnly      0x0001  // Call the dongle by means of driver only
#define GrdWMFM_BypassDriver    0x0002  // Bypass driver in Win9X

//- Lock Mode flags ---
//  Login, Check and Find commands cannot be locked
#define GrdLM_Nothing       0x00000000  // Works like critical section
#define GrdLM_Init          0x00000001  // Prevent Init operations
#define GrdLM_Protect       0x00000002  // Prevent Protect operations
#define GrdLM_Transform     0x00000004  // Prevent Transform operations
#define GrdLM_Read          0x00000008  // Prevent reading from UAM memory
#define GrdLM_Write         0x00000010  // Prevent writing to UAM memory
#define GrdLM_Activate      0x00000020  // Prevent activation of protected items
#define GrdLM_Deactivate    0x00000040  // Prevent deactivation of protected items
#define GrdLM_ReadItem      0x00000080  // Prevent reading from protected items
#define GrdLM_UpdateItem    0x00000100  // Prevent updating of protected items
#define GrdLM_All           0xFFFFFFFF  // Prevent all mentioned above operations

//- UAM Addresses of Fields ---
#define GrdUAM_bNProg           (30 - 30) // 00h Programm number
#define GrdUAM_bVer             (31 - 30) // 01h Version
#define GrdUAM_wSN              (32 - 30) // 02h Serial number
#define GrdUAM_wMask            (34 - 30) // 04h Bit mask
#define GrdUAM_wGP              (36 - 30) // 06h Counter #1 (GP)
#define GrdUAM_wRealLANRes      (38 - 30) // 08h Current network license limit
#define GrdUAM_dwIndex          (40 - 30) // 0Ah Index
#define GrdUAM_abAlgoAddr       (44 - 30) // 0Eh User data, algorithm descriptors

//- SAM Addresses of Fields
// Fields protection against nsc_Init, nsc_Protect, nsc_Write commands
//    * - Partial protection: nsc_Protect can be executed only after nsc_Init
//    X - Full protection
// Protection against command:                Init Protect Write
#define GrdSAM_byDongleModelAddr     0 //  0h   X     X     X    0=GS,1=GU,2=GF
#define GrdSAM_byDongleMemSizeAddr   1 //  1h   X     X     X    0=0, 8=256
#define GrdSAM_byDongleProgVerAddr   2 //  2h   X     X     X
#define GrdSAM_byDongleProtocolAddr  3 //  3h   X     X     X
#define GrdSAM_wClientVerAddr        4 //  4h   X     X     X    0x104=1.4
#define GrdSAM_byDongleUserAddrAddr  6 //  6h   X     X     X
#define GrdSAM_byDongleAlgoAddrAddr  7 //  7h   X     X     X
#define GrdSAM_wPrnportAddr          8 //  8h   X     X     X
#define GrdSAM_byWriteProtectS3     10 //  Ah         *     X
#define GrdSAM_byReadProtectS3      12 //  Ch         *     X
#define GrdSAM_dwPublicCode         14 //  Eh   X     X     X
#define GrdSAM_byVersion            18 // 12h   X     X     X
#define GrdSAM_byLANRes             19 // 13h   X     X     X
#define GrdSAM_wType                20 // 14h   X     X     X
#define GrdSAM_dwID                 22 // 16h   X     X     X
#define GrdSAM_byWriteProtect       26 // 1Ah         *     X
#define GrdSAM_byReadProtect        27 // 1Bh         *     X
#define GrdSAM_byNumFunc            28 // 1Ch         *     X
#define GrdSAM_byTableLMS           29 // 1Dh         *     X
#define GrdSAM_byTableLMS_S3        29 // 1Dh         *     X
#define GrdSAM_UAM                  30 // 1Eh  start address of UAM memory
#define GrdSAM_byNProg              30 // 1Eh   X     X
#define GrdSAM_byVer                31 // 1Fh   X     X
#define GrdSAM_wSN                  32 // 20h   X     X
#define GrdSAM_wMask                34 // 22h   X     X
#define GrdSAM_wGP                  36 // 24h   X     X
#define GrdSAM_wRealLANRes          38 // 26h   X     X
#define GrdSAM_dwIndex              40 // 28h   X     X
#define GrdSAM_abyAlgoAddr          44 // 2Ch

/*
   //-- Guardant Stealth I & II old compatibility default Algorithm Numbers & Request Sizes
   //- Guardant Stealth I & II Algorithm Numbers
   #define GrdAN_AutoProtect           0   // For automatic protection
   #define GrdAN_Fast                  1   // For CodeInit (EnCode/DeCode) operation
   #define GrdAN_Random                2   // Random number generator
   #define GrdAN_DEMO                  3   // For Transform operation
   #define GrdAN_GSII64                4   // For GSII64 TransformEx operation
   //- Guardant Stealth I & II Algorithm Request Size
   #define GrdARS_AutoProtect          4   // For automatic protection
   #define GrdARS_Fast                 32   // For CodeInit (EnCode/DeCode) operation
   #define GrdARS_Random               4   // Random number generator
   #define GrdARS_DEMO                 4   // For Transform operation
   #define GrdARS_GSII64               8   // For GSII64 TransformEx operation
 */

//-- Guardant Stealth III default Algorithms & Protected Items
//- Guardant Stealth III default Algorithms & Protected Items numbers
#define GrdAN_GSII64                0   // GSII64 for automatic protection & use in API
#define GrdAN_HASH64                1   // HASH64 for automatic protection & use in API
#define GrdAN_RAND64                2   // RAND64 for automatic protection & use in API
#define GrdAN_READ_ONLY             3   // Protected Item for read-only data. Can be updated via Trusted Remote Update
#define GrdAN_READ_WRITE            4   // Protected Item for read/write data. Can be updated at protected application runtime
#define GrdAN_GSII64_DEMO           5   // GSII64 demo algorithm for use in Guardant examples
#define GrdAN_HASH64_DEMO           6   // HASH64 demo algorithm for use in Guardant examples
#define GrdAN_ECC160                8   // ECC160 for automatic protection & use in API
#define GrdAN_AES128                9   // AES128 for automatic protection & use in API
#define GrdAN_GSII64_ENCRYPT        10  // GSII64_ENCRYPT for automatic protection & use in API
#define GrdAN_GSII64_DECRYPT        11  // GSII64_DECRYPT for automatic protection & use in API

//- Guardant Stealth III default Algorithm & Protected Items minimum Request Size
#define GrdARS_GSII64               8   // GSII64 for automatic protection & use in API
#define GrdARS_HASH64               8   // HASH64 for automatic protection & use in API
#define GrdARS_RAND64               8   // RAND64 for automatic protection & use in API
#define GrdARS_READ_ONLY            8   // Protected Item for read-only data. Can be updated via Trusted Remote Update
#define GrdARS_READ_WRITE           8   // Protected Item for read/write data. Can be updated at protected application runtime
#define GrdARS_GSII64_DEMO          8   // GSII64 demo algorithm for use in Guardant examples
#define GrdARS_HASH64_DEMO          8   // HASH64 demo algorithm for use in Guardant examples
#define GrdARS_ECC160               20  // ECC160 for automatic protection & use in API
#define GrdARS_AES128               16  // AES128 for automatic protection & use in API
#define GrdARS_HASH_SHA256          0   // SHA256 for automatic protection & use in API

//- Guardant Stealth III default Algorithm & Protected Items maximum Request Size
#define GrdAMRS_GSII64              248 // GSII64 for automatic protection & use in API
#define GrdAMRS_HASH64              248 // HASH64 for automatic protection & use in API
#define GrdAMRS_RAND64              248 // RAND64 for automatic protection & use in API

//- Guardant Stealth III default Algorithm & Protected Items Determinant Size
#define GrdADS_GSII64               16  // GSII64 for automatic protection & use in API
#define GrdADS_HASH64               16  // HASH64 for automatic protection & use in API
#define GrdADS_RAND64               16  // RAND64 for automatic protection & use in API
#define GrdADS_READ_ONLY            8   // Protected Item for read-only data. Can be updated via Trusted Remote Update
#define GrdADS_READ_WRITE           8   // Protected Item for read/write data. Can be updated at protected application runtime
#define GrdADS_GSII64_DEMO          16  // GSII64 demo algorithm for use in Guardant examples
#define GrdADS_HASH64_DEMO          16  // HASH64 demo algorithm for use in Guardant examples
#define GrdADS_ECC160               20  // ECC160 for automatic protection & use in API
#define GrdADS_AES128               16  // AES128 for automatic protection & use in API

// Guardant GSII64 DEMO Algorithm default passwords
#define GrdAP_GSII64_DEMO_ACTIVATION    0xAAAAAAAA
#define GrdAP_GSII64_DEMO_DEACTIVATION  0xDDDDDDDD
#define GrdAP_GSII64_DEMO_READ          0xBBBBBBBB
#define GrdAP_GSII64_DEMO_UPDATE        0xCCCCCCCC

// Guardant HASH64 DEMO Algorithm default passwords
#define GrdAP_HASH64_DEMO_ACTIVATION    0xAAAAAAAA
#define GrdAP_HASH64_DEMO_DEACTIVATION  0xDDDDDDDD
#define GrdAP_HASH64_DEMO_READ          0xBBBBBBBB
#define GrdAP_HASH64_DEMO_UPDATE        0xCCCCCCCC

//- Guardant Stealth Fast EnCode/DeCode Algorithm Methods
#define GrdAT_Algo0                 0  // Basic method
#define GrdAT_AlgoASCII             1  // Character method
#define GrdAT_AlgoFile              2  // File method

//- Guardant Stealth API Algorithm use Method
//- bit 0-5 block chaining mode
#define GrdAM_ECB                   0    // Electronic Code Book
#define GrdAM_CBC                   1    // Cipher Block Chaining
#define GrdAM_CFB                   2    // Cipher Feed Back
#define GrdAM_OFB                   3    // Output Feed Back
//- bit 7 - Encode/Decode
#define GrdAM_Encode                0    // Encode mode
#define GrdAM_Decode                0x80 // Decode mode
//- Software- or hardware-implemented cryptographic algorithm mode
// can be combined with GrdAM_ECB, GrdAM_CBC, GrdAM_CFB, GrdAM_OFB
//- bit 8-9 First/Next/Last
#define GrdSC_First                 0x100  // First data block
#define GrdSC_Next                  0x200  // Next data block
#define GrdSC_Last                  0x400  // Last data block
#define GrdSC_All                   (GrdSC_First + GrdSC_Next + GrdSC_Last)
// Synonym definitions
#define GrdAM_Encrypt               GrdAM_Encode
#define GrdAM_Decrypt               GrdAM_Decode

//- Hardware-implemented ECC160 algorithm properties
#define GrdECC160_PUBLIC_KEY_SIZE   40  // ECC160 public key size
#define GrdECC160_PRIVATE_KEY_SIZE  20  // ECC160 private key size
#define GrdECC160_DIGEST_SIZE       40  // ECC160 digest size
#define GrdECC160_MESSAGE_SIZE      20  // ECC160 message size

//- Software-implemented cryptographic or hashing algorithm flag
#define GrdSA_SoftAlgo              0x80000000

//- Software implemented asymmetric cryptoalgorithms for call GrdVerifySign
#define GrdVSC_ECC160               0   // ECC160 asymmetric cryptoalgorithm type

//- Software implemented asymmetric cryptoalgorithms for call GrdSign
#define GrdSS_ECC160                (GrdSA_SoftAlgo + 0)   // ECC160 asymmetric cryptoalgorithm number

//- Software-implemented cryptographic algorithm for call GrdCrypt & GrdCryptEx
#define GrdSC_AES256                (GrdSA_SoftAlgo + 0)

//- Software-implemented hashing algorithms for call GrdHash & GrdHashEx
#define GrdSH_CRC32                 (GrdSA_SoftAlgo + 0)
#define GrdSH_SHA256                (GrdSA_SoftAlgo + 1)

//- Hashing algorithms properties
#define GrdHASH64_DIGEST_SIZE       8   // HASH64 algorithm hash size
#define GrdCRC32_DIGEST_SIZE        4   // CRC32 algorithm hash size
#define GrdSHA256_DIGEST_SIZE       32  // SHA256 algorithm hash size

//- Software-implemented AES256 algorithm properties
#define GrdAES256_KEY_SIZE          32  // AES256 key size
#define GrdAES256_BLOCK_SIZE        16  // AES256 block size

//- Software-implemented or Hardware-implemented algorithms context size
#define GrdSHA256_CONTEXT_SIZE      0x200               // software SHA256 context size
#define GrdCRC32_CONTEXT_SIZE       4                   // software CRC32 context size
#define GrdHASH_CONTEXT_SIZE        0x200               // hardware HASH64 & SHA256 context size
//- Software-implemented algorithms context size
#define GrdAES_CONTEXT_SIZE         0x4000              // must be >= sizeof(AES_CONTEXT)
#define GrdAES256_CONTEXT_SIZE      GrdAES_CONTEXT_SIZE // synonym definition

// GrdTRU_SetAnswerProperties() mode
#define GrdTRU_Flags_Init                1  // Execute Init before Update
#define GrdTRU_Flags_Protect             2  // Execute Protect after Update

// GrdProtect() and GrdTRU_SetAnswerProperties() global flags
#define GrdGF_ProtectTime                1     // Disable RTC modification by GrdSetTime call
#define GrdGF_HID                        2     // Enable HID mode
#define GrdGF_OnlyOneSessKey             4     // Allow only one instance of Guardant API Protected application to run
#define GrdGF_2ndSessKey                 8     // Allow only one instance of AutoProtected application to run
#define GrdGF_WINUSB                     0x10  // Enable WINUSB mode NOTE: this bit overlap GrdGF_HID if they set at the same time

// GrdTRU_DecryptQuestionEx(), GrdTRU_DecryptQuestionTimeEx(), GrdTRU_EncryptAnswerEx(), crypt mode
#define GrdTRU_CryptMode_GSII64          0
#define GrdTRU_CryptMode_AES128SHA256    1

// GrdPI_Update Methods
#define GrdUM_MOV                        0
#define GrdUM_XOR                        1

// Language constants for conversion Guardant error code to text message
// string via GrdGetErrorMessage()
#define GrdLng_ENG                       0
#define GrdLng_RUS                       7

// Guardant get information code constants for GrdGetInfo()
// API information
#define GrdGIV_VerAPI               0x0000  // API version
// Common Mode
#define GrdGIM_WorkMode             0x1000  // Work mode
#define GrdGIM_HandleMode           0x1001  // Handle mode
// Find & logon mode
#define GrdGIF_Remote               0x2000  // Local and/or remote (GrdFMR_Local + GrdFMR_Remote)
#define GrdGIF_Flags                0x2001  // Flags
#define GrdGIF_Prog                 0x2002  // Program number field value
#define GrdGIF_ID                   0x2003  // Dongle ID field value
#define GrdGIF_SN                   0x2004  // Serial number field value
#define GrdGIF_Ver                  0x2005  // Version field value
#define GrdGIF_Mask                 0x2006  // Bit mask field value
#define GrdGIF_Type                 0x2007  // Dongle type field value
#define GrdGIF_Model                0x2008  // Possible model     bits. 1 << GrdDM_XXX (GS1L, GS1U, GF1L, GS2L, GS2U )
#define GrdGIF_Interface            0x2009  // Possible interface bits. 1 << GrdDI_XXX (LPT | USB) )
// Logon information on current dongle
#define GrdGIL_Remote               0x3000  // Local or remote dongle
#define GrdGIL_ID                   0x3001  // ID of current dongle
#define GrdGIL_Model                0x3002  // Model of current dongle
#define GrdGIL_Interface            0x3003  // Interface of current dongle
//#define GrdGIL_LockID             0x3004
#define GrdGIL_LockCounter          0x3005  // Lock counter value for current dongle
#define GrdGIL_Seek                 0x3006  // Current dongle memory address
#define GrdGIL_RProgVer             0x3007  // Program version (in MCU)
#define GrdGIL_McuType              0x3008  // Dongle MCU Type
#define GrdGIL_MemoryType           0x3009  // Dongle Memory Type

// Logon information on current local or remote dongle driver
#define GrdGIL_DrvVers              0x4000  // Driver       version (0x0550=5.50)
#define GrdGIL_DrvBuild             0x4001  // Driver       build
#define GrdGIL_PortLPT              0x4002  // LPT port address (0 == USB)
#define GrdGIL_SoftFileName         0x4003  // Software dongle container file name unicode string

// Logon information on current remote dongle
#define GrdGIR_VerSrv               0x5000  // Guardant Net server version. Requires the handle to be logged in.
#define GrdGIR_LocalIP              0x5001  // Guardant Net local IP address. Requires the handle to be logged in.
#define GrdGIR_LocalPort            0x5002  // TCP/IP port. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_LocalNB              0x5003  // Guardant Net local NetBIOS name. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_RemoteIP             0x5004  // Guardant Net remote IP address. Requires the handle to be logged in.
#define GrdGIR_RemotePort           0x5005  // TCP/IP port
#define GrdGIR_RemoteNB             0x5006  // Guardant Net remote NetBIOS name. Requires the handle to be logged in.
#define GrdGIR_HeartBeatThread      0x5007  // Handle of internal heartbeat thread
#define GrdGIR_IniTimeOutSend       0x5008  // Send operation timeout in seconds. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniTimeOutReceive    0x5009  // Receive operation timeout in seconds. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniTimeOutSearch     0x500A  // Broadcasting search timeout in seconds. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniClientUDPPort     0x500B  // Client's UDP port for sending of datagrams to a server. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniServerUDPPort     0x500C  // Server's UDP port for sending of replies to a client. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniBroadcastAddress  0x500D  // Broadcasting address . Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniFileName          0x500E  // Initialization file name. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_LocalMACAddress      0x500F  // MAC address of the local network adapter. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_FullHostName         0x5010  // Full name of the local host. Requires the API to be started up with GrdFMR_Remote flag.
#define GrdGIR_IniServerIPName      0x5011  // Server IP address or host name. Requires the API to be started up with GrdFMR_Remote flag.


//- CRC Starting value
#define Grd_StartCRC                -1l // Starting value for GrdCRC

#define GrdSeekCur                  -1l // Use current dongle memory pointer associated with handle

//- Error codes
#define GrdE_OK                     0   // No errors
#define GrdE_DongleNotFound         1   // Dongle with specified search conditions not found
// 2 Code not found (N/A)
#define GrdE_AddressTooBig          3   // The specified address is too big
// 4 Byte counter too big // N/A
#define GrdE_GPis0                  5   // GP executions counter exhausted (has 0 value)
#define GrdE_InvalidCommand         6   // Invalid dongle call command
// 7 not used (N/A)
#define GrdE_VerifyError            8   // Write verification error
#define GrdE_NetProtocolNotFound    9   // Network protocol not found
#define GrdE_NetResourceExhaust     10  // License counter of Guardant Net exhausted
#define GrdE_NetConnectionLost      11  // Connection with Guardant Net server was lost
#define GrdE_NetDongleNotFound      12  // Guardant Net server not found
#define GrdE_NetServerMemory        13  // Guardant Net server memory allocation error
#define GrdE_DPMI                   14  // DPMI error
#define GrdE_Internal               15  // Internal error
#define GrdE_NetServerReloaded      16  // Guardant Net server has been reloaded
#define GrdE_VersionTooOld          17  // This command is not supported by this dongle version
#define GrdE_BadDriver              18  // Windows NT driver is required
#define GrdE_NetProtocol            19  // Network protocol error
#define GrdE_NetPacket              20  // Network packet format is not supported
#define GrdE_NeedLogin              21  // Logging in is required
#define GrdE_NeedLogout             22  // Logging out is required
#define GrdE_DongleLocked           23  // Guardant dongle is busy (locked by another copy of protected application)
#define GrdE_DriverBusy             24  // Guardant driver cannot capture the parallel port
// 25 - 29 not used
#define GrdE_CRCError               30  // CRC error occurred while attempting to call the dongle
#define GrdE_CRCErrorRead           31  // CRC error occurred while attempting to read data from the dongle
#define GrdE_CRCErrorWrite          32  // CRC error occurred while attempting to write data to the dongle
#define GrdE_Overbound              33  // The boundary of the dongle's memory has been override
#define GrdE_AlgoNotFound           34  // The hardware algorithm with this number has not been found in the dongle
#define GrdE_CRCErrorFunc           35  // CRC error of the hardware algorithm
#define GrdE_AllDonglesFound        36  // All dongles found, or CRC error occurred while attempting to execute ChkNSK operation
#define GrdE_ProtocolNotSup         37  // Guardant API release is too old
#define GrdE_InvalidCnvType         38  // Non-existent reversible conversion method has been specified
#define GrdE_UnknownError           39  // Unknown error occurred while attempting to call the algorithm or protected item, operation may be incomplete
#define GrdE_AccessDenied           40  // Invalid password
#define GrdE_StatusUnchangeable     41  // Error counter has been exhausted for this Protected Item
#define GrdE_NoService              42  // Specified algorithm or protected item does not support requested service
#define GrdE_InactiveItem           43  // Specified algorithm or protected item is inactive, command has not been executed
#define GrdE_DongleServerTooOld     44  // Dongle server does not support specified command
#define GrdE_DongleBusy             45  // Dongle is busy at this moment. The command cannot be executed. The dongle keeps busy for 11 seconds after GrdInit
#define GrdE_InvalidArg             46  // One or more arguments are invalid
#define GrdE_MemoryAllocation       47  // Memory allocation error
#define GrdE_InvalidHandle          48  // Invalid handle
#define GrdE_ContainerInUse         49  // This protected container is already in use
#define GrdE_Reserved50             50  // Reserved
#define GrdE_Reserved51             51  // Reserved
#define GrdE_Reserved52             52  // Reserved
#define GrdE_SystemDataCorrupted    53  // Remote update system data corrupted
#define GrdE_NoQuestion             54  // Remote update question has not been generated
#define GrdE_InvalidData            55  // Invalid remote update data format
#define GrdE_QuestionOK             56  // Remote update question has been already generated
#define GrdE_UpdateNotComplete      57  // Remote update writing has not been completed
#define GrdE_InvalidHash            58  // Invalid remote update data hash
#define GrdE_GenInternal            59  // Internal error
#define GrdE_AlreadyInitialized     60  // This copy of Guardant API has been already initialized
#define GrdE_RTC_Error              61  // Real Time Clock error
#define GrdE_BatteryError           62  // Real Time Clock battery low error
#define GrdE_DuplicateNames         63  // Duplicate items/algorithms names
#define GrdE_AATFormatError         64  // Address in AAT table is out of range
#define GrdE_SessionKeyNtGen        65  // Session key not generated
#define GrdE_InvalidPublicKey       66  // Invalid public key
#define GrdE_InvalidDigitalSign     67  // Invalid digital sign
#define GrdE_SessionKeyGenError     68  // Session key generation error
#define GrdE_InvalidSessionKey      69  // Invalid session key
#define GrdE_SessionKeyTooOld       70  // Session key too old
#define GrdE_NeedInitialization     71  // Initialization is required
#define GrdE_gcProhibitCode         72  // Verification of loadable code failed
// Probable reasons: invalid entry point, forbidden instructions
// in the code, attempt to access prohibited memory address
#define GrdE_gcLoadableCodeTimeOut  73                 // Loadable code time out
#define GrdE_gcFlashSizeFromDescriptorTooSmall      74 // Flash memory size specified in item descriptor for loadable code is too small
#define GrdE_Reserved75                             75 // Reserved
#define GrdE_Reserved76                             76 // Reserved
#define GrdE_Reserved77                             77 // Reserved
#define GrdE_Reserved78                             78 // Reserved
#define GrdE_Reserved79                             79 // Reserved
#define GrdE_gcIncorrectMask                        80 // TGrdLoadableCodeData structure exceeds space reserved for determinant in item descriptor
#define GrdE_gcRamOverboundInProtect                81 // Incorrect RAM area specified in loadable code descriptor
#define GrdE_gcFlashOverboundInProtect              82 // Incorrect FLASH memory area specified in loadable code descriptor
#define GrdE_gcIntersectionOfCodeAreasInProtect     83 // Allocation of intersecting FLASH memory areas for different loadable code modules
#define GrdE_gcBmapFileTooBig                       84 // BMAP file is too long
#define GrdE_gcZeroLengthProgram                    85 // The loadable code has zero length
#define GrdE_gcDataCorrupt                          86 // Data verification failed
#define GrdE_gcProtocolError                        87 // Error in Guardant Code protocol
#define GrdE_gcGCEXENotFound                        88 // Loadable code not found
#define GrdE_gcNotEnoughRAM                         89 // IO buffer size specified in loadable code is not enough for transmitting/receiving data
#define GrdE_gcException                            90 // Security violation in Guardant Code virtual environment
#define GrdE_gcRamOverboundInCodeLoad               91 // IO buffer specified in loadable code exceeds the bounds of allowed RAM area
#define GrdE_gcFlashOverboundInCodeLoad             92 // Loadable code exceeds the bounds of allowed FLASH memory area
#define GrdE_gcIntersectionOfCodeAreasInCodeLoad    93 // Allocation of intersecting RAM areas for different loadable code modules ( Init operation is required)
#define GrdE_gcGCEXEFormatError                     94 // Incorrect GCEXE file format
#define GrdE_gcRamAccessViolation                   95 // Incorrect RAM area specified in loadable code for GcaCodeRun
#define GrdE_gcCallDepthOverflow                    96 // Too many nested calls of GcaCodeRun.
#define GrdE_UnableToCreateIniFile                  97 // Unable to create client configuration file
#define GrdE_LastError                              98 // Total number of errors

// Structure returned by GrdFind
#pragma pack(push,1)
typedef struct
{
    DWORD dwPublicCode;             // Public code
    BYTE byHrwVersion;              // Dongle hardware version
    BYTE byMaxNetRes;               // Maximum Guardant Net license limit
    WORD wType;                     // Dongle type flags
    DWORD dwID;                     // Dongle ID (unique)
    // Following fields are available from UAM mode
    BYTE byNProg;                   // Program number
    BYTE byVer;                     // Version
    WORD wSN;                       // Serial number
    WORD wMask;                     // Bit mask
    WORD wGP;                       // Executions GP counter/ License time counter
    WORD wRealNetRes;               // Current Guardant Net license limit, must be <= byMaxNetRes
    DWORD dwIndex;                  // Index for remote programming
    // Only Stealth III info
    BYTE abyReservedISEE[0x1C];     // Reserved for future
    WORD wWriteProtectS3;           // Stealth III write protect address
    WORD wReadProtectS3;            // Stealth III read protect address
    WORD wGlobalFlags;              // Global dongle flags for Sign\Time or higher dongles. See GrdGF_xxx definition.
    DWORD dwDongleState;            // Dongle State. See GrdDSF_XXX definition
    // Available since:
    // 1. Stealth Sign.(Firmware number >= 0x01000011h or 01.00.00.11)
    // 2. Guardant Code.
    DWORD dwOldMPNum;                       // Old firmware number(before SFU).
    BYTE abyReservedD[0x20];
    BYTE abyUid[0x10];                      // MCU unique identifier (Guardant Code since 1.0.2.29)
    BYTE abyReservedH[0x8C];                // Reserved. For align to 0x100
    // Reserved info from gsA
    // Driver info
    DWORD dwGrDrv_Platform;                 // Driver platform (Win32/Win64)
    DWORD dwGrDrv_Vers;                     // Driver version (0x04801234=4.80.12.34
    DWORD dwGrDrv_Build;                    // Driver build
    DWORD dwGrDrv_Flags;                    // Additional flags. 1 - means that is SP dongle in driverless mode
    // dongle info
    DWORD dwRkmUserAddr;                    // wkmUserAddr
    DWORD dwRkmAlgoAddrW;                   // wkmAlgoAddr
    DWORD dwPrnPort;                        // Printer port address or 0 if it USB
    DWORD dwClientVersion;                  // Dongle client version
    // SAP start
    DWORD dwRFlags;                         // Type of MCU
    DWORD dwRProgVer;                       // Program version (in MCU)
    DWORD dwRcn_rc;                         // curr_num & answer code
    DWORD dwNcmps;                          // Number of compare conditions
    DWORD dwNSKClientVersion;               // Client version (low byte - minor, hi - major)
    DWORD dwModel;                          // Dongle Model
    DWORD dwMcuType;                        // Dongle MCU Type
    DWORD dwMemoryType;                     // Dongle Memory Type
    BYTE byFeaturesFlags;                   // Support features (1 - means support)
    // Reserved for future
    BYTE abyReserved[0x200 - 0x100 - 0x29]; // Reserved. For align to 0x200
} TGrdFindInfo;
#pragma pack(pop)

//- Dongle State Flags
#define GrdDSF_CRCError                  1
#define GrdDSF_RemoteUpdateModeOn        2
#define GrdDSF_TimerErrorFrequency       4
#define GrdDSF_TimerErrorIntegrity       8
#define GrdDSF_BatteryError              16

// Dongle memory fields in SAM
#pragma pack(push,1)
typedef struct
{
    // Fields protection against nsc_Init, nsc_Protect, nsc_Write commands
    //    * - Partial protection: nsc_Protect can be executed only after nsc_Init
    //    X - Full protection
    // Protection against command:      Init Protect Write
    BYTE byDongleModel;         //  0h   X     X     X    0=GS,1=GU,2=GF,
    BYTE byDongleMemSize;       //  1h   X     X     X    0=0, 8=256 ( Memsize = 1 << byDongleMemSize )
    BYTE byDongleProgVer;       //  2h   X     X     X
    BYTE byDongleProtocol;      //  3h   X     X     X
    WORD wClientVer;            //  4h   X     X     X    0x104=1.4
    BYTE byDongleUserAddr;      //  6h   X     X     X
    BYTE byDongleAlgoAddr;      //  7h   X     X     X
    WORD wPrnport;              //  8h   X     X     X
    WORD wWriteProtectS3;       //  Ah         *     X      // Stealth III write protect SAM address in bytes
    WORD wReadProtectS3;        //  Ch         *     X      // Stealth III read  protect SAM address in bytes
    DWORD dwPublicCode;         //  Eh   X     X     X
    BYTE byVersion;             // 12h   X     X     X
    BYTE byLANRes;              // 13h   X     X     X
    WORD wType;                 // 14h   X     X     X
    DWORD dwID;                 // 16h   X     X     X
    BYTE byWriteProtect;        // 1Ah         *     X      // Stealth I & II write protect SAM address in words
    BYTE byReadProtect;         // 1Bh         *     X      // Stealth I & II read  protect SAM address in words
    BYTE byNumFunc;             // 1Ch         *     X
    BYTE byTableLMS;            // 1Dh         *     X
    BYTE byNProg;               // 1Eh   X     X
    BYTE byVer;                 // 1Fh   X     X
    WORD wSN;                   // 20h   X     X
    WORD wMask;                 // 22h   X     X
    WORD wGP;                   // 24h   X     X
    WORD wRealLANRes;           // 26h   X     X
    DWORD dwIndex;              // 28h   X     X
    BYTE abyAlgoAddr[1];        // 2Ch
} TGrdStdFields;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    WORD wYear;             // The year (1601 - 2099)
    WORD wMonth;            // The month (January = 1, February = 2, ...)
    WORD wDayOfWeek;        // The day of the week (Sunday = 0, Monday = 1, ...)
    WORD wDay;              // The day of the month (1-31)
    WORD wHour;             // The hour (0-23)
    WORD wMinute;           // The minute (0-59)
    WORD wSecond;           // The second (0-59)
    WORD wMilliseconds;     // The millisecond (0-999)
} TGrdSystemTime;
#pragma pack(pop)

#define GrdNotifyMessage_DongleArrived     0   // Dongle has been connected to USB port
#define GrdNotifyMessage_DongleRemoved     1   // Dongle has been disconnected from USB port
#define GrdNotifyMessage_ConnectionLost    2   // Lost connection with the Guardant Net Server
#define GrdNotifyMessage_ConnectionRestore 3   // Restore connection with the Guardant Net Server

// Dongle Notification CallBack routine
typedef void (GRD_API GrdDongleNotifyCallBack)(
                                               HANDLE hGrd,            // Handle to Guardant protected container
                                               int nGrdNotifyMessage); // Dongle notification message. See GrdNotifyMessage_XXX definition

#pragma pack(push,1)
typedef struct
{
    BYTE bLoadableCodeVersion;    // Loadable code version.
    BYTE bReserved0;              // Reserved
    BYTE bState;                  // Loadable code state.
    BYTE bReserved;               // Reserved
    DWORD dwLoadingDate;          // Loading date.
} TGrdCodePublicData;
#pragma pack(pop)

// Loadable code state (for TGrdCodePublicData.bState):
#define GrdCodeState_CodeNotLoad      0   // - not loading.
#define GrdCodeState_CodeStartLoad    1   // - loading now.
#define GrdCodeState_CodeOk           2   // - loaded now.

// Return's struct for GrdCodeGetInfo
#pragma pack(push,1)
typedef struct
{
    DWORD dwStartAddr;              // Flash start address for loadable code.
    DWORD dwCodeSizeMax;            // Flash size for loadable code.
    DWORD dwCodeSectorSize;         // Flash sector size for loadable code.
    DWORD dwStartRamAddr;           // RAM start address for loadable code.
    DWORD dwRamSizeMax;             // RAM size for loadable code.
    DWORD dwReserved;               // Reserved.
    TGrdCodePublicData PublicDataLoadableCode;
    BYTE abHashLoadableCode[32];    // Hash of loadable code.
    BYTE abReserved[64];            // Reserved.
} TGrdCodeInfo;
#pragma pack(pop)

// The structure of the determinant for the type descriptor "loadable code"
#pragma pack(push,1)
typedef struct
{
    TGrdCodePublicData PublicDataLoadableCode;  //
    BYTE abLoadableCodePublicKey4VerifySign[GrdECC160_PUBLIC_KEY_SIZE];
    BYTE abLoadableCodePrivateKey4DecryptKey[GrdECC160_PRIVATE_KEY_SIZE];
    DWORD dwBegFlashAddr;                       // The specified start address flash-memory for loadable code.
    DWORD dwEndFlashAddr;                       // The specified end address flash-memory for loadable code.
    DWORD dwBegMemAddr;                         // The specified start address RAM-memory for loadable code.
    DWORD dwEndMemAddr;                         // The specified end address RAM-memory for loadable code.
} TGrdLoadableCodeData;
#pragma pack(pop)

// --- Guardant Stealth API functions Prototypes for C/C++
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Initialize this copy of GrdAPI. GrdStartup() must be called once before first GrdAPI call at application startup
int GRD_API GrdStartup(DWORD dwRemoteMode);         // Local and/or remote (GrdFMR_Local + GrdFMR_Remote)

// Initialize this copy of GrdAPI. GrdStartup() must be called once before first GrdAPI call at application startup
// Support user defined path to gnClient.ini file
int GRD_API GrdStartupEx(DWORD dwRemoteMode,                 // Local and/or remote (GrdFMR_Local + GrdFMR_Remote)
                         const char* szNetworkClientIniPath, // Path to gnClient.ini, depends on dwFlags value
                         DWORD dwFlags);                     // See GrdRCS_XXXXX definition

// Deinitialize this copy of GrdAPI. GrdCleanup() must be called after last GrdAPI call before program termination
int GRD_API GrdCleanup(void);

// Must be called at the beginning of any Win32 DllMain function
int GRD_API GrdDllMain(HANDLE hinstDLL, DWORD fdwReason, void* lpvReserved);

// Get last error information from protected container from specified handle
// The last error code is maintained on a per-handle basis
// Multiple handles do not overwrite each other's last-error code
int GRD_API GrdGetLastError(HANDLE hGrd,
                            void** ppLastErrFunc); // If not used, must be NULL

// Convert Guardant error code to a text message string (Windows OS with Russian lang => CP-1251, otherwise => UTF-8 as is)
int GRD_API GrdFormatMessage(HANDLE hGrd,       // if != NULL then handle for GrdGetLastError()
                             int nErrorCode,    // if hGrd == NULL then Guardant error code. Else ignored
                             int nLanguage,     // Guardant language ID
                             char* szErrorMsg,  // Pointer to a buffer for the formatted (and null-terminated) message.
                             int nErrorMsgSize, // this parameter specifies the maximum number of characters that can be stored in the output buffer.
                             void* pReserved);  // Reserved, must be NULL

// Convert Guardant error code to a text message string (UTF-8 for all)
int GRD_API GrdFormatMessageUtf8(HANDLE hGrd,       // if != NULL then handle for GrdGetLastError()
                                 int nErrorCode,    // if hGrd == NULL then Guardant error code. Else ignored
                                 int nLanguage,     // Guardant language ID
                                 char* szErrorMsg,  // Pointer to a buffer for the formatted (and null-terminated) message.
                                 int nErrorMsgSize, // this parameter specifies the maximum number of characters that can be stored in the output buffer.
                                 void* pReserved);  // Reserved, must be NULL

// Get requested information
int GRD_API GrdGetInfo(HANDLE hGrd,       // Handle to Guardant protected container
                       DWORD dwInfoCode,  // Code of requested information. See GrdGIX_XXXXX
                       void* pInfoData,   // Pointer to a buffer for return data
                       DWORD dwInfoSize); // Number of bytes for returning

// Protected container handle validator
BOOL GRD_API GrdIsValidHandle(HANDLE hGrd);

// Create Grd protected container & return it's handle
HANDLE GRD_API GrdCreateHandle(HANDLE hGrd,      // Pointer to memory allocated for protected container
                                                 //                       if NULL, GrdAPI allocates memory for a new protected container by itself
                               DWORD dwMode,     // Since 6.31, this parameter is ignored and should always be GrdCHM_MultiThread
                               void* pReserved); // Reserved, must be NULL

// Close specified handle
// Log out from dongle/server & free allocated memory
int GRD_API GrdCloseHandle(HANDLE hGrd);

// Store dongle codes in Guardant protected container
int GRD_API GrdSetAccessCodes(HANDLE hGrd,         // Handle to Guardant protected container
                              DWORD dwPublic,      // Must be already specified
                              DWORD dwPrivateRD,   // Must be already specified
                              DWORD dwPrivateWR,   // == 0 if not used
                              DWORD dwPrivateMST); // == 0 if not used

// Set dongle working mode to Guardant protected container
int GRD_API GrdSetWorkMode(HANDLE hGrd,
                           DWORD dwFlagsWork,  // combination of GrdWM_XXX flags
                           DWORD dwFlagsMode); // combination of GrdWMFM_XXX flags

// Set dongle search conditions and operation modes to Guardant protected container
int GRD_API GrdSetFindMode(HANDLE hGrd,        // Handle to Guardant protected container
                           DWORD dwRemoteMode, // Local and/or remote (GrdFMR_Local + GrdFMR_Remote)
                           DWORD dwFlags,      // Combination of GrdFM_XXXXX flags
                           DWORD dwProg,
                           DWORD dwID,
                           DWORD dwSN,
                           DWORD dwVer,
                           DWORD dwMask,
                           DWORD dwType,
                           DWORD dwModels,      // Possible dongle model bits. See GrdFMM_GSX definition
                           DWORD dwInterfaces); // Possible dongle interface bits. See GrdFMI_XXX definition

// Obtain the ID and other dongle info of the first or next dongle found
int GRD_API GrdFind(HANDLE hGrd,              // Handle to Guardant protected container
                    DWORD dwMode,             // GrdF_First or GrdF_Next
                    DWORD* pdwID,             // Pointer to variable for return dongle ID
                    TGrdFindInfo* pFindInfo); // Structure for returning dongle info or NULL for ignoring it in other languages

// Login to Guardant dongle
int GRD_API GrdLogin(HANDLE hGrd,         // Handle to Guardant protected container
                     DWORD dwModuleLMS,   // Module number or 0xFFFFFFFF if License Management System functions are not used
                     DWORD dwLoginFlags); // Login flags (GrdLM_PerStation or GrdLM_PerHandle)

// Logout from Guardant dongle
int GRD_API GrdLogout(HANDLE hGrd,   // Handle to Guardant protected container
                      DWORD dwMode); // == NULL if not need

// Increment lock counter of specified dongle
int GRD_API GrdLock(HANDLE hGrd,                  // Handle to logged in Guardant protected container
                    DWORD dwTimeoutWaitForUnlock, // Max GrdAPI unlock waiting time. 0xFFFFFFFF == infinity. 0 == no waiting
                    DWORD dwTimeoutAutoUnlock,    // Max dongle locking time in ms.  0xFFFFFFFF == infinity. 0 == 10000 ms (10 sec)
                    DWORD dwMode);

// Unlock specified dongle
int GRD_API GrdUnlock(HANDLE hGrd);

// Check for dongle availability
int GRD_API GrdCheck(HANDLE hGrd);

// Check for dongle availability and decrement GP executions counter
// Requires Private write code to be specified in GrdSetAccessCodes
int GRD_API GrdDecGP(HANDLE hGrd);

// Read a block of bytes from the dongle's memory
int GRD_API GrdRead(HANDLE hGrd,      // Handle to Guardant protected container
                    DWORD dwAddr,     // Starting address in dongle memory to be read
                    DWORD dwLng,      // Length of data to be read
                    void* pData,      // Buffer for data to be read
                    void* pReserved); // Reserved, must be NULL

// Write a block of bytes into the dongle's memory
// Requires Private write code to be specified in GrdSetAccessCodes
int GRD_API GrdWrite(HANDLE hGrd,       // Handle to Guardant protected container
                     DWORD dwAddr,      // Starting address in dongle memory to be written
                     DWORD dwLng,       // Length of data to be written
                     const void* pData, // Buffer for data to be written
                     void* pReserved);  // Reserved, must be NULL

// Moves the dongle memory pointer associated with handle to a new location that is offset bytes from origin
// of dongle memory in current addressing mode. The next operation on the handle takes place at the new location.
int GRD_API GrdSeek(HANDLE hGrd, DWORD dwAddr);

// Initialize the dongle's memory
// Requires Private master code to be specified in GrdSetAccessCodes
int GRD_API GrdInit(HANDLE hGrd);

// Implement locks / Specify the number of hardware algorithms
// and LMS table address
// Requires Private master code to be specified in GrdSetAccessCodes
int GRD_API GrdProtect(HANDLE hGrd,         // Handle to Guardant protected container
                       DWORD dwWrProt,      // SAM addres of the first byte available for writing in bytes
                       DWORD dwRdProt,      // SAM addres of the first byte available for reading in bytes
                       DWORD dwNumFunc,     // Number of hardware-implemented algorithms in the dongle including all protected items and LMS table of Net III
                       DWORD dwTableLMS,    // Net II: SAM address of the first byte of LMS Table in 2-byte words;
                                            // Net III: number of protected item that contains LMS Table
                       DWORD dwGlobalFlags, // Global dongle flags for Sign\Time or higher dongles. See GrdGF_xxx definition.
                       void* pReserved);    // Reserved, must be NULL


//--- Cryptographic functions

// Transform a block of bytes using dongle's hardware-implemented algorithm (including GSII64)
int GRD_API GrdTransform(HANDLE hGrd,        // Handle to Guardant protected container
                         DWORD dwAlgoNum,    // Number of hardware- or software-implemented algorithm
                         DWORD dwLng,        // Data length
                         void* pData,        // Data for Encryption/Decryption
                         DWORD dwMethod,     // if Stealth I or Fidus it must be 0. Otherwise - combination of GrdAM_XXX flags
                         void* pIV);         // if Stealth I or Fidus it must be NULL. Otherwise - 8-bytes initialization vector

int GRD_API GrdTransformEx(HANDLE hGrd,      // Handle to Guardant protected container
                           DWORD dwAlgoNum,  // Number of hardware- or software-implemented algorithm
                           DWORD dwDataLng,  // Data length
                           void* pData,      // Data for Encryption/Decryption
                           DWORD dwMethod,   // if Stealth I or Fidus it must be 0. Otherwise - combination of GrdAM_XXX flags
                           DWORD dwIVLng,    // IV length
                           void* pIV,        // if Stealth I or Fidus it must be NULL. Otherwise - n-bytes initialization vector
                           void* pReserved); // Reserved, must be NULL

// Encrypt/decrypt a block of bytes using encryption algorithm
int GRD_API GrdCrypt(HANDLE hGrd,           // Handle to Guardant protected container
                     DWORD dwAlgo,          // Number of hardware- or software-implemented algorithm
                     DWORD dwDataLng,       // Data length
                     void* pData,           // Data for Encryption/Decryption
                     DWORD dwMethod,        // Encrypt/Decrypt, First/Next/Last, block chaining modes (ECB/OFB/...)
                     void* pIV,             // Initialization Vector
                     const void* pKeyBuf,   // Encryption/decryption secret key for software-implemented algorithm (NULL if not used)
                     void* pContext);       // Context for multiple-buffer encryption. Must be corresponded GrdXXXXXX_CONTEXT_SIZE bytes size

int GRD_API GrdCryptEx(HANDLE hGrd,         // Handle to Guardant protected container
                       DWORD dwAlgo,        // Number of hardware- or software-implemented algorithm
                       DWORD dwDataLng,     // Data length
                       void* pData,         // Data for Encryption/Decryption
                       DWORD dwMethod,      // Encrypt/Decrypt, First/Next/Last, block chaining modes (ECB/OFB/...)
                       DWORD dwIVLng,       // IV length
                       void* pIV,           // Initialization Vector
                       const void* pKeyBuf, // Encryption/decryption secret key for software-implemented algorithm (NULL if not used)
                       void* pContext,      // Context for multiple-buffer encryption. Must be corresponded GrdXXXXXX_CONTEXT_SIZE bytes size
                       void* pReserved);    // Reserved, must be NULL

// Hash calculation of a block of bytes
int GRD_API GrdHash(HANDLE hGrd,           // Handle to Guardant protected container
                    DWORD dwHash,          // Number of hardware- or software-implemented algorithm
                    DWORD dwDataLng,       // Data length
                    const void* pData,     // Data for hash calculation
                    DWORD dwMethod,        // GrdSC_First/GrdSC_Next/GrdSC_Last
                    void* pDigest,         // Pointer to memory allocated for hash on GrdSC_Last step
                    const void* pKeyBuf,   // Not used, must be NULL
                    void* pContext);       // Context for multiple buffer calculation. Must be corresponded GrdXXXXXX_CONTEXT_SIZE bytes size

int GRD_API GrdHashEx(HANDLE hGrd,         // Handle to Guardant protected container
                      DWORD dwHash,        // Number of hardware- or software-implemented algorithm
                      DWORD dwDataLng,     // Data length
                      const void* pData,   // Data for hash calculation
                      DWORD dwMethod,      // GrdSC_First/GrdSC_Next/GrdSC_Last
                      DWORD dwDigestLng,   // Digest length
                      void* pDigest,       // Pointer to memory allocated for hash on GrdSC_Last step
                      DWORD dwKeyBufLng,   // Not used, must be 0
                      const void* pKeyBuf, // Not used, must be NULL
                      DWORD dwContextLng,  // Context length
                      void* pContext,      // Context for multiple buffer calculation. Must be corresponded GrdXXXXXX_CONTEXT_SIZE bytes size
                      void* pReserved);    // Reserved, must be NULL

//--- Old compatibility software coding functions

// Initialize a password for fast reversible conversion
int GRD_API GrdCodeInit(HANDLE hGrd, DWORD dwCnvType, DWORD dwAddr, void* pKeyBuf);

// Encode data using fast reversible conversion
int GRD_API GrdEnCode(DWORD dwCnvType, const void* pKeyBuf, const void* pData, DWORD dwLng);

// Decode data using fast reversible conversion
int GRD_API GrdDeCode(DWORD dwCnvType, const void* pKeyBuf, void* pData, DWORD dwLng);

// Calculate 32-bit CRC of a memory block
DWORD GRD_API GrdCRC(const void* pData, DWORD dwLng, DWORD dwPrevCRC);

//--- Protected Item functions

// Activate dongle Algorithm or Protected Item
int GRD_API GrdPI_Activate(HANDLE hGrd,          // Handle to Guardant protected container
                           DWORD dwItemNum,      // Algorithm or Protected Item number to be activated
                           DWORD dwActivatePsw); // Optional password. If not used, must be 0

// Deactivate dongle Algorithm or Protected Item
int GRD_API GrdPI_Deactivate(HANDLE hGrd,            // Handle to Guardant protected container
                             DWORD dwItemNum,        // Algorithm or Protected Item number to be deactivated
                             DWORD dwDeactivatePsw); // Optional password. If not used, must be 0

// Read data from dongle Protected Item
int GRD_API GrdPI_Read(HANDLE hGrd,      // Handle to Guardant protected container
                       DWORD dwItemNum,  // Algorithm or Protected Item number to be read
                       DWORD dwAddr,     // Offset in Algorithm or Protected Item data
                       DWORD dwLng,      // Number of bytes for reading
                       void* pData,      // Pointer to buffer for read data
                       DWORD dwReadPsw,  // Optional password. If not used, must be 0
                       void* pReserved); // Reserved, must be NULL

// Update data in dongle Protected Item
int GRD_API GrdPI_Update(HANDLE hGrd,       // Handle to Guardant protected container
                         DWORD dwItemNum,   // Algorithm or Protected Item number to be updated
                         DWORD dwAddr,      // Offset in Algorithm or Protected Item data
                         DWORD dwLng,       // Number of bytes for updating
                         const void* pData, // Pointer to buffer with data to be written
                         DWORD dwUpdatePsw, // Optional password. If not used, must be 0
                         DWORD dwMethod,    // Update method. See GrdUM_XXX definitions
                         void* pReserved);  // Reserved, must be NULL

// --- Guardant Trusted Remote Update API

// Write secret GSII64 remote update key for Guardant Secured Remote Update to the dongle
// Requires Private master code to be specified in GrdSetAccessCodes
int GRD_API GrdTRU_SetKey(HANDLE hGrd,          // Handle to Guardant protected container
                          const void* pKey128); // Unique Trusted Remote Update 128-bit secret key

// Generate encrypted question and initialize remote update procedure
int GRD_API GrdTRU_GenerateQuestionEx(HANDLE hGrd,         // Handle to Guardant protected container
                                      DWORD dwLngQuestion, // Size Question buf.
                                      void* pQuestion,     // Pointer to Question
                                      DWORD* pdwID,        // Pointer to ID (4 bytes)
                                      DWORD* pdwPublic,    // Pointer to Dongle Public Code 4 bytes
                                      DWORD dwLngHash,     // Size Hash buf.
                                      void* pHash,         // Pointer to Hash of prev
                                      DWORD dwReserved,    // Reserved, must be 0
                                      void* pReserved);    // Reserved, must be NULL
int GRD_API GrdTRU_GenerateQuestion(HANDLE hGrd,           // Handle to Guardant protected container
                                    void* pQuestion,       // Pointer to Question                  8 bytes (64 bit)
                                    DWORD* pdwID,          // Pointer to ID                        4 bytes
                                    DWORD* pdwPublic,      // Pointer to Dongle Public Code        4 bytes
                                    void* pHash);          // Pointer to Hash of previous 16 bytes 8 bytes

// Generate encrypted question and initialize remote update procedure
int GRD_API GrdTRU_GenerateQuestionTimeEx(HANDLE hGrd,         // Handle to Guardant protected container
                                          DWORD dwLngQuestion, // Size for Question
                                          void* pQuestion,     // Pointer to Question
                                          DWORD* pdwID,        // Pointer to ID 4 bytes
                                          DWORD* pdwPublic,    // Pointer to Dongle Public Code 4 bytes
                                          QWORD* pqwDongleTime,
                                          DWORD dwDeadTimesSize,
                                          QWORD* pqwDeadTimes,
                                          DWORD* pdwDeadTimesNumbers,
                                          DWORD dwLngHash,    // Size for Hash
                                          void* pHash,        // Pointer to Hash of previous data
                                          DWORD dwReserved,   // Reserved, must be 0
                                          void* pReserved);   // Reserved, must be NULL
int GRD_API GrdTRU_GenerateQuestionTime(HANDLE hGrd,          // Handle to Guardant protected container
                                        void* pQuestion,      // Pointer to Question                  8 bytes (64 bit)
                                        DWORD* pdwID,         // Pointer to ID                        4 bytes
                                        DWORD* pdwPublic,     // Pointer to Dongle Public Code        4 bytes
                                        QWORD* pqwDongleTime, // Pointer to Dongle Time (encrypted)   8 bytes
                                        DWORD dwDeadTimesSize,
                                        QWORD* pqwDeadTimes,
                                        DWORD* pdwDeadTimesNumbers,
                                        void* pHash,      // Pointer to Hash of previous data
                                        void* pReserved); // Reserved, must be NULL

// Decrypt and validate question
int GRD_API GrdTRU_DecryptQuestionEx2(HANDLE hGrd,            // Handle to Guardant protected container of dongle that contains
                                                              // decrypt algorithm with the same key as in remote dongle
                                      DWORD dwLngQuestion,    // Size for Question
                                      void* pQuestion,        // Pointer to Question
                                      DWORD dwID,             // ID                                   4 bytes
                                      DWORD dwPublic,         // Public Code                          4 bytes
                                      DWORD dwLngHash,        // Size for Hash
                                      const void* pHash,      // Pointer to Hash of prev
                                      DWORD dwMode,           // IN: Only GrdTRU_CryptMode_AES128SHA256 supported
                                      DWORD dwKeySize,        // Size of key as in remote dongle (TRU key).crypt=aes128(16 byte).
                                      const void* pKey);      // Pointer to same key as in remote dongle (TRU key).crypt=aes128(16 byte).

int GRD_API GrdTRU_DecryptQuestionEx(HANDLE hGrd,             // Handle to Guardant protected container of dongle that contains
                                                              // decrypt algorithm with the same key as in remote dongle
                                     DWORD dwAlgoNum_Decrypt, // Dongle decrypt algorithm number with same key as in remote dongle
                                     DWORD dwAlgoNum_Hash,    // Dongle HASH algorithm number with same key as in remote dongle
                                     DWORD dwLngQuestion,     // Size for Question
                                     void* pQuestion,         // Pointer to Question
                                     DWORD dwID,              // ID                                   4 bytes
                                     DWORD dwPublic,          // Public Code                          4 bytes
                                     DWORD dwLngHash,         // Size for Hash
                                     const void* pHash,       // Pointer to Hash of prev
                                     DWORD dwMode,            // GrdTRU_CryptMode_GSII64      : crypt=gsII64( 8 byte), hash=based on gsII64(8 byte)
                                                              // GrdTRU_CryptMode_AES128SHA256: crypt=aes128(16 byte), hash=sha256(32 byte)
                                     DWORD dwReserved,        // Reserved, must be 0
                                     void* pReserved);        // Reserved, must be NULL
int GRD_API GrdTRU_DecryptQuestion(HANDLE hGrd,               // Handle to Guardant protected container of dongle that contains
                                                              // GSII64 algorithm with the same key as in remote dongle
                                   DWORD dwAlgoNum_GSII64,    // Dongle GSII64 algorithm number with same key as in remote dongle
                                   DWORD dwAlgoNum_HashS3,    // Dongle HASH64 algorithm number with same key as in remote dongle
                                   void* pQuestion,           // Pointer to Question                  8 bytes (64 bit)
                                   DWORD dwID,                // ID                                   4 bytes
                                   DWORD dwPublic,            // Public Code                          4 bytes
                                   const void* pHash);        // Pointer to Hash of previous 16 bytes 8 bytes

// Decrypt and validate question
int GRD_API GrdTRU_DecryptQuestionTimeEx2(HANDLE hGrd,          // Handle to Guardant protected container of dongle that contains
                                          DWORD dwLngQuestion,  // Size for Question
                                          void* pQuestion,      // Pointer to Question
                                          DWORD dwID,           // ID                                   4 bytes
                                          DWORD dwPublic,       // Public Code                          4 bytes
                                          QWORD* pqwDongleTime, // Pointer to Dongle Time (encrypted)   8 bytes
                                          QWORD* pqwDeadTimes,
                                          DWORD dwDeadTimesNumbers,
                                          DWORD dwLngHash,        // Size for Hash
                                          const void* pHash,      // Pointer to Hash of previous
                                          DWORD dwMode,           // IN: Only GrdTRU_CryptMode_AES128SHA256 supported
                                          DWORD dwKeySize,        // Size of key as in remote dongle (TRU key).crypt=aes128(16 byte).
                                          const void* pKey);      // Pointer to same key as in remote dongle (TRU key).crypt=aes128(16 byte).

int GRD_API GrdTRU_DecryptQuestionTimeEx(HANDLE hGrd,             // Handle to Guardant protected container of dongle that contains
                                                                  // decrypt algorithm with the same key as in remote dongle
                                         DWORD dwAlgoNum_Decrypt, // Dongle decrypt algorithm number with same key as in remote dongle
                                         DWORD dwAlgoNum_Hash,    // Dongle HASH algorithm number with same key as in remote dongle
                                         DWORD dwLngQuestion,     // Size for Question
                                         void* pQuestion,         // Pointer to Question
                                         DWORD dwID,              // ID                                   4 bytes
                                         DWORD dwPublic,          // Public Code                          4 bytes
                                         QWORD* pqwDongleTime,    // Pointer to Dongle Time (encrypted)   8 bytes
                                         QWORD* pqwDeadTimes,
                                         DWORD dwDeadTimesNumbers,
                                         DWORD dwLngHash,      // Size for Hash
                                         const void* pHash,    // Pointer to Hash of previous
                                         DWORD dwMode,         // GrdTRU_CryptMode_GSII64      : crypt=gsII64( 8 byte), hash=based on gsII64(8 byte)
                                                               // GrdTRU_CryptMode_AES128SHA256: crypt=aes128(16 byte), hash=sha256(32 byte)
                                         DWORD dwReserved,     // Reserved, must be 0
                                         void* pReserved);     // Reserved, must be NULL
int GRD_API GrdTRU_DecryptQuestionTime(HANDLE hGrd,            // Handle to Guardant protected container of dongle that contains
                                                               // GSII64 algorithm with the same key as in remote dongle
                                       DWORD dwAlgoNum_GSII64, // Dongle GSII64 algorithm number with same key as in remote dongle
                                       DWORD dwAlgoNum_HashS3, // Dongle HASH64 algorithm number with same key as in remote dongle
                                       void* pQuestion,        // Pointer to Question                  8 bytes (64 bit)
                                       DWORD dwID,             // ID                                   4 bytes
                                       DWORD dwPublic,         // Public Code                          4 bytes
                                       QWORD* pqwDongleTime,   // Pointer to Dongle Time (encrypted)   8 bytes
                                       QWORD* pqwDeadTimes,
                                       DWORD dwDeadTimesNumbers,
                                       const void* pHash); // Pointer to Hash of previous 16 bytes 8 bytes

// Set Init & Protect parameters for Trusted Remote Update
// This function must be called after GrdTRU_DecryptQuestion and before GrdTRU_EncryptAnswer functions
// only if Init & Protect operations will be executed during remote update (call GrdTRU_ApplyAnswer) procedure on remote PC
int GRD_API GrdTRU_SetAnswerProperties(HANDLE hGrd,         // Handle to Guardant protected container
                                       DWORD dwTRU_Flags,   // Use Init & Protect or not
                                       DWORD dwReserved,    // Reserved, must be 0
                                       DWORD dwWrProt,      // remote GrdProtect parameters, SAM addres of the first byte available for writing in bytes
                                       DWORD dwRdProt,      // remote GrdProtect parameters, SAM addres of the first byte available for reading in bytes
                                       DWORD dwNumFunc,     // remote GrdProtect parameters, Number of hardware-implemented algorithms in the dongle
                                                            // including all protected items and LMS table of Net III
                                       DWORD dwTableLMS,    // remote GrdProtect parameters, Net III: number of protected item that contains LMS Table
                                       DWORD dwGlobalFlags, // Global dongle flags for Sign\Time or higher dongles. See GrdGF_xxx definition.
                                       void* pReserved);    // remote GrdProtect parameters, Reserved, must be NULL

// Prepare data for Trusted Remote Update
int GRD_API GrdTRU_EncryptAnswerEx2(HANDLE hGrd,             // Handle to Guardant protected container of dongle that contains
                                                             // encrypt algorithm with the same key as in remote dongle
                                                             // and pre-stored GrdTRU_SetAnswerProperties data if needed
                                                             // Works only with GrdTRU_CryptMode_AES128SHA256
                                    DWORD dwAddr,            // Starting address for writing in dongle
                                    DWORD dwLng,             // Size of data to be written
                                    const void* pData,       // Buffer for data to be written
                                    DWORD dwLngQuestion,     // Size for Question
                                    const void* pQuestion,   // Pointer to decrypted Question
                                    void* pAnswer,           // Pointer to the buffer for Answer data
                                    DWORD* pdwAnswerSize,    // IN: Maximum buffer size for Answer data, OUT: Size of pAnswer buffer
                                    DWORD dwMode,            // IN: Only GrdTRU_CryptMode_AES128SHA256 supported
                                    DWORD dwKeySize,         // Size of key as in remote dongle (TRU key).crypt=aes128(16 byte).
                                    const void* pKey);       // Pointer to same key as in remote dongle (TRU key).crypt=aes128(16 byte).

int GRD_API GrdTRU_EncryptAnswerEx(HANDLE hGrd,              // Handle to Guardant protected container of dongle that contains
                                                             // encrypt algorithm with the same key as in remote dongle
                                                             // and pre-stored GrdTRU_SetAnswerProperties data if needed
                                   DWORD dwAddr,             // Starting address for writing in dongle
                                   DWORD dwLng,              // Size of data to be written
                                   const void* pData,        // Buffer for data to be written
                                   DWORD dwLngQuestion,      // Size for Question
                                   const void* pQuestion,    // Pointer to decrypted Question
                                   DWORD dwAlgoNum_Encrypt,  // Dongle encrypt algorithm number with the same key as in remote dongle
                                   DWORD dwAlgoNum_Hash,     // Dongle HASH algorithm number with the same key as in remote dongle
                                   void* pAnswer,            // Pointer to the buffer for Answer data
                                   DWORD* pdwAnswerSize,     // IN: Maximum buffer size for Answer data, OUT: Size of pAnswer buffer
                                   DWORD dwMode,             // GrdTRU_CryptMode_GSII64      : crypt=gsII64( 8 byte), hash=based on gsII64(8 byte)
                                                             // GrdTRU_CryptMode_AES128SHA256: crypt=aes128(16 byte), hash=sha256(32 byte)
                                   DWORD dwReserved,         // Reserved, must be 0
                                   void* pReserved);         // Reserved, must be NULL
int GRD_API GrdTRU_EncryptAnswer(HANDLE hGrd,                // Handle to Guardant protected container of dongle that contains
                                                             // GSII64 algorithm with the same key as in remote dongle
                                                             // and pre-stored GrdTRU_SetAnswerProperties data if needed
                                 DWORD dwAddr,               // Starting address for writing in dongle
                                 DWORD dwLng,                // Size of data to be written
                                 const void* pData,          // Buffer for data to be written
                                 const void* pQuestion,      // Pointer to decrypted Question            8 bytes (64 bit)
                                 DWORD dwAlgoNum_GSII64,     // Dongle GSII64 algorithm number with the same key as in remote dongle
                                 DWORD dwAlgoNum_HashS3,     // Dongle HASH64 algorithm number with the same key as in remote dongle
                                 void* pAnswer,              // Pointer to the buffer for Answer data
                                 DWORD* pdwAnswerSize);      // IN: Maximum buffer size for Answer data, OUT: Size of pAnswer buffer

// Apply encrypted answer data buffer received via remote update procedure
int GRD_API GrdTRU_ApplyAnswer(HANDLE hGrd,         // Handle to Guardant protected container of dongle with corresponding pregenerated question
                               const void* pAnswer, // Answer data update buffer prepared and encrypted by GrdTRU_EncryptAnswer
                               DWORD dwAnswerSize); // Size of pAnswer buffer

// Digitally sign a block of bytes by using dongle hardware implemented ECC algorithm
int GRD_API GrdSign(HANDLE hGrd,           // Handle to Guardant protected container
                    DWORD dwAlgoNum,       // Number of hardware implemented ECC algorithm
                    DWORD dwDataLng,       // Size of message to sign (must be equal to GrdECC160_MESSAGE_SIZE for ECC160)
                    const void* pData,     // Pointer to message for sign
                    DWORD dwSignResultLng, // Size of result ECC sign (must be equal to GrdECC160_DIGEST_SIZE for ECC160)
                    void* pSignResult,     // Pointer to result ECC sign
                    void* pReserved);      // Reserved, must be NULL

// ECC algorithm digest verifing. Full software implemented
int GRD_API GrdVerifySign(HANDLE hGrd,            // Handle to Guardant protected container
                          DWORD dwAlgoType,       // Type of asymmetric cryptoalgorithm. See GrdVSC_XXXXX definition
                          DWORD dwPublicKeyLng,   // Size of ECC public key (must be equal to GrdECC160_PUBLIC_KEY_SIZE for ECC160)
                          const void* pPublicKey, // Pointer to ECC public key
                          DWORD dwDataLng,        // Size of original message (must be equal to GrdECC160_MESSAGE_SIZE for ECC160)
                          const void* pData,      // Pointer to original message
                          DWORD dwSignLng,        // Size of ECC sign (must be equal to GrdECC160_DIGEST_SIZE for ECC160)
                          const void* pSign,      // Pointer to ECC sign
                          void* pReserved);       // Reserved, must be NULL

// Set dongle system time
int GRD_API GrdSetTime(HANDLE hGrd,                          // Handle to Guardant protected container
                       const TGrdSystemTime* pGrdSystemTime, // Pointer to TGrdSystemTime
                       void* pReserved);                     // Reserved, must be NULL

// Get dongle system time
int GRD_API GrdGetTime(HANDLE hGrd,                    // Handle to Guardant protected container
                       TGrdSystemTime* pGrdSystemTime, // Pointer to TGrdSystemTime
                       void* pReserved);               // Reserved, must be NULL

// Get time limit for specified item
int GRD_API GrdPI_GetTimeLimit(HANDLE hGrd,                    // Handle to Guardant protected container
                               DWORD dwItemNum,                // Algorithm or Protected Item number
                               TGrdSystemTime* pGrdSystemTime, // Pointer to TGrdSystemTime
                               void* pReserved);               // Reserved, must be NULL

// Get counter for specified item
int GRD_API GrdPI_GetCounter(HANDLE hGrd,        // Handle to Guardant protected container
                             DWORD dwItemNum,    // Algorithm or Protected Item number
                             DWORD* pdwCounter,  // Pointer to counter value
                             void* pReserved);   // Reserved, must be NULL

// Create a system time from components
int GRD_API GrdMakeSystemTime(HANDLE hGrd,                     // Handle to Guardant protected container
                              WORD wYear,                      // The year (2000 - 2099)
                              WORD wMonth,                     // The month (January = 1, February = 2, ...)
                              WORD wDayOfWeek,                 // The day of the week (Sunday = 0, Monday = 1, ...)
                              WORD wDay,                       // The day of the month (1-31)
                              WORD wHour,                      // The hour (0-23)
                              WORD wMinute,                    // The minute (0-59)
                              WORD wSecond,                    // The second (0-59)
                              WORD wMilliseconds,              // The millisecond (0-999)
                              TGrdSystemTime* pGrdSystemTime); // Pointer to destination system time

// Break a system time into components
int GRD_API GrdSplitSystemTime(HANDLE hGrd,                          // Handle to Guardant protected container
                               const TGrdSystemTime* pGrdSystemTime, // Pointer to source system time
                               WORD* pwYear,                         // Pointer for return year value
                               WORD* pwMonth,                        // Pointer for return month value
                               WORD* pwDayOfWeek,                    // Pointer for return day of the week value
                               WORD* pwDay,                          // Pointer for return day of the month value
                               WORD* pwHour,                         // Pointer for return hour value
                               WORD* pwMinute,                       // Pointer for return minute value
                               WORD* pwSecond,                       // Pointer for return second value
                               WORD* pwMilliseconds);                // Pointer for return millisecond value

// Get information from user-defined loadable code descriptor
int GRD_API GrdCodeGetInfo(HANDLE hGrd,          // Handle to Guardant protected container
                           DWORD dwAlgoName,     // Algorithm numerical name to be loaded
                           DWORD dwLng,          // Number of bytes for reading (size of TGrdCodeInfo)
                           void* pBuf,           // Buffer for data to be read (pointer to TGrdCodeInfo)
                           void* pReserved);     // Reserved

// Load GCEXE file to the dongle
int GRD_API GrdCodeLoad(HANDLE hGrd,             // Handle to Guardant protected container
                        DWORD dwAlgoName,        // Algorithm numerical name to be loaded
                        DWORD dwFileSize,        // Buffer size for GCEXE-file
                        const void* pFileBuf,    // Pointer to the buffer for GCEXE-file
                        void* pReserved);        // Reserved

// Run user-defined loadable code
int GRD_API GrdCodeRun(HANDLE hGrd,               // Handle to Guardant protected container
                       DWORD dwAlgoName,          // Algorithm numerical name to be loaded
                       DWORD dwP1,                // Parameter (subfunction code) for loadable code
                       DWORD* pdwRet,             // Return code of loadable code
                       DWORD dwDataFromDongleLng, // The amount of data to be received from the dongle
                       void* pDataFromDongle,     // Pointer to a buffer for data to be received from the dongle
                       DWORD dwDataToDongleLng,   // The amount of data to be sent to the dongle
                       const void* pDataToDongle, // Pointer to a buffer for data to be sent to the dongle
                       void* pReserved);          // Reserved

// Switching driver type of USB-dongle
int GRD_API GrdSetDriverMode(HANDLE hGrd,        // Handle to Guardant protected container
                             DWORD dwMode,       // New Guardant dongle USB driver mode. See GrdDM_XXX definitions
                             void* pReserved);   // Reserved

#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64)

// --- Guardant Dongle Notification API ---

// Initialize Dongle Notification API
int GRD_API GrdInitializeNotificationAPI(void);

// Register dongle notification for specified handle
int GRD_API GrdRegisterDongleNotification(HANDLE hGrd,                         // Handle to Guardant protected container
                                          GrdDongleNotifyCallBack* pCallBack); // Pointer to Dongle Notification CallBack routine

// Unregister dongle notification for specified handle
int GRD_API GrdUnRegisterDongleNotification(HANDLE hGrd); // Handle to Guardant protected container

// Uninitialize Dongle Notification API
int GRD_API GrdUnInitializeNotificationAPI(void);

#endif // __WIN32__ || _WIN32 || WIN32 || _WIN64

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !__GRDAPI__H_
