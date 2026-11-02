cmake_minimum_required(VERSION 3.5)
project(moonlight-sfos C CXX)

find_package (Qt5 COMPONENTS Core Network Qml Gui Quick REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)
pkg_search_module(OPENSSL openssl REQUIRED)
pkg_search_module(CURL libcurl REQUIRED)
pkg_search_module(EXPAT expat REQUIRED)
pkg_search_module(UUID uuid REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src/moonlight-common-c)

add_library(gamestream STATIC
    src/moonlight-embedded/libgamestream/client.c
    src/moonlight-embedded/libgamestream/http.c
    src/moonlight-embedded/libgamestream/mkcert.c
    src/moonlight-embedded/libgamestream/xml.c
)
target_include_directories(gamestream PUBLIC
    src/moonlight-embedded/libgamestream
    src/moonlight-common-c/src
)
target_include_directories(gamestream PRIVATE
    ${OPENSSL_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIRS}
    ${UUID_INCLUDE_DIRS}
)
target_link_libraries(gamestream PUBLIC
    ${CURL_LDFLAGS}
    ${EXPAT_LDFLAGS}
    ${UUID_LDFLAGS}
    ${OPENSSL_LDFLAGS}
)

add_executable(moonlight-sfos
    src/moonlight-sfos.cpp
    src/pairingclient.cpp
    src/pairingclient.h
    $<TARGET_OBJECTS:enet>
    $<TARGET_OBJECTS:moonlight-common-c>
    )
target_compile_definitions(moonlight-sfos PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)
target_include_directories(moonlight-sfos PRIVATE
    $<BUILD_INTERFACE:
    ${SAILFISH_INCLUDE_DIRS}
>)
target_link_libraries(moonlight-sfos
    gamestream
    Qt5::Quick
    Threads::Threads
    ${SAILFISH_LDFLAGS}
)

install(TARGETS moonlight-sfos
    RUNTIME DESTINATION bin
)
install(DIRECTORY qml
    DESTINATION share/moonlight-sfos
)
install(DIRECTORY translations
    DESTINATION share/moonlight-sfos
    FILES_MATCHING PATTERN "*.qm"
)
install(FILES moonlight-sfos.desktop
    DESTINATION share/applications
)
install(FILES icons/86x86/moonlight-sfos.png
    DESTINATION share/icons/hicolor/86x86/apps
)
install(FILES icons/108x108/moonlight-sfos.png
    DESTINATION share/icons/hicolor/108x108/apps
)
install(FILES icons/128x128/moonlight-sfos.png
    DESTINATION share/icons/hicolor/128x128/apps
)
install(FILES icons/172x172/moonlight-sfos.png
    DESTINATION share/icons/hicolor/172x172/apps
)

FILE(GLOB TsFiles "translations/*.ts")
add_custom_target(distfiles
    SOURCES
        moonlight-sfos.desktop
        qml/moonlight-sfos.qml
        qml/cover/CoverPage.qml
        qml/pages/Main.qml
        qml/pages/Apps.qml
        qml/pages/About.qml
        qml/pages/Settings.qml
        qml/dialogs/PairDialog.qml
        rpm/moonlight-sfos.spec
        ${TsFiles})

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/moonlight-sfos:bin
")

