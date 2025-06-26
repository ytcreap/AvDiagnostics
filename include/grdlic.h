#ifndef __GRD_GRDLIC_API__
#define __GRD_GRDLIC_API__

#if defined(_WIN32_WCE)
#define GRD_API             __cdecl
#elif defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64)
#define GRD_API             __stdcall
#elif defined(__GNUC__)
#define GRD_API
#else
#error "This Guardant API supports 32/x64-bit compilers only"
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 grd_uint64;
typedef signed __int64 grd_int64;
#else
typedef unsigned long long grd_uint64;
typedef signed long long grd_int64;
#endif

#if defined(_MSC_VER)
typedef unsigned long grd_uint32;
typedef signed long grd_int32;
#else
typedef unsigned int grd_uint32;
typedef signed int grd_int32;
#endif

typedef unsigned short grd_uint16;
typedef signed short grd_int16;
typedef unsigned char grd_uint8;
typedef signed char grd_int8;

typedef long grd_time_t;

typedef grd_uint32 GrdHandle;
#define GRD_INVALID_HANDLE_VALUE 0xFFFFFFFF

enum GrdStatus
{
    GRD_OK                                          = 0,       // Successful operation status
    GRD_INVALID_HANDLE                              = 1,       // Invalid handle
    GRD_FEATURE_NOT_FOUND                           = 2,       // Feature not found
    GRD_FEATURE_RESOURCE_EXHAUST                    = 3,       // Feature network resource exhausted
    GRD_INVALID_FEATURE                             = 4,       // Invalid feature data
    GRD_INVALID_SERIAL_NUMBER                       = 5,       // Serial number is invalid
    GRD_INVALID_LICENSE                             = 6,       // Can't read DL license file because of hardware mismatched or part of file header is corrupted
    GRD_LICENSE_NOT_ACTIVATED                       = 7,       // License with this licenseId has not been activated yet or reactivation required
    GRD_LICENSE_ALREADY_ACTIVATED                   = 8,       // License already activated
    GRD_GET_HARDWARE_ID_ERROR                       = 9,       // Can not get hardware id
    GRD_NOT_ENOUGH_HARWARE_TO_BIND                  = 10,      // Insufficient number of computer components to activate
    GRD_REQUEST_FAILED                              = 12,      // Unable to send request
    GRD_GET_RESPONSE_FAILED                         = 13,      // Unable to receive response
    GRD_SERVER_NOT_FOUND                            = 14,      // Unable to find server
    GRD_INCORRECT_RESPONSE_DATA                     = 15,      // Incorrect response format or data not valid
    GRD_FILE_CREATION_ERROR                         = 16,      // Error during creation of file (e.g. license file)
    GRD_SERIAL_NUMBER_RESOURCE_EXHAUSTED            = 17,      // Serial number resource exhausted
    GRD_NO_UPDATES_AVAILABLE                        = 18,      // No updates available for license
    GRD_ACTIVATION_NOT_AVAILABLE                    = 19,      // Activation not enabled by software vendor reasons
    GRD_NO_CUSTOMER_SPECIFIED                       = 20,      // No customer info specified
    GRD_CAN_NOT_ACTIVATE_NON_DEMO_LICENSE           = 21,      // Can't activate non demo license
    GRD_OUTDATED_API_VERSION                        = 22,      // API version too low
    GRD_INVALID_PARAMETER                           = 23,      // Invalid function argument
    GRD_BUFFER_TOO_SMALL                            = 24,      // Buffer is not enough to perform operation
    GRD_INTERNAL_ERROR                              = 25,      // Internal error
    GRD_LICENSE_IS_BANNED                           = 26,      // Software vendor has banned serial number and license
    GRD_NO_LICENSE_AVAILABLE                        = 27,      // No license available (e.g. order access time is over)
    GRD_FEATURE_RUNCOUNTER_EXHAUST                  = 28,      // Feature execution counter is exhausted
    GRD_FEATURE_EXPIRED                             = 29,      // Feature life time is expired
    GRD_INVALID_PUBLIC_KEY                          = 30,      // Invalid public key
    GRD_NO_SERVICE                                  = 31,      // Service is not supported
    GRD_UNABLE_SEND_REQUEST_TO_ADMIN_RUNTIME        = 32,      // Guardant Control Center is not installed or outdated
    GRD_DONGLE_NOT_FOUND                            = 33,      // Guardant dongle not found
    GRD_NO_RESULTS_FOUND                            = 34,      // No any results found
    GRD_INVALID_DIGEST                              = 35,      // Invalid message or digest
    GRD_FEATURE_INACTIVE                            = 36,      // The period of using feature has not yet arrived or the feature is not active
    GRD_MEMORY_OUT_OF_RANGE                         = 37,      // Access to memory beyond its range
    GRD_ACCESS_DENIED                               = 38,      // Access denied
    GRD_NUMBER_ATTEMPTS_EXHAUSTED                   = 39,      // The number of password attempts has been exhausted
    GRD_SERIAL_NUMBER_NOT_FOUND                     = 40,      // Such serial number not found on server
    GRD_INVALID_MEMORY                              = 41,      // Invalid memory ID specified or item does not exist
    GRD_VIRTUAL_ENVIRONMENT_DETECTED                = 42,      // Running in virtual environment detected
    GRD_REMOTE_DESKTOP_DETECTED                     = 43,      // Detected running in remote desktop mode
    GRD_OUTDATED_GCC_VERSION                        = 44,      // Outdated Gurdant Control Center version
    GRD_REHOST_IS_NOT_ALLOWED                       = 45,      // Rehost is not allowed
    GRD_STORAGE_CORRUPTED                           = 46,      // DL storage corrupted
    GRD_STORAGE_ID_MISMATCH                         = 47,      // DL storage ID mismatch (input is not intended for current storage)
    GRD_INVALID_UPDATE_OR_ALREADY_INSTALLED         = 48,      // Invalid update for DL received or update has been installed before
    GRD_INVALID_REHOST_DATA                         = 49,      // Invalid rehost datа sent to server
    GRD_HARDWARE_ID_MISMATCH                        = 50,      // Hardware ID mismatch
    GRD_REACTIVATION_IS_NOT_ALLOWED                 = 51,      // Reactivation is not allowed because of license contains write memory or features with counter
    GRD_RECIPIENT_HAS_THE_SAME_LICENSE              = 52,      // Recipient has the same license activated as the license trying to rehost to it
    GRD_CAN_NOT_DETACH_SOME_FEATURES                = 53,      // Detach failed for some of chosen features
    GRD_CANCEL_PREVIOUS_DETACH_REQUIRED             = 54,      // Required cancel previous detach before making new one
    GRD_DETACHED_LICENSE_EXPIRED                    = 55,      // Detached license already expired
    GRD_DETACHED_LICENSE_NOT_FOUND                  = 56,      // Detached license not found
    GRD_DETACH_FORBIDDEN_BY_GCC                     = 57,      // Detach forbidden by Gurdant Control Center
    GRD_TRIAL_LICENSE_EXPIRED                       = 58,      // Trial license expired
    GRD_OPERATION_IS_NOT_ALLOWED                    = 59,      // Operation is not allowed
    GRD_RTC_ERROR                                   = 60,      // Timer error
};

