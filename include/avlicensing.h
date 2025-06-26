#ifndef AVLICENSING_H
#define AVLICENSING_H
#include <iostream>
#include <vector>
#include <ctime>
#include <cstring>
#include <nlohmann/json.hpp>
#include "grdlic.h"

class AVLicensing
{
public:
    AVLicensing();
    void ShowProducts();
    int Checker(grd_uint32 FEATURE_ID);
    std::string GetLicenseInfoJson();
};

#endif // AVLICENSING_H
