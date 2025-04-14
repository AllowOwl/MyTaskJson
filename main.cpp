#include <iostream>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

std::string GetExecutableDir() {
    namespace fs = std::filesystem;
    try {
        // Получаем канонический путь к исполняемому файлу
        fs::path exe_path = fs::canonical("/proc/self/exe");
        // Возвращаем родительскую директорию как строку
        return exe_path.parent_path().string();
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка получения пути: " << e.what() << '\n';
        return {}; // Возвращаем пустую строку при ошибке
    }
}
std::string Delete_back_path(std::string path, int n) {
    for (int i = 0; i < n; ++i) {
        auto pos = path.find_last_of('/');
        if (pos == std::string::npos) return "";
        path.erase(pos);
    }
    return path;
}
int main(int, char**){
    std::string path = Delete_back_path(GetExecutableDir(), 1);
    system("curl -o sisyphus_packages.json https://rdb.altlinux.org/api/export/branch_binary_packages/sisyphus");
    system("curl -o p10_packages.json https://rdb.altlinux.org/api/export/branch_binary_packages/p10");
    system(("mv " + path + "/build/sisyphus_packages.json " + path + "/data/Sisyphus/").c_str());
    system(("mv " + path + "/build/p10_packages.json " + path + "/data/p10/").c_str());
}