enum GrdRemoteMode
{
    GRD_LOCAL_MODE_ON  = 0x01,
    GRD_REMOTE_MODE_ON = 0x02
};

enum GrdDongleModel
{
    GRD_DONGLE_MODEL_SIGN     = 0x80,
    GRD_DONGLE_MODEL_CODE     = 0x100,
    GRD_DONGLE_MODEL_DL       = 0x400
};

enum GrdDriverType
{
    GRD_DRIVER_TYPE_NONE = 0,
    GRD_DRIVER_TYPE_HID = 1,
    GRD_DRIVER_TYPE_WINUSB = 2,
    GRD_DRIVER_TYPE_VENDOR = 3
};

enum GrdFirmwareState
{
    GRD_FIRMWARE_STATE_OK = 0,
    GRD_FIRMWARE_STATE_CRC_ERROR = 1,
    GRD_REMOTE_UPDATE_MODE_ON = 2,
    GRD_FIRMWARE_STATE_TIMER_FREQUENCY_ERROR = 4,
    GRD_FIRMWARE_STATE_TIMER_INTEGRITY_ERROR = 8,
    GRD_FIRMWARE_STATE_TIMER_BATTERY_ERROR = 16,
    GRD_FIRMWARE_STATE_TIMER_COMMINICATION_ERROR = 32,
    GRD_FIRMWARE_STATE_TIMER_STOPPED = 64,
    GRD_FIRMWARE_STATE_VTC_RUNNING = 128
};

enum GrdConsumptionMode
{
    GRD_CONSUMPTION_MODE_PERSTATION    = 0x00,
    GRD_CONSUMPTION_MODE_PERLOGIN      = 0x01,
    GRD_CONSUMPTION_MODE_PERPROCESS    = 0x02
};

