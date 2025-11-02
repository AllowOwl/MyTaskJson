#include "libaltlinux_comparison.h"
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <unordered_map>
#include <set>
#include <chrono>
#include <sstream>
#include <iostream>

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

    json PackageComparator::compare_branches(const std::string& branch_1, const std::string& branch_2, const std::string& arch){
        json final_result;

        json branch_1_data = load_branch_packages(branch_1);
        json branch_2_data = load_branch_packages(branch_2);

        std::set<std::string> architectures;
        for (const auto& pkg : branch_1_data["packages"]){
            architectures.insert(pkg["arch"]);}
        for (const auto& pkg : branch_2_data["packages"]){
            architectures.insert(pkg["arch"]);}
        
        std::set<std::string> target_archs;
        if (!arch.empty()){
            target_archs.insert(arch);}
        else{
            target_archs = architectures;}
        
        json by_arch = json::object();
        for (const auto& arch_name : target_archs){
            json branch_1_arch_pkg = json::array();
            json branch_2_arch_pkg = json::array();
            
            for (const auto& pkg : branch_1_data["packages"]){
                if (pkg["arch"] == arch_name){
                    branch_1_arch_pkg.push_back(pkg);
                }
            }
            for (const auto& pkg : branch_2_data["packages"]){
                if (pkg["arch"] == arch_name){
                    branch_2_arch_pkg.push_back(pkg);
                }
            }

            by_arch[arch_name] = compare_packages_by_arch(branch_1_arch_pkg, branch_2_arch_pkg);
        }

        final_result["comparison_by_architecture"] = by_arch;
        final_result["metadata"] = {
            {"branch_1", branch_1},
            {"branch_2", branch_2},
            {"compared_architectures", target_archs},
            {"comparison_date", std::time(nullptr)}
        };

        unsigned int size_branch_1_only = 0;
        unsigned int size_branch_2_only = 0;
        unsigned int size_branch_1_newer = 0;

        for (const auto& [arch, data] : by_arch.items()){
            size_branch_1_only += data["branch_1_only"].size();
            size_branch_2_only += data["branch_2_only"].size();
            size_branch_1_newer += data["branch_1_newer_versions"].size();
        }
        final_result["summary"] = {
            {"size_branch_1_only_packages", size_branch_1_only},
            {"size_branch_2_only_packages", size_branch_2_only},
            {"size_packages_newer_in_branch_1", size_branch_1_newer},
            {"size_branch_1_packages", branch_1_data["packages"].size()},
            {"size_branch_2_packages", branch_2_data["packages"].size()}
        };
        return final_result;
    }


    //private methods
    std::string PackageComparator::build_package_key(const json& pkg){
        return std::string(pkg["name"]) + ":" + std::string(pkg["arch"]);
    }

    bool PackageComparator::version_greater(const std::string& ver1, const std::string& ver2) {
        return ver1 > ver2;
    }

    json PackageComparator::compare_packages_by_arch(const json& branch_1_pk, const json& branch_2_pk){
        json result;

        std::unordered_map<std::string, json> branch_1_map;
        std::unordered_map<std::string, json> branch_2_map;

        for (const auto& pkg : branch_1_pk){
            std::string key = build_package_key(pkg);
            branch_1_map[key] = pkg;
        }
        for (const auto& pkg : branch_2_pk){
            std::string key = build_package_key(pkg);
            branch_2_map[key] = pkg;
        }


    json branch_1_only = json::array();
    for (const auto& [key, pkg] : branch_1_map) {
        if (branch_2_map.find(key) == branch_2_map.end()) {
            branch_1_only.push_back({
                {"name", pkg["name"]},
                {"arch", pkg["arch"]},
                {"version", pkg["version"]},
                {"release", pkg["release"]},
                {"epoch", std::to_string(pkg["epoch"].get<int>())},
                {"disttag", pkg.value("disttag", "")}
            });
        }
    }
    result["branch_1_only"] = branch_1_only;
    
    json branch_2_only = json::array();
    for (const auto& [key, pkg] : branch_2_map) {
        if (branch_1_map.find(key) == branch_1_map.end()) {
            branch_2_only.push_back({
                {"name", pkg["name"]},
                {"arch", pkg["arch"]},
                {"version", pkg["version"]},
                {"release", pkg["release"]},
                {"epoch", std::to_string(pkg["epoch"].get<int>())},
                {"disttag", pkg.value("disttag", "")}
            });
        }
    }
    result["branch_2_only"] = branch_2_only;
    

    json branch_1_newer = json::array();
    for (const auto& [key, pkg1] : branch_1_map) {
        auto it = branch_2_map.find(key);
        if (it != branch_2_map.end()) {
            const auto& pkg2 = it->second;
            
            std::string ver1 = std::string(pkg1["version"]) + "-" + std::string(pkg1["release"]);
            std::string ver2 = std::string(pkg2["version"]) + "-" + std::string(pkg2["release"]);
            
            if (version_greater(ver1, ver2)) {
                branch_1_newer.push_back({
                    {"name", pkg1["name"]},
                    {"arch", pkg1["arch"]},
                    {"branch_1_version", ver1},
                    {"branch_2_version", ver2}
                });
            }
        }
    }
    result["branch_1_newer_versions"] = branch_1_newer;

    return result;
    }
}