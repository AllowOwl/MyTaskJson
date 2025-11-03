cmake_minimum_required(VERSION 3.10)
project(AltLinuxComparator VERSION 1.0.0)

# Настройка компилятора C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем позиционно-независимый код
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Поиск зависимостей
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)

# Основная библиотека shared library
add_library(altlinux_comparison SHARED
    libaltlinux_comparison.cpp
)

# Главная CLI утилита
add_executable(altlinux-compare
    altlinux_compare.cpp
)

# Связывание библиотек
target_link_libraries(altlinux-compare
    altlinux_comparison
    ${CURL_LIBRARIES}
)

# Флаги компилятора
target_compile_options(altlinux-compare PRIVATE -Wall -Wextra)

#Устанавливаем версию библиотеки
set_target_properties(altlinux_comparison PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)