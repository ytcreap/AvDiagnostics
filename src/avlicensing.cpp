#include "avlicensing.h"

namespace {
    constexpr grd_uint32 VENDOR_PUBLIC_CODE  = 0x41D5C723;
    constexpr grd_uint32 VENDOR_PRIVATE_READ  = 0x3BBA7843;
    constexpr grd_uint32 VENDOR_PRIVATE_WRITE = 0x934687C4;

    const GrdVendorCodes vendorCodes = {
        VENDOR_PUBLIC_CODE,
        VENDOR_PRIVATE_READ,
        VENDOR_PRIVATE_WRITE
    };
}

std::vector<std::string> ExtractAllProductNames(const char* json)
{
    std::vector<std::string> productNames;

    try {
        nlohmann::json root = nlohmann::json::parse(json);
        //std::cout << root.dump(4);
        // Проверка наличия массива licenses
        if (root.find("licenses") != root.end() && root["licenses"].is_array()) {
            for (auto& license : root["licenses"]) {
                // Проверяем licenseInfo
                if (license.find("licenseInfo") != license.end() &&
                    license["licenseInfo"].is_object())
                {
                    nlohmann::json licenseInfo = license["licenseInfo"];

                    // Проверяем массив products внутри licenseInfo
                    if (licenseInfo.find("products") != licenseInfo.end() &&
                        licenseInfo["products"].is_array())
                    {
                        for (auto& product : licenseInfo["products"]) {
                            if (product.find("name") != product.end() &&
                                product["name"].is_string())
                            {
                                productNames.push_back(product["name"].get<std::string>());
                            }
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }

    return productNames;
}

AVLicensing::AVLicensing()
{

}

void AVLicensing::ShowProducts()
{
    GrdHandle featureHandle = GRD_INVALID_HANDLE_VALUE;
    const grd_uint32 FEATURE_ID = 5;
    int status = GRD_OK;

    // 1. Получение информации о лицензиях
    char* licenseInfoJson = nullptr;
    status = GrdGetLicenseInfo(nullptr, &vendorCodes, &licenseInfoJson);

    auto productNames = ExtractAllProductNames(licenseInfoJson);
    // Исправленный блок вывода
    if (!productNames.empty()) {
        std::cout << "Записанные лицензии:\n";
        for (const auto& name : productNames) {
            std::cout << name << "\n";
        }
    } else {
        std::cout << "No licensed products found\n";
    }
}

std::string AVLicensing::GetLicenseInfoJson() {
    char* licenseInfoJson = nullptr;
    int status = GrdGetLicenseInfo(nullptr, &vendorCodes, &licenseInfoJson);
    if (status != GRD_OK || licenseInfoJson == nullptr) {
        if (licenseInfoJson) GrdFree(licenseInfoJson);
        return {};
    }
    std::string jsonStr(licenseInfoJson);
    GrdFree(licenseInfoJson);
    return jsonStr;
}

int AVLicensing::Checker(grd_uint32 FEATURE_ID)
{
    GrdHandle featureHandle = GRD_INVALID_HANDLE_VALUE;
    int status = GRD_OK;

    // 1. Получение информации о лицензиях
    char* licenseInfoJson = nullptr;
    status = GrdGetLicenseInfo(nullptr, &vendorCodes, &licenseInfoJson);
    if (status != GRD_OK) {
    //    std::cerr << "Ошибка получения информации о лицензиях: " << status << std::endl;
        GrdFree(licenseInfoJson);
        return 1;
    }
    //std::cout << "Информация о лицензиях:\n" << licenseInfoJson << std::endl;


    GrdFree(licenseInfoJson);

    // 2. Вход в фичу
    status = GrdFeatureLogin(FEATURE_ID, &vendorCodes, nullptr, &featureHandle);
    if (status != GRD_OK) {
    //    std::cerr << "Ошибка входа в фичу: " << status << std::endl;
        return 1;
    }


    // 3. Проверка времени действия
    grd_time_t remainingTime = 0;
    status = GrdFeatureGetTimeLimit(featureHandle, &remainingTime);
    if (status == GRD_OK && remainingTime > 0) {
        time_t currentTime = time(nullptr);
        if (currentTime > remainingTime) {
    //        std::cerr << "Лицензия просрочена" << std::endl;
            GrdFeatureLogout(featureHandle);
            return 2;
        }
    }

    // 4. Проверка счетчика запусков
    grd_uint32 runCounter = 0;
    status = GrdFeatureGetRunCounter(featureHandle, &runCounter);
    if (status == GRD_OK && runCounter == 0) {
        // std::cerr << "Лимит использований исчерпан" << std::endl;
        GrdFeatureLogout(featureHandle);
        return 3;
    }



    // 5. Выход из фичи и очистка
    GrdFeatureLogout(featureHandle);
    // std::cout << "Проверка лицензии успешна" << std::endl;
    return 0;
}
