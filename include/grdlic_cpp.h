#pragma once

#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include "grdlic.h"

/// Feature search option (visibility) example (in JSON format). All fields are optional
/*
   {
    // Perform feature search only inside of specified license. By default, the feature search is performed in all licenses
    "licenseId": 0x12345678,
    // The mask that specifying what kind of features can be found - local (0x01), remote (0x02) or both (0x03). See GrdRemoteMode enumeration.
    // By default, the search is performed for all features
    "remoteMode": 3,
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
       // Ipv4 broadcast address (255.255.255.255 by default)
       "broadcastAddress": "255.255.255.255"
    }
   }
 */

namespace guardant
{

struct VendorCodes
{
    VendorCodes(grd_uint32 publicCode, grd_uint32 privateReadCode = 0, grd_uint32 privateWriteCode = 0)
        : m_publicCode(publicCode), m_privateReadCode(privateReadCode), m_privateWriteCode(privateWriteCode) {}

    operator GrdVendorCodes()
    {
        GrdVendorCodes codes = { m_publicCode, m_privateReadCode, m_privateWriteCode };
        return codes;
    }

    grd_uint32 m_publicCode;
    grd_uint32 m_privateReadCode;
    grd_uint32 m_privateWriteCode;
};

enum RemoteMode
{
    LOCAL    = 0x01,
    REMOTE   = 0x02
};

inline RemoteMode operator |(RemoteMode lhs, RemoteMode rhs)
{
    return static_cast<RemoteMode>(
                                   static_cast<unsigned>(lhs) |
                                   static_cast<unsigned>(rhs)
                                   );
}

enum DongleModel
{
    SIGN    = 0x80,
    CODE    = 0x100,
    DL      = 0x400
};

inline DongleModel operator |(DongleModel lhs, DongleModel rhs)
{
    return static_cast<DongleModel>(
                                    static_cast<unsigned>(lhs) |
                                    static_cast<unsigned>(rhs)
                                    );
}

class Exception : public std::runtime_error
{
public:

    Exception(const std::string& error, const std::string& code = std::string())
        : std::runtime_error(error),
        m_code(code)
    {}

    virtual ~Exception() {}

    std::string code() const { return m_code; }
    int status() const { return std::atoi(m_code.c_str()); }
    std::string message() const
    {
        char buf[1024] = { 0 };
        GrdGetErrorMessage(status(), GRD_LANG_EN, buf, 1024);
        return buf;
    }

private:
    std::string m_code;
};

inline void throwIfNeed(int status)
{
    if (status != GRD_OK)
    {
        std::ostringstream ss;
        ss << status;
        throw Exception("Guardant exception", ss.str());
    }
}

class Feature
{
private:
    grd_uint32 m_featureId;
    GrdVendorCodes m_codes;
    std::string m_visibility;
    GrdHandle m_handle;
    char* m_licenseInfo;

public:
    Feature(grd_uint32 featureId, GrdVendorCodes codes, const std::string& visibility) :
        m_featureId(featureId),
        m_codes(codes),
        m_visibility(visibility),
        m_handle(0),
        m_licenseInfo(NULL) {}

    virtual ~Feature()
    {
        if (m_licenseInfo)
            GrdFree(m_licenseInfo);
    }

    void Login()
    {
        throwIfNeed(GrdFeatureLogin(m_featureId, &m_codes, m_visibility.empty() ? NULL : m_visibility.c_str(), & m_handle));
    }
    void Logout()
    {
        throwIfNeed(GrdFeatureLogout(m_handle));
        m_handle = 0;
    }