enum GrdEcc160
{
    GRD_ECC160_MESSAGE_SIZE = 20,
    GRD_ECC160_PUBLIC_KEY_SIZE = 40,
    GRD_ECC160_DIGEST_SIZE = 40
};

enum GrdFeatureEncryptMode
{
    GRD_EM_ECB                = 0x00,       // Electronic Code Book encryption mode (set by default). Data size must be multiple of 16
    GRD_EM_CBC                = 0x01,       // Cipher Block Chaining encryption mode. Data size must be multiple of 16
    GRD_EM_CFB                = 0x02,       // Cipher Feed Back encryption mode. Is allowed only if GRD_SOFTWARE_ACCELERATION flag is provided
    GRD_EM_OFB                = 0x04,       // Output Feed Back encryption mode. Is allowed only if GRD_SOFTWARE_ACCELERATION flag is provided
    GRD_NO_COUNTER_DECREMENT  = 0x100,      // Execute encryption/decryption without counter decrement (used another cryptographic key)
    GRD_SOFTWARE_ACCELERATION = 0x200       // Software acceleration for encryption/decryption of a large amount of data
};

enum GrdFeatureLicenseType
{
    GRD_PERPETUAL             = 0x01,       // License without restrictions
    GRD_TIME_PERIOD           = 0x02,       // License with a period of time
    GRD_EXPIRATION_DATE       = 0x03,       // License with expiration date
    GRD_NUMBER_OF_DAYS        = 0x04,       // License with the expiration of a given number of days
    GRD_EXECUTIONS            = 0x05        // License with a limited number of launches
};

enum GrdFeatureFlags
{
    FEATURE_VM_FORBIDDEN      = 0x04,           // Work on VM is prohibited
    FEATURE_VTC_FORBIDDEN     = 0x20,           // Virtual timer clock is disabled
    FEATURE_RDP_FORBIDDEN     = 0x0100,         // Work on RD is prohibited
    FEATURE_IS_DETACHABLE     = 0x0800,         // Marked as detachable
    FEATURE_DETACH_ALLOWED    = 0x1000          // Detaching allowed
};

enum GrdMemoryWriteMode
{
    GRD_WRITE_MODE_MOV = 0x00,       // Destination becomes source
    GRD_WRITE_MODE_XOR = 0x01        // Destination becomes source XORed with destination
};

// for getting formatted error message
enum GrdLanguageId
{
    GRD_LANG_EN = 0,
    GRD_LANG_RU = 1,
};

typedef struct
{
    /// @brief Vendor public code necessary for finding feature (0 means param is not used)
    grd_uint32 publicCode;
    /// @brief Vendor private read code necessary for access to feature memory (0 means param is not used)
    grd_uint32 privateReadCode;
    /// @brief Vendor private write code allow to change feature (0 means param is not used)
    /// Generally, this param must not be specified
    grd_uint32 privateWriteCode;

} GrdVendorCodes;


/// Feature search option (visibility) example (in JSON format). All fields are optional
/*
   {
    // Perform feature search only inside of specified license. By default, the feature search is performed in all licenses
    "licenseId": 0x12345678,
    // The mask that specifying what kind of features can be found - local (0x01), remote (0x02) or both (0x03). See GrdRemoteMode enumeration.
    // By default, the search is performed for all features
    "remoteMode": 0x03,
    // The mask for specifying the dongle models to which feature can be bound to (0 means all models). See GrdDongleModel enumeration.
    // By default, the feature search is performed in all dongle models
    "dongleModel": 0x400,
    // The number of the product to which feature bound to (0 means all products). By default, the feature search is performed in all products
    "productNumber": 777,
    // Number of network resource to consume. By default, 1
    "networkResourceToConsume" : 1,
    // Configuration for Guardant Control Center
    "controlCenter":
    {
        // HostName or IP address of the computer on which Guardant Control Center is running
       "hostName": [ "192.168.77.77" ],
       // Broadcast search for remote licenses (enabled by default)
       "broadcastSearch" : true,
    }
   }
 */

#pragma pack(push, 1)

typedef struct
{
    char firstName[64];
    char lastName[64];
    char email[128];
    char companyName[128];
    char phone[32];
} GrdCustomerInfo;

typedef struct
{
    grd_uint8 iv[16]; // initialization vector
    grd_uint8 inf[8]; // additional encryption info
} GrdAesContext;

typedef struct
{
    grd_uint32 productNumber;
    grd_uint32 featureNumber;
} GrdProductFeaturePair;

