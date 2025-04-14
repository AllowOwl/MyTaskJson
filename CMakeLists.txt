cmake_minimum_required(VERSION 3.10.0)
project(mytaskjson VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Загрузка nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(nlohmann_json)

# Создание исполняемого файла (ОДИН РАЗ!)
add_executable(mytaskjson main.cpp)
target_link_libraries(mytaskjson PRIVATE nlohmann_json::nlohmann_json)