    void GetInfo(std::string& licenseInfo)
    {
        if (m_licenseInfo)
            GrdFree(m_licenseInfo);
        throwIfNeed(GrdFeatureGetInfo(m_handle, &m_licenseInfo));
        licenseInfo = m_licenseInfo;
    }
    void Encrypt(grd_uint32 dataSize, void* data, GrdFeatureEncryptMode mode, void* context, grd_uint32 contextSize)
    {
        throwIfNeed(GrdFeatureEncrypt(m_handle, dataSize, data, mode, context, contextSize));
    }
    void Decrypt(grd_uint32 dataSize, void* data, GrdFeatureEncryptMode mode, void* context, grd_uint32 contextSize)
    {
        throwIfNeed(GrdFeatureDecrypt(m_handle, dataSize, data, mode, context, contextSize));
    }
    void Sign(grd_uint32 dataSize, const void* data, grd_uint32 signResultSize, void* signResult)
    {
        throwIfNeed(GrdFeatureSign(m_handle, dataSize, data, signResultSize, signResult));
    }
    void VerifyDigest(grd_uint32 pubKeySize, const void* pubKey,
                      grd_uint32 dataSize, const void* data,
                      grd_uint32 signResultSize, const void* signResult)
    {
        throwIfNeed(GrdVerifyDigest(pubKeySize, pubKey, dataSize, data, signResultSize, signResult));
    }
    void GetTimeLimit(grd_time_t& remainingTime)
    {
        throwIfNeed(GrdFeatureGetTimeLimit(m_handle, &remainingTime));
    }
    void GetRealTime(grd_time_t& currentTime)
    {
        throwIfNeed(GrdGetRealTime(m_handle, &currentTime));
    }

    void GetRunCounter(grd_uint32& value)
    {
        throwIfNeed(GrdFeatureGetRunCounter(m_handle, &value));
    }

    void GrdGetMaxConcurrentResource(grd_uint32& value)
    {
        throwIfNeed(GrdFeatureGetMaxConcurrentResource(m_handle, &value));
    }

    void MemoryGetSize(grd_uint32 memoryId, grd_uint32& size)
    {
        throwIfNeed(GrdMemoryGetSize(m_handle, memoryId, &size));
    }

    void MemoryRead(grd_uint32 memoryId, grd_uint32 password, grd_uint32 dataSize, grd_uint32 offset, void* data)
    {
        throwIfNeed(GrdMemoryRead(m_handle, memoryId, password, dataSize, offset, data));
    }

    void MemoryWrite(grd_uint32 memoryId, grd_uint32 password, grd_uint32 mode, grd_uint32 dataSize, grd_uint32 offset, void* data)
    {
        throwIfNeed(GrdMemoryWrite(m_handle, memoryId, password, mode, dataSize, offset, data));
    }
};

inline grd_uint32 GetApiVersion(grd_uint32& major, grd_uint32& minor)
{
    return GrdGetApiVersion(&major, &minor);
}

inline void GetErrorMessage(int errorCode, GrdLanguageId lang, std::string& msg)
{
    char buf[1024] = { 0 };
    throwIfNeed(GrdGetErrorMessage(errorCode, lang, buf, 1024));
    msg = std::string(buf);
}

inline void GetLicenseInfo(const std::string& visibility, VendorCodes* codes, std::string& licenseInfoList)
{
    GrdVendorCodes c;
    GrdVendorCodes* cPtr = NULL;
    char* rawLicenseInfoList;

    if (codes)
    {
        c = (GrdVendorCodes) * codes;
        cPtr = &c;
    }

    throwIfNeed(GrdGetLicenseInfo(visibility.empty() ? NULL : visibility.c_str(), cPtr, & rawLicenseInfoList));

    licenseInfoList = rawLicenseInfoList;
    GrdFree(rawLicenseInfoList);
}

inline void GetSessionInfo(const std::string& visibility, VendorCodes* codes, std::string& sessionInfoList)
{
    GrdVendorCodes c;
    GrdVendorCodes* cPtr = NULL;
    char* rawSessionInfoList = NULL;

    if (codes)
    {
        c = (GrdVendorCodes) * codes;
        cPtr = &c;
    }

    throwIfNeed(GrdGetSessionInfo(visibility.empty() ? NULL : visibility.c_str(), cPtr, & rawSessionInfoList));

    sessionInfoList = rawSessionInfoList;
    GrdFree(rawSessionInfoList);
}

inline void GetHostInfo(const std::string& visibility, std::string& hostInfoJsonArray)
{
    char* rawHostInfoJson = NULL;
    throwIfNeed(GrdGetHostInfo(visibility.empty() ? NULL : visibility.c_str(), &rawHostInfoJson));
    hostInfoJsonArray = rawHostInfoJson;
    GrdFree(rawHostInfoJson);
}

inline void LedBlink(grd_uint32 dongleId)
{
    throwIfNeed(GrdLedBlink(dongleId));
}

inline void SetDriverType(VendorCodes* codes, grd_uint32 dongleId, grd_uint32 driverType)
{
    GrdVendorCodes c;
    GrdVendorCodes* cPtr = NULL;

    if (codes)
    {
        c = (GrdVendorCodes) * codes;
        cPtr = &c;
    }

    throwIfNeed(GrdSetDriverType(cPtr, dongleId, driverType));
}

class LicenseActivator
{
private:
    std::string m_host;
    grd_uint32 m_port;

public:
    LicenseActivator(const std::string& host = "", grd_uint32 port = 443)
        : m_host(host),
        m_port(port) {}

