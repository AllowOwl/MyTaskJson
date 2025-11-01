#include "libaltlinux_comparison.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>

using json = nlohmann::json;

namespace altlinux {

    // Callback функция для записи данных от CURL
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userresponse)
    {
    ((std::string*)userresponse)->append((char*)contents, size * nmemb);
    return size * nmemb;
    }


json PackageComparator::load_branch_packages(const std::string& branch, const std::string& arch) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Failed to initialize CURL");

    std::string url = api_base_url_ + "/export/branch_binary_packages/" + branch;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "AltLinux-Comparator");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));

    json data = json::parse(response);

    if (!arch.empty()) {
        json filtered_packages = json::array();
        for (const auto& pkg : data["packages"]) {
            if (pkg["arch"] == arch) {
                filtered_packages.push_back(pkg);
            }
        }
        data["packages"] = filtered_packages;
        data["length"] = filtered_packages.size();
    }
    
    return data;
}
}