#pragma pack(pop)

/// @brief Retrieves Licensing API version
///   @param majorVersion - version major value
///   @param minorVersion - version minor value
///   @return Returns compound version value: (majorVersion << 16) + minorVersion
grd_uint32 GRD_API GrdGetApiVersion(grd_uint32* majorVersion, grd_uint32* minorVersion);
/// @brief Retrieves error message for corresponding errorCode
///   @param errorCode - is the value of GrdStatus returned one of API function call for which error message string is requested
///   @param lang - is the language ID (type of GrdLanguageId) of the output error message string
///   @param msgBuf - allocated buffer for output error message string (null-terminated)
///   @param msgBufSize - the size of msgBuf
///   @return Returns GrdStatus
int GRD_API GrdGetErrorMessage(int errorCode, int lang, char* msgBuf, grd_uint32 msgBufSize);
/// @brief Retrieves information about all available licenses
/// @details This function allocates memory for the information it retrieves. To release allocated memory,
/// use the GrdFree function.
///   @param visibility [optional] - feature search options in JSON format (see visibility example)
///   @param codes [optional] - vendor codes specified by VendorCodes structure
///   @param licenseInfoJsonArray - a pointer to return a pointer for license information in json format
///   @return Returns GrdStatus
int GRD_API GrdGetLicenseInfo(const char* visibilityJson,
                              const GrdVendorCodes* codes,
                              char** licenseInfoJsonArray);
/// @brief Retrieves information about all remote sessions
/// @details This function allocates memory for the information it retrieves. To release allocated memory,
/// use the GrdFree function.
///   @param visibility [optional] - license search options in JSON format (see visibility example)
///   @param codes [optional] - vendor codes specified by VendorCodes structure
///   @param sessionInfoJsonArray - a pointer to return a pointer for session information in json format
///   @return Returns GrdStatus
int GRD_API GrdGetSessionInfo(const char* visibilityJson,
                              const GrdVendorCodes* codes,
                              char** sessionInfoJsonArray);
/// @brief Returns host information about all available hosts where the GCC is running.
/// @details This function allocates memory for the information it retrieves. To release allocated memory,
/// use the GrdFree function.
///   @param visibility [optional] - GCC search options in JSON format (see visibility example)
///   @param hostInfoJsonArray - a pointer to return a pointer for host information in json format
///   @return Returns GrdStatus
int GRD_API GrdGetHostInfo(const char* visibilityJson, char** hostInfoJsonArray);
/// @brief Logs into the feature
///   @param featureId - id of the feature to log in
///   @param codes - vendor codes specified by VendorCodes structure
///   @param visibility [optional] - feature search options in JSON format (see visibility example)
///   @param handle - handle of the feature
///   @return Returns GrdStatus
int GRD_API GrdFeatureLogin(grd_uint32 featureId,
                            const GrdVendorCodes* codes,
                            const char* visibilityJson,
                            GrdHandle* handle);