    void CheckSerialNumberFormat(const std::string& serialNumber)
    {
        throwIfNeed(GrdLicenseCheckSerialNumberFormat(serialNumber.c_str()));
    }
    void Activate(const std::string& serialNumber, GrdCustomerInfo* customerInfo, grd_uint32& outLicenseId)
    {
        throwIfNeed(GrdLicenseActivate(serialNumber.c_str(), m_host.c_str(), m_port, customerInfo, reinterpret_cast<grd_uint32*>(&outLicenseId)));
    }
    void Update(grd_uint32 licenseId)
    {
        throwIfNeed(GrdLicenseUpdate(licenseId, m_host.c_str(), m_port));
    }
    void CheckUpdateIsAvailable(grd_uint32 licenseId)
    {
        throwIfNeed(GrdLicenseCheckUpdateIsAvailable(licenseId, m_host.c_str(), m_port));
    }
    void RemoveLicense(grd_uint32 licenseId)
    {
        throwIfNeed(GrdLicenseRemove(licenseId));
    }
    void CheckIsNotBanned(grd_uint32 licenseId)
    {
        throwIfNeed(GrdLicenseCheckIsNotBanned(licenseId, m_host.c_str(), m_port));
    }
    void CreateActivationRequest(std::vector<grd_uint8>& request)
    {
        grd_uint8* outRequest;
        grd_uint32 outRequestSize;
        throwIfNeed(GrdLicenseCreateActivationRequest(reinterpret_cast<void**>(&outRequest), &outRequestSize));
        request.assign(outRequest, outRequest + outRequestSize);
        GrdFree(outRequest);
    }
    void CreateUpdateRequest(grd_uint32 licenseId, std::vector<grd_uint8>& request)
    {
        grd_uint8* outRequest;
        grd_uint32 outRequestSize;
        throwIfNeed(GrdLicenseCreateUpdateRequest(licenseId, reinterpret_cast<void**>(&outRequest), &outRequestSize));
        request.assign(outRequest, outRequest + outRequestSize);
        GrdFree(outRequest);
    }
    void SendActivationRequest(const std::string& serialNumber, GrdCustomerInfo* customerInfo, const std::vector<grd_uint8>& request, std::vector<grd_uint8>& response)
    {
        grd_uint8* outResponse;
        grd_uint32 outResponseSize;
        throwIfNeed(GrdLicenseSendActivationRequest(serialNumber.c_str(), customerInfo,
                                                    &request[0], static_cast<grd_uint32>(request.size()),
                                                    m_host.c_str(), m_port,
                                                    reinterpret_cast<void**>(&outResponse), &outResponseSize));
        response.assign(outResponse, outResponse + outResponseSize);
        GrdFree(outResponse);
    }
    void SendUpdateRequest(const std::vector<grd_uint8>& request, std::vector<grd_uint8>& response)
    {
        grd_uint8* outResponse;
        grd_uint32 outResponseSize;
        throwIfNeed(GrdLicenseSendUpdateRequest(&request[0], static_cast<grd_uint32>(request.size()), m_host.c_str(), m_port,
                                                reinterpret_cast<void**>(&outResponse), &outResponseSize));
        response.assign(outResponse, outResponse + outResponseSize);
        GrdFree(outResponse);
    }

