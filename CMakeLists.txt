cmake_minimum_required(VERSION 3.11.0)
project(task_json VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_json main.cpp)

include(FetchContent)

# Загрузка nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2  # Укажите нужную версию (например, v3.11.2)
)

FetchContent_MakeAvailable(nlohmann_json)

# Создание исполняемого файла
add_executable(your_target main.cpp)
target_link_libraries(your_target PRIVATE nlohmann_json::nlohmann_json)