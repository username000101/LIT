cmake_minimum_required(VERSION 3.31)

set(LIT_VERSION "0.0.1")
set(LIT_VERSION_TYPE "Alpha")

project(
        LIT
        VERSION ${LIT_VERSION}
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
set(CMAKE_C_COMPILER "/usr/bin/clang")


find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Td REQUIRED)

if (NOT DEFINED LIT_INSTALL_PREFIX)
	message(WARNING "The LIT_INSTALL_PREFIX variable is not defined, default installition in /etc/.lit")
	set(LIT_INSTALL_PREFIX "/etc/.lit")
	message("LIT variables:
		LIT_DIR=${LIT_INSTALL_PREFIX}
		LIT_MODULES_DIR=${LIT_INSTALL_PREFIX}/modules
		LIT_FILES_DIR=${LIT_INSTALL_PREFIX}/files
		LIT_TRANSLATIONS_DIR=${LIT_INSTALL_PREFIX}/translations
		LIT_LOGS_DIR=${LIT_INSTALL_PREFIX}/logs
		LIT_CONFIG=${LIT_INSTALL_PREFIX}/config.json
		LIT_MODULES_CONFIG=${LIT_INSTALL_PREFIX}/modules.json
		LIT_VERSION=${LIT_VERSION}
                LIT_VERSION_TYPE=${LIT_VERSION_TYPE}")
else ()
	message(WARNING "The LIT_INSTALL_PREFIX has been overrided, now install prefix is ${LIT_INSTALL_PREFIX}")
	message("LIT variables:
		LIT_DIR=${LIT_INSTALL_PREFIX}
		LIT_MODULES_DIR=${LIT_INSTALL_PREFIX}/modules
		LIT_FILES_DIR=${LIT_INSTALL_PREFIX}/files
		LIT_TRANSLATIONS_DIR=${LIT_INSTALL_PREFIX}/translations
		LIT_CONFIG=${LIT_INSTALL_PREFIX}/config.json
		LIT_MODULES_CONFIG=${LIT_INSTALL_PREFIX}/modules.json
		LIT_VERSION=${LIT_VERSION}
                LIT_VERSION_TYPE=${LIT_VERSION_TYPE}")
endif()

if (NOT DEFINED API_ID OR NOT DEFINED API_HASH)
    message(FATAL_ERROR "You need to specify the API_ID and API_HASH variables")
endif()

include_directories(
	include
)

file(GLOB LIT "main.cxx")
file(GLOB Args "src/Args/*.cxx" "include/Args/*.hxx")
file(GLOB Auth "src/Auth/*.cxx" "include/Auth/*.hxx")
file(GLOB Configuration "src/Configuration/*.cxx" "include/Configuration/*.hxx")
file(GLOB Filesystem "src/Filesystem/*.cxx" "include/Filesystem/*.hxx")
file(GLOB ModulesInteraction "src/ModulesInteraction/*.cxx" "include/ModulesInteraction/*.hxx")
file(GLOB RuntimeStorage "include/RuntimeStorage/*.hxx")
file(GLOB TelegramAPIInteraction "src/TelegramAPIInteraction/*.cxx" "include/TelegramAPIInteraction/*.hxx")
file(GLOB Utils "src/Utils/*.cxx" "include/Utils/*.hxx")
file(GLOB XLML "src/XLML/*.cxx" "include/XLML/*.hxx")

add_executable(LIT
	${LIT}
	${Args}
	${Auth}
	${Configuration}
	${Filesystem}
	${ModulesInteraction}
        ${RuntimeStorage}
	${TelegramAPIInteraction}
        ${Utils}
	${XLML}
)

target_compile_definitions(LIT PUBLIC
   LIT_VERSION="${LIT_VERSION}"
   LIT_VERSION_TYPE="${LIT_VERSION_TYPE}"
)

target_compile_definitions(LIT PRIVATE
    LIT_API_ID="${API_ID}"
    LIT_API_HASH="${API_HASH}"
    LIT_DIR="${LIT_INSTALL_PREFIX}"
    LIT_CONFIG="${LIT_INSTALL_PREFIX}/config.json"
    LIT_MODULES_CONFIG="${LIT_INSTALL_PREFIX}/modules.json"
    LIT_FILES_DIR="${LIT_INSTALL_PREFIX}/files"
    LIT_MODULES_DIR="${LIT_INSTALL_PREFIX}/modules"
    LIT_TRANSLATIONS_DIR="${LIT_INSTALL_PREFIX}/translations"
    LIT_LOGS_DIR="${LIT_INSTALL_PREFIX}/logs"
    LIT_TDLIB_DATABASE_DIR="${LIT_INSTALL_PREFIX}/tdlib/db"
    LIT_TDLIB_FILES_DIR="${LIT_INSTALL_PREFIX}/tdlib/files"
    LIT_TDLIB_TIMEOUT=15.0
    LIT_TDLIB_ATTEMPTS=150
)

target_link_libraries(LIT PUBLIC
	spdlog::spdlog
	nlohmann_json::nlohmann_json
	Td::TdStatic
        libarchive.so
)
