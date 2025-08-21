project(moonlight-sfos CXX)
cmake_minimum_required(VERSION 3.5)

find_package (Qt5 COMPONENTS Core Network Qml Gui Quick REQUIRED)

include(FindPkgConfig)
pkg_search_module(SAILFISH sailfishapp REQUIRED)
pkg_search_module(OPENSSL openssl REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(src/moonlight-common-c)

add_executable(moonlight-sfos
    src/moonlight-sfos.cpp
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
    Qt5::Quick
    Threads::Threads
    ${SAILFISH_LDFLAGS}
    ${OPENSSL_LDFLAGS}
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
        qml/pages/About.qml
        qml/pages/Settings.qml
        qml/dialogs/PairDialog.qml
        rpm/moonlight-sfos.spec
        ${TsFiles})

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/moonlight-sfos:bin
")

