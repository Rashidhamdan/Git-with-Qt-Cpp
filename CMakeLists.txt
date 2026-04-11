cmake_minimum_required(VERSION 3.16)

project(GIT_GUI VERSION 0.1 LANGUAGES CXX)

# Automatische Qt-Verarbeitung
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt-Abhängigkeiten finden
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui Test LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui Test LinguistTools)


# Übersetzungsdateien
set(TS_FILES GIT_GUI_de_DE.ts)

# Hauptquellen (z. B. MainWindow)
set(PROJECT_SOURCES
    main.cpp
    ${TS_FILES}
)

# Unterordner einbinden
add_subdirectory(src)

# Hauptziel definieren
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GIT_GUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc

    )
    qt_finalize_executable(GIT_GUI)
else()
    add_executable(GIT_GUI ${PROJECT_SOURCES})
endif()

# Qt-Bibliotheken + Submodule linken
target_link_libraries(GIT_GUI
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    core
    gui
    commands
    cli
    utils
)

# Übersetzung
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS GIT_GUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
