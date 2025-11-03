#include "libaltlinux_comparison.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using json = nlohmann::json;

void print_usage() {
    std::cout << "ALT Linux Package Comparator - CLI Utility\n\n";
    std::cout << "Usage:\n";
    std::cout << "  altlinux-compare <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  compare <branch1> <branch2> [arch]    Compare two branches\n";
    std::cout << "  list-branches                         Show available branches\n";
    std::cout << "  info <branch> [arch]                  Show branch package info\n";
    std::cout << "  help                                  Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  altlinux-compare compare p10 p9 x86_64\n";
    std::cout << "  altlinux-compare compare p10 sisyphus\n";
    std::cout << "  altlinux-compare info p10\n";
    std::cout << "  altlinux-compare list-branches\n";
    std::cout << "!!! Before comparing branches by architecture,\nensure that the required packages are present | info <branch> [arch]!!!\n";
}

void print_branches() {
    std::cout << "Available ALT Linux branches:\n";
    std::cout << "sisyphus\n";
    std::cout << "sisyphus_e2k\n";
    std::cout << "sisyphus_riscv64\n";
    std::cout << "sisyphus_loongarch64\n";
    std::cout << "p11\n";
    std::cout << "p10\n";
    std::cout << "p10_e2k\n";
    std::cout << "p9\n";
    std::cout << "c10f2\n";
    std::cout << "c9f2\n";
    
    std::cout << "Common architectures:\n";
    std::cout << "srpm  noarch  x86_64  i586  aarch64  x86_64-i586\n";
}

void print_package_info(const std::string& branch, const std::string& arch = "") {
    try {
        altlinux::PackageComparator comp;
        std::cout << "Loading packages for branch '" << branch << "'";
        if (!arch.empty()) {
            std::cout << " (arch: " << arch << ")";
        }
        std::cout << "...\n";
        
        auto packages = comp.load_branch_packages(branch, arch);
        std::cout << "Total packages: " << packages["packages"].size() << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void compare_branches(const std::string& branch1, const std::string& branch2, const std::string& arch = "") {
    try {
        altlinux::PackageComparator comp;
        std::filesystem::create_directories("data");

        
        std::cout << "Comparing " << branch1 << " vs " << branch2;
        if (!arch.empty()) {
            std::cout << " (arch: " << arch << ")";
        }
        std::cout << "\n";
        
        std::cout << "Loading packages...\n";
        auto result = comp.compare_branches(branch1, branch2, arch);
        
        std::cout << "Comparison completed!\n\n";
        

        auto& summary = result["summary"];
        
        std::cout << "summary of comparison:\n";
        std::cout << "  - pkg only in " << branch1 << ": " << summary["size_branch_1_only_packages"] << "\n";
        std::cout << "  - pkg only in " << branch2 << ": " << summary["size_branch_2_only_packages"] << "\n";
        std::cout << "  - pkg newer in " << branch1 << ": " << summary["size_packages_newer_in_branch_1"] << "\n";
        std::cout << "  - all pkg in " << branch1 << ": " << summary["size_branch_1_packages"] << "\n";
        std::cout << "  - all pkg in " << branch2 << ": " << summary["size_branch_2_packages"] << "\n\n";
        

        std::string filename = "data/comparison_" + branch1 + "_vs_" + branch2;
        if (!arch.empty()) {
            filename += "_" + arch;
        }
        filename += ".json";

        json output;
        output["metadata"] = result["metadata"];
        output["summary"] = result["summary"];
        
        
        json packages_only_branch1 = json::array();
        json packages_only_branch2 = json::array();
        json packages_newer_branch1 = json::array();
        
        auto& by_arch = result["comparison_by_architecture"];
        for (const auto& [arch_name, arch_data] : by_arch.items()) {
            
            for (const auto& pkg : arch_data["branch_1_only"]) {
                packages_only_branch1.push_back(pkg);
            }
            
            
            for (const auto& pkg : arch_data["branch_2_only"]) {
                packages_only_branch2.push_back(pkg);
            }
            
            
            for (const auto& pkg : arch_data["branch_1_newer_versions"]) {
                packages_newer_branch1.push_back(pkg);
            }
        }
        
        output["packages_only_in_" + branch1] = packages_only_branch1;
        output["packages_only_in_" + branch2] = packages_only_branch2;
        output["packages_newer_in_" + branch1] = packages_newer_branch1;
        
        std::ofstream out_file(filename);
        if (out_file.is_open()) {
            out_file << output.dump(4);
            std::cout << "Full results saved to: " << filename << std::endl;
            
            std::cout << "Examples in file:\n";
            if (output["packages_only_in_" + branch1].size() > 0) {
                std::cout << "  - " << output["packages_only_in_" + branch1].size() 
                          << " packages only in " << branch1 << "\n";
            }
            if (output["packages_only_in_" + branch2].size() > 0) {
                std::cout << "  - " << output["packages_only_in_" + branch2].size() 
                          << " packages only in " << branch2 << "\n";
            }
            if (output["packages_newer_in_" + branch1].size() > 0) {
                std::cout << "  - " << output["packages_newer_in_" + branch1].size() 
                          << " packages newer in " << branch1 << "\n";
            }
        } else {
            std::cout << "Error saving file\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "compare" && argc >= 4) {
        std::string branch1 = argv[2];
        std::string branch2 = argv[3];
        std::string arch = (argc >= 5) ? argv[4] : "";
        compare_branches(branch1, branch2, arch);
    }
    else if (command == "info" && argc >= 3) {
        std::string branch = argv[2];
        std::string arch = (argc >= 4) ? argv[3] : "";
        print_package_info(branch, arch);
    }
    else if (command == "list-branches") {
        print_branches();
    }
    else if (command == "help" || command == "--help" || command == "-h") {
        print_usage();
    }
    else {
        std::cerr << "Unknown command or invalid arguments\n\n";
        print_usage();
        return 1;
    }
    
    return 0;
}