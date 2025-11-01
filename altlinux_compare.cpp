// #include <iostream>
// #include <string>
// #include <fstream>
// #include <filesystem>
// #include <nlohmann/json.hpp>

// using json = nlohmann::json;

// std::string GetExecutableDir() {
//     namespace fs = std::filesystem;
//     try {
//         //получает путь расположения программы
//         fs::path exe_path = fs::canonical("/proc/self/exe");
//         return exe_path.parent_path().string();
//     } catch (const std::filesystem::filesystem_error& e) {
//         std::cerr << "Ошибка получения пути: " << e.what() << '\n';
//         return {};
//     }
// }
// //удаление последнего адреса директории (с счетчиком)
// std::string Delete_back_path(std::string path, int n) {
//     for (int i = 0; i < n; ++i) {
//         auto pos = path.find_last_of('/');
//         if (pos == std::string::npos) return "";
//         path.erase(pos);
//     }
//     return path;
// }
// //считывание файла в класс nlohmann::json
// json read_json_file(const std::string& filename) {
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         throw std::runtime_error("Failed to open file: " + filename);
//     }
//     return json::parse(file);
// }

// //вывод arch
// void process_request_args(const json& data) {
//     const auto& args = data["request_args"];
//     std::string arch = args["arch"].is_null() ? "null" : args["arch"];
//     std::cout << "Request Architecture: " << arch << "\n";
// }

// //вывод всего json файла
// void print_packages(const json& data) {
//     std::cout << "\nPackages (" << data["packages"].size() << "):\n";
//     for (const auto& pkg : data["packages"]) {
//         std::cout << "Name: " << pkg["name"] << "\n"
//                   << "Version: " << pkg["version"] << "\n"
//                   << "Arch: " << pkg["arch"] << "\n"
//                   << "Build time: " << pkg["buildtime"] << "\n\n";
//     }
// }

// // полный вывод
// void process_json_data(const json& data) {
//     std::cout << "Total records: " << data["length"] << "\n";
//     process_request_args(data);
//     print_packages(data);
// }
// //сравнение данных
// json compare_packages(const std::unordered_map<std::string, json>& source_packages,
//                          const std::unordered_map<std::string, json>& target_packages) {
//     json result = json::array(); //пустой JSON-массив
    
//     for (const auto& [key, pkg] : source_packages) {
//         if (target_packages.find(key) == target_packages.end()) {
//             result.push_back({
//                 {"name", pkg["name"]},
//                 {"arch", pkg["arch"]},
//                 {"version", pkg["version"]},
//                 {"release", pkg["release"]}
//             });
//         }
//     }
    
//     return result;
// }

// json create_new_list(const json& p10_data, const json& sisyphus_data) {
//     json result;

//     //хеш-таблицы
//     std::unordered_map<std::string, json> p10_packages;
//     std::unordered_map<std::string, json> sisyphus_packages;

//     //заполнение
//     for (const auto& pkg : p10_data["packages"]) {
//         std::string key = std::string(pkg["name"]) + ":" + std::string(pkg["arch"]);
//         p10_packages[key] = pkg;
//     }

//     for (const auto& pkg : sisyphus_data["packages"]) {
//         std::string key = std::string(pkg["name"]) + ":" + std::string(pkg["arch"]);
//         sisyphus_packages[key] = pkg;
//     }

//     //все пакеты, которые есть в p10 но нет в sisyphus
//     json p10_only = compare_packages(p10_packages, sisyphus_packages);;
//     result["p10_only"] = p10_only;
//     //все пакеты, которые есть в sisyphus но их нет в p10
//     json sisyphus_only = compare_packages(sisyphus_packages, p10_packages);;
//     result["sisyphus_only"] = sisyphus_only;

//     //все пакеты, version-release которых больше в sisyphus чем в p10
//     json up_ver_relise_in_sisyphus;
//     for (const auto& [key, p10_pkg] : p10_packages) {
//         if (sisyphus_packages.find(key) != sisyphus_packages.end()) {
//             const auto& sisyphus_pkg = sisyphus_packages[key];
            
//             //сравниваем version-release
//             std::string p10_ver = std::string(p10_pkg["version"]) + "-" + std::string(p10_pkg["release"]);
//             std::string sisyphus_ver = std::string(sisyphus_pkg["version"]) + "-" + std::string(sisyphus_pkg["release"]);
            
//             if (sisyphus_ver > p10_ver) {
//                 up_ver_relise_in_sisyphus.push_back({
//                     {"name", p10_pkg["name"]},
//                     {"arch", p10_pkg["arch"]},
//                     {"p10_version", p10_ver},
//                     {"sisyphus_version", sisyphus_ver}
//                 });
//             }
//         }
//     }
//     result["up_ver_relise_in_sisyphus"] = up_ver_relise_in_sisyphus;

//     //общие сведенья
//     result["--stats--"] = {
//         {"p10_only_count", p10_only.size()},
//         {"sisyphus_only_count", sisyphus_only.size()},
//         {"up_ver_relise_in_sisyphus_count", up_ver_relise_in_sisyphus.size()},
//         {"total_p10_packages", p10_data["packages"].size()},
//         {"total_sisyphus_packages", sisyphus_data["packages"].size()}
//     };

//     return result;
// }

// int main(int, char**){
//     std::string path = Delete_back_path(GetExecutableDir(), 0);
//     std::cout << path;
//     system("curl -o sisyphus_packages.json https://rdb.altlinux.org/api/export/branch_binary_packages/sisyphus");
//     system("curl -o p10_packages.json https://rdb.altlinux.org/api/export/branch_binary_packages/p10");
//     system(("mv " "'" + path + "/sisyphus_packages.json' " + path + "/data/Sisyphus/").c_str());
//     system(("mv " "'" + path + "/p10_packages.json' " + path + "/data/p10/").c_str());
    
//     json file_p10 = read_json_file(path + "/data/p10/p10_packages.json");
//     json file_sisyphus = read_json_file(path + "/data/Sisyphus/sisyphus_packages.json");
//     //process_json_data(file_p10);//вывод всего json

//     json comparison_result = create_new_list(file_p10, file_sisyphus);
    
//     //сохранение результата
//     std::ofstream out_file(path + "/data/result/comparison_result.json");
//     out_file << comparison_result.dump(4);
// }
#include "libaltlinux_comparison.h"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

int main() {
    std::cout << "Простая проверка библиотеки...\n";
    
    try {
        altlinux::PackageComparator comp;
        
        // Просто загружаем пакеты одной ветки
        std::cout << "Загружаем пакеты из ветки 'p10'...\n";
        auto packages = comp.load_branch_packages("p10", "x86_64");
        
        std::cout << "УСПЕХ! Загружено пакетов: " << packages["packages"].size() << "\n";
        
        //первый пакет
        if (packages["packages"].size() > 0) {
            auto first_pkg = packages["packages"][0];
            std::cout << "Пример пакета: " << first_pkg["name"] << " " 
                      << first_pkg["version"] << " (" << first_pkg["arch"] << ")\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ОШИБКА: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}