/// @brief Performs feature logout
///   @param handle - handle of the feature
///   @return Returns GrdStatus
int GRD_API GrdFeatureLogout(GrdHandle handle);
/// @brief Performs feature check
///   @param handle - handle of the feature
///   @return Returns GrdStatus
int GRD_API GrdFeatureCheck(GrdHandle handle, grd_uint32 publicKeySize, const void* publicKey);
/// @brief Retrieves information about logged in feature
/// @details This function allocates memory for the information it retrieves. To release allocated memory,
///   use the GrdFree function.
///   @param handle - handle of the feature
///   @param outLicenseInfoJson - pointer to return pointer for feature information in json format
///   @return Returns GrdStatus
int GRD_API GrdFeatureGetInfo(GrdHandle handle, char** outLicenseInfoJson);
/// @brief Performs encryption by AES128
/// @details If a licensed restriction on the number of executions is used, this function subtracts the
/// feature run counter value. If GRD_NO_COUNTER_DECREMENT flag is set, this function uses a different
/// encryption key and does not use an execution counter restriction.
///   @param handle - handle of feature you are currently logged in
///   @param dataSize - size of buffer to encrypt in bytes. The minimum size is 16.
///   @param data - buffer to encrypt
///   @param mode - encryption mode (GRD_EM_CFB and GRD_EM_OFB modes are available only if GRD_SOFTWARE_ACCELERATION flag is provided)
///   @param context - pointer to GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be NULL
///   @param contextSize - the size of GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be 0.
///   @return Returns GrdStatus
int GRD_API GrdFeatureEncrypt(GrdHandle handle, grd_uint32 dataSize, void* data, grd_uint32 mode, void* context, grd_uint32 contextSize);
/// @brief Performs decryption by AES128
/// @details If a licensed restriction on the number of executions is used, this function subtracts the
/// feature run counter value. If GRD_NO_COUNTER_DECREMENT flag is set, this function uses a different
/// encryption key and does not use an execution counter restriction.
///   @param handle - handle of feature you are currently logged in
///   @param dataSize - size of buffer to decrypt in bytes. The minimum size is 16.
///   @param data - buffer to decrypt
///   @param mode - decryption mode
///   @param context - pointer to GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be NULL
///   @param contextSize - the size of GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be 0.
///   @return Returns GrdStatus
int GRD_API GrdFeatureDecrypt(GrdHandle handle, grd_uint32 dataSize, void* data, grd_uint32 mode, void* context, grd_uint32 contextSize);
/// @brief Performs AES128 encryption (by default) or decryption and decrement counter by n units
/// @details If a licensed has no restriction on the number of executions - just encryption/decryption will be performed
/// If GRD_NO_COUNTER_DECREMENT flag is set, this function uses a different encryption key and does not use an execution counter restriction.
///   @param handle - handle of feature you are currently logged in
///   @param dataSize - size of buffer to decrypt in bytes. The minimum size is 16.
///   @param data - buffer to decrypt
///   @param mode - decryption mode
///   @param context - pointer to GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be NULL
///   @param contextSize - the size of GrdAesContext. Not used for GRD_EM_ECB encryption mode and can be 0.
///   @param decryption - make the function do decryption instead of encryption
///   @param n  -  the amount of counter units to decrement
///   @param flags - reserved for future
///   @return Returns GrdStatus
int GRD_API GrdFeatureCryptAndCount(GrdHandle handle, grd_uint32 dataSize, void* data, grd_uint32 mode, void* context, grd_uint32 contextSize,
                                    int decryption,
                                    grd_uint32 n,
                                    grd_uint32 flags);
/// @brief Performs digital sign by ECC160
/// @details The function first computes the SHA2-256 hash from the message being signed
///   @param handle - handle of feature you are currently logged in
///   @param messageSize - size of message to sign in bytes
///   @param message - pointer to buffer that must contain message to sign
///   @param digestSize - size of buffer that will contain the digest in bytes, must be GRD_ECC160_DIGEST_SIZE
///   @param digest - pointer to buffer that will contain the digest in bytes
///   @return Returns GrdStatus
int GRD_API GrdFeatureSign(GrdHandle handle,
                           grd_uint32 messageSize, const void* message,
                           grd_uint32 digestSize, void* digest);
/// @brief Performs digital sign verification by ECC160
/// @details The function first computes the SHA2-256 hash from the message being verified
///   @param publicKeySize - size of public key in bytes, must be GRD_ECC160_PUBLIC_KEY_SIZE
///   @param publicKey - pointer to buffer that must contain public key
///   @param messageSize - size of message to verify in bytes
///   @param message - pointer to buffer that must contain message to verify
///   @param digestSize - size of digest in bytes, must be GRD_ECC160_DIGEST_SIZE
///   @param digest - pointer to buffer that must contain digest to verify
///   @return Returns GrdStatus
int GRD_API GrdVerifyDigest(grd_uint32 publicKeySize, const void* publicKey,
                            grd_uint32 messageSize, const void* message,
                            grd_uint32 digestSize, const void* digest);
/// @brief Returns feature time limit in seconds
///   @param handle - handle of the feature
///   @param remainingTime - pointer to return value
///   @return Returns GrdStatus
int GRD_API GrdFeatureGetTimeLimit(GrdHandle handle, grd_time_t* remainingTime);
/// @brief Returns dongle RTC time
///   @param handle - handle of the feature
///   @param realTime - pointer grd_time_t
///   @return Returns GrdStatus
int GRD_API GrdGetRealTime(GrdHandle handle, grd_time_t* realTime);
/// @brief Returns feature max concurrent resource (aka Net resource)
///   @param handle - handle of the feature
///   @param maxResource - pointer to return value
///   @return Returns GrdStatus
int GRD_API GrdFeatureGetMaxConcurrentResource(GrdHandle handle, grd_uint32* maxResource);
/// @brief Returns feature current execution counter
///   @param handle - handle of the feature
///   @param runCounter - pointer to return value
///   @return Returns GrdStatus
int GRD_API GrdFeatureGetRunCounter(GrdHandle handle, grd_uint32* runCounter);
/// @brief Returns data size of memory specified by memoryId
///   @param handle - handle of the feature
///   @param memoryId - id of the memory
///   @param size - the size of data in memory in bytes
///   @return Returns GrdStatus
int GRD_API GrdMemoryGetSize(GrdHandle handle, grd_uint32 memoryId, grd_uint32* size);
/// @brief Reads data from memory
///   @param handle - handle of the feature
///   @param memoryId - id of the memory to read
///   @param password - optional read password. If not used, must be 0
///   @param dataSize - number of bytes for reading
///   @param offset - start offset in memory for reading
///   @param data - pointer to buffer for read data
///   @return Returns GrdStatus
int GRD_API GrdMemoryRead(GrdHandle handle, grd_uint32 memoryId, grd_uint32 password, grd_uint32 dataSize, grd_uint32 offset, void* data);
/// @brief Writes data to memory
///   @param handle - handle of the feature
///   @param memoryId - id of the memory to write
///   @param password - optional write password. If not used, must be 0
///   @param mode - memory write mode (GRD_WRITE_MODE_MOV or GRD_WRITE_MODE_XOR)
///   @param dataSize - number of bytes for writing
///   @param offset - start offset in memory for writing
///   @param data - pointer to buffer that contains data for writing
///   @return Returns GrdStatus
int GRD_API GrdMemoryWrite(GrdHandle handle, grd_uint32 memoryId, grd_uint32 password, grd_uint32 mode, grd_uint32 dataSize, grd_uint32 offset, const void* data);
/// @brief Turns on the LED blinking on the dongle with the given identifier
///   @param dongleId - dongle identifier
///   @return Returns GrdStatus
int GRD_API GrdLedBlink(grd_uint32 dongleId);
/// @brief Toggles a dongle to use a different type of device driver
///   @param codes - vendor codes specified by VendorCodes structure
///   @param dongleId - dongle identifier
///   @param driverType - new type of devie driver (see GrdDriverType enumeration)
///   @return Returns GrdStatus
int GRD_API GrdSetDriverType(const GrdVendorCodes* codes, grd_uint32 dongleId, grd_uint32 driverType);
/// @brief Returns a fingerprint for the local computer or any computer on the network where the GCC is running.
/// @details To release allocated memory, use the GrdFree function.
///   @param hostFingerprintJsonArray - a pointer to return a pointer for fingerprint array in json format
///   @param visibility [optional] - GCC search options in JSON format (see visibility example, NULL for local computer also permitted)
///   @return Returns GrdStatus
int GRD_API GrdGetHostFingerprint(char** hostFingerprintJsonArray, const char* visibilityJson);
/// @brief Checks the serial number for correctness
///   @param serialNumber - pointer to a string that contains a serial number
///   @return Returns GrdStatus
int GRD_API GrdLicenseCheckSerialNumberFormat(const char* serialNumber);
/// @brief Activates Guardant DL by serial number (online mode)
///   @param serialNumber - pointer to a string that contains a serial number
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @param customerInfo - a pointer to an optional customer information. Can be NULL
///   @param outLicenseId - a pointer to return value of activated license id
///   @return Returns GrdStatus
int GRD_API GrdLicenseActivate(const char* serialNumber,
                               const char* host,
                               grd_uint32 port,
                               const GrdCustomerInfo* customerInfo,
                               grd_uint32* outLicenseId);
/// @brief Updates Guardant DL or hardware dongle by license id (online mode)
///   @param licenseId - previously activated license id value or dongle id value
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @return Returns GrdStatus
int GRD_API GrdLicenseUpdate(grd_uint32 licenseId, const char* host, grd_uint32 port);
/// @brief Checks whether update for Guardant DL license or hardware dongle is available (online mode)
///   @param licenseId - previously activated license id value or dongle id value
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @return Returns GrdStatus
int GRD_API GrdLicenseCheckUpdateIsAvailable(grd_uint32 licenseId, const char* host, grd_uint32 port);
/// @brief Remove Guardant DL by license id
///   @param licenseId - previously activated license id value
///   @return Returns GrdStatus
int GRD_API GrdLicenseRemove(grd_uint32 licenseId);
/// @brief Checks whether license is not banned by software vendor (online mode)
///   @param licenseId - previously activated license id value
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @return Returns GrdStatus
int GRD_API GrdLicenseCheckIsNotBanned(grd_uint32 licenseId, const char* host, grd_uint32 port);
/// @brief Creates license activation request for sending to activation server (offline mode).
/// @details The purpose of this function is to get computer hardware fingerprint that is needed for getting a new
/// license or the license rehosted from another computer. Must be executed on a computer without Internet access. This function allocates memory for the
/// request it retrieves. To release allocated memory, use the GrdFree function.
///   @param outRequest - a pointer to return a pointer to the buffer that receives a request that must be sent to
///   the activation server from a computer with Internet access
///   @param outRequestSize - a pointer to return value of request buffer size in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseCreateActivationRequest(void** outRequest, grd_uint32* outRequestSize);
/// @brief Sends license activation request to activation server and getting response from it (offline mode)
/// @details Must be executed on a computer with Internet access. This function allocates memory for the
/// response it retrieves. To release allocated memory, use the GrdFree function.
///   @param serialNumber - pointer to a string that contains a serial number
///   @param customerInfo - a pointer to an optional customer information. Can be NULL
///   @param request - a pointer to a request from a computer without Internet access
///   @param requestSize - size of request in bytes
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @param outResponse -  a pointer to return a pointer to the buffer that receives a response from
///   activation server and must be installed on a computer without Internet access
///   @param outResponseSize - a pointer to return value of response buffer size in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseSendActivationRequest(const char* serialNumber, GrdCustomerInfo* customerInfo,
                                            const void* request, grd_uint32 requestSize,
                                            const char* host, grd_uint32 port,
                                            void** outResponse, grd_uint32* outResponseSize);
/// @brief Creates license update request for sending to activation server (offline mode)
/// @details Must be executed on a computer without Internet access. This function allocates memory for the
/// request it retrieves. To release allocated memory, use the GrdFree function.
///   @param licenseId - previously activated license id or dongle id for hardware dongles
///   @param outRequest - a pointer to return a pointer to the buffer that receives a request that must be sent to
///   the activation server from a computer with Internet access
///   @param outRequestSize - a pointer to return value of request buffer size in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseCreateUpdateRequest(grd_uint32 licenseId, void** outRequest, grd_uint32* outRequestSize);
/// @brief Sends license update request to activation server and getting response from it (offline mode)
/// @details Must be executed on a computer with Internet access. This function allocates memory for the
/// response it retrieves. To release allocated memory, use the GrdFree function.
///   @param request - a pointer to a request from a computer without Internet access
///   @param requestSize - size of request in bytes
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @param outResponse - a pointer to a buffer that receives a response from activation server that must be
///   installed on a computer withouth Internet access
///   @param outResponseSize - a pointer to return value of response buffer size in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseSendUpdateRequest(const void* request, grd_uint32 requestSize,
                                        const char* host, grd_uint32 port,
                                        void** outResponse, grd_uint32* outResponseSize);

/// @brief Retrieve license from the computer for future rehost
/// @details After this functions complete with success - current license will no more be available on this PC
/// (if not rehosted on this in the future or not activated one more from the serial with multiple activations).
/// This function allocates memory for the outDonorLicense. To release allocated memory, use the GrdFree function.
///   @param licenseId - previously activated license id value
///   @param outDonorLicense - a pointer to a buffer that containes the current license (in base64 format)
///   @param outDonorLicenseSize - a pointer to outDonorLicense buffer size in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseExtractForRehost(grd_uint32 licenseId, void** outDonorLicense, grd_uint32* outDonorLicenseSize);
/// @brief Rehosts (installs) the license from the donor computer to the recipient computer
/// @details Must be called on the recipient computer with Internet access.
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @param donorLicense - a pointer to a buffer that containes the license from the donor computer (in base64 format)
///   @param donorLicenseSize - size of donorLicense buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseRehost(const char* host, grd_uint32 port,
                             const void* donorLicense, grd_uint32 donorLicenseSize);
/// @brief Sends the license from the donor computer and recipient computer fingerprint to the activation server in order to recieve
/// the license for recipient computer in response
/// @details Must be called on the computer with Internet access. Returned in recipientLicense buffer can be installed
/// on the recipient computer by GrdLicenseInstall call. This function allocates memory for the recipientLicense.
/// To release allocated memory, use the GrdFree function.
///   @param host - pointer to a string that contains the address of the activation server
///   @param port - port number of the activation server
///   @param donorLicense - a pointer to a buffer that containes the license from the donor computer (in base64 format)
///   @param donorLicenseSize - size of donorLicense buffer in bytes
///   @param recipientHardwareFingerprint - a pointer to a buffer that containes the recipient computer hardware fingerprint obtained by GrdGetHostFingerprint function call
///   @param recipientLicense - a pointer to a buffer that containes the license for the recipient computer (in base64 format)
///   @param recipientLicenseSize - size of recipientLicense buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseSendRehostRequest(const char* host, grd_uint32 port,
                                        const void* donorLicense, grd_uint32 donorLicenseSize,
                                        const char* recipientHardwareFingerprint,
                                        void** recipientLicense, grd_uint32* recipientLicenseSize);
