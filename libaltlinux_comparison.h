#ifndef LIBALTLINUX_COMPARISON_H
#define LIBALTLINUX_COMPARISON_H

#include <string>
#include <nlohmann/json.hpp>

namespace altlinux {
    class PackageComparator {
        public:
            nlohmann::json load_branch_packages(
                const std::string& branch,
                const std::string& arch = ""
            );

            nlohmann::json compare_branches(
                const std::string& branch_1,
                const std::string& branch_2,
                const std::string& arch = "");
                
            

        private:
            std::string api_base_url_ = "https://rdb.altlinux.org/api";
            std::string build_package_key(const nlohmann::json& pkg);

            nlohmann::json compare_packages_by_arch(
                const nlohmann::json& branch_1_pk,
                const nlohmann::json& branch_2_pk
            );
            
            std::string get_current_date_string();
            bool version_greater(const std::string& ver1, const std::string& ver2);
    };
}

#endif