    void ExtractForRehost(grd_uint32 licenseId, std::vector<grd_uint8>& outLicense)
    {
        grd_uint8* outDonorLicense;
        grd_uint32 outDonorLicenseSize;
        throwIfNeed(GrdLicenseExtractForRehost(licenseId, reinterpret_cast<void**>(&outDonorLicense), &outDonorLicenseSize));
        outLicense.assign(outDonorLicense, outDonorLicense + outDonorLicenseSize);
        GrdFree(outDonorLicense);
    }

    void Rehost(const std::vector<grd_uint8>& donorLicense)
    {
        throwIfNeed(GrdLicenseRehost(m_host.c_str(), m_port, donorLicense.data(), static_cast<grd_uint32>(donorLicense.size())));
    }

    void SendRehostRequest(const std::vector<grd_uint8>& donorLicense, std::string recipientHardwareFingerpint, std::vector<grd_uint8>& recipientLicense)
    {
        grd_uint8* recipientLicenseBuf;
        grd_uint32 recipientLicenseBufSize;
        throwIfNeed(GrdLicenseSendRehostRequest(m_host.c_str(), m_port,
                                                donorLicense.data(), static_cast<grd_uint32>(donorLicense.size()),
                                                recipientHardwareFingerpint.c_str(),
                                                reinterpret_cast<void**>(&recipientLicenseBuf), &recipientLicenseBufSize));
        recipientLicense.assign(recipientLicenseBuf, recipientLicenseBuf + recipientLicenseBufSize);
        GrdFree(recipientLicenseBuf);
    }

    void Detach(const std::string& visibility, const std::vector<GrdProductFeaturePair> pfPairs,
                grd_time_t durationInSec, grd_uint16 concurrentResourceToConsume, const std::string& recipientHardwareFingerpint, std::vector<grd_uint8>& recipientLicense)
    {
        grd_uint8* recipientLicenseBuf;
        grd_uint32 recipientLicenseBufSize;
        throwIfNeed(GrdLicenseDetach(visibility.c_str(), pfPairs.data(), static_cast<grd_uint32>(pfPairs.size()),
                                     durationInSec, concurrentResourceToConsume, recipientHardwareFingerpint.c_str(), reinterpret_cast<void**>(&recipientLicenseBuf), &recipientLicenseBufSize));
        recipientLicense.assign(recipientLicenseBuf, recipientLicenseBuf + recipientLicenseBufSize);
        GrdFree(recipientLicenseBuf);
    }

    void ReturnToPool(grd_uint32 licenseId, std::vector<grd_uint8>& licenseToReturn)
    {
        grd_uint8* licenseToReturnBuf;
        grd_uint32 licenseToReturnBufSize;
        throwIfNeed(GrdLicenseReturnToPool(licenseId, reinterpret_cast<void**>(&licenseToReturnBuf), &licenseToReturnBufSize));
        licenseToReturn.assign(licenseToReturnBuf, licenseToReturnBuf + licenseToReturnBufSize);
        GrdFree(licenseToReturnBuf);
    }

    void Install(const std::vector<grd_uint8>& response)
    {
        throwIfNeed(GrdLicenseInstall(&response[0], static_cast<grd_uint32>(response.size())));
    }

    void InstallRemotely(const std::string& visibility, const std::vector<grd_uint8>& response)
    {
        throwIfNeed(GrdLicenseInstallRemotely(visibility.c_str(), &response[0], static_cast<grd_uint32>(response.size())));
    }
};

} // guardant