/// @brief Detaches the features (if they are detachable) from the computer where the network license is installed to recipient computer
/// @details Returned in recipientLicense buffer can be installed
/// on the recipient computer by GrdLicenseInstall call. This function allocates memory for the recipientLicense.
/// To release allocated memory, use the GrdFree function.
///   @param visibility [optional] - remote license search options in JSON format (see visibility example)
///   @param featuresList - the array of feature numbers (and related theirs product numbers) those are intended for detach
///   @param featuresCount - the count of features numbers in the featuresList
///   @param durationInSec - lifetime of the detached license in seconds (after this time expire - the license will be returned to license pool automatically)
///   @param concurrentResourceToConsume - defines how much network features resource to consume from the main license
///   @param recipientHardwareFingerprint - a string that containes the recipient computer hardware fingerprint
///   @param recipientLicense - a pointer to a buffer that containes the license for the recipient computer (in base64 format)
///   @param recipientLicenseSize - size of recipientLicense buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseDetach(const char* visibilityJson, const GrdProductFeaturePair* featuresList, grd_uint32 featuresCount,
                             grd_time_t durationInSec, grd_uint16 concurrentResourceToConsume,
                             const char* recipientHardwareFingerprint,
                             void** recipientLicense, grd_uint32* recipientLicenseSize);
/// @brief Call this function to get data for returning detached license to the license pool before its lifetime is expired
/// @details Returned in licenseToReturn buffer can be installed
/// on the computer from which license was detached by GrdLicenseInstall call. This function allocates memory for the licenseToReturn.
/// To release allocated memory, use the GrdFree function.
///   @param licenseId - the ID of the license to detach
///   @param licenseToReturn - a pointer to a buffer that containes the license to return (in base64 format). This can be NULL if detached license is already expired
///   @param licenseToReturnSize - size of licenseToReturn buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseReturnToPool(grd_uint32 licenseId, void** licenseToReturn, grd_uint32* licenseToReturnSize);
/// @brief installs/update Guardant DL license or update hardware dongle from activation server response (offline mode)
/// or installs rehosted license on the recipient computer
///   @param response - a pointer to the response buffer from the activation server received on a computer
///   with Internet access
///   @param responseSize - size of response buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseInstall(const void* response, grd_uint32 responseSize);
/// @brief Installs/update Guardant DL license or installs rehosted license on the recipient computer remotely
///   @param visibility [optional] - remote GCC search options in JSON format (see visibility example)
///   @param response - a pointer to the response buffer from the activation server received on a computer
///   with Internet access
///   @param responseSize - size of response buffer in bytes
///   @return Returns GrdStatus
int GRD_API GrdLicenseInstallRemotely(const char* visibilityJson, const void* response, grd_uint32 responseSize);
/// @brief Install (or update) license (related with serialNumber) into hardware dongle.
/// This function will be especially usefull when software vendor put no license in the hardware dongle before sending it to the customer.
/// In this case GrdDongleSetLicense function can be used for the most first dongle update only and the next updates can be done by GrdLicenseUpdate function (without knowning serial numbers).
///   @param dongleId - the Id of the hardware dongle that requires specific license (specified by param serialNumber)
///   @param serialNumber - the specific license that is intended for the hardware dongle (specified by param dongleId)
///   @param driverType - the one of GrdDriverType enumeration values that defines default driver type of the hardware dongle when the license will be installed into
///   @param host - string that contains the address of the activation server
///   @param port - port number of the activation server
///   @return Returns GrdStatus
int GRD_API GrdDongleSetLicense(grd_uint32 dongleId, const char* serialNumber, grd_uint32 driverType, const char* host, grd_uint32 port);
/// @brief Releases memory resources utilized by other functions (offline mode)
///   @param buffer - Pointer to memory resources allocated from API calls using the GrdGetLicenseInfo, GrdLicenseCreateUpdateRequest
///   @return The function has no return status code
void GRD_API GrdFree(void* buffer);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __GRD_GRDLIC_API__
