cmake_minimum_required(VERSION 3.14)

project(PicCompareTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/imageviewer.cpp
    src/imageviewer.h
    src/imageloader.cpp
    src/imageloader.h
    src/titlebar.cpp
    src/titlebar.h
    src/thememanager.cpp
    src/thememanager.h
    src/configmanager.cpp
    src/configmanager.h
    src/resources.qrc
)

add_executable(PicCompareTool ${PROJECT_SOURCES})

target_link_libraries(PicCompareTool PRIVATE
    Qt5::Widgets
    Qt5::Network
)

# Install directives
install(TARGETS PicCompareTool
    RUNTIME DESTINATION bin
)

# Windows-specific deployment with windeployqt
if(WIN32)
    # Find the Qt5 windeployqt executable
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        # Add a custom command to run windeployqt after build
        add_custom_command(TARGET PicCompareTool POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                    --verbose 0
                    --no-compiler-runtime
                    --no-opengl-sw
                    --no-system-d3d-compiler
                    "$<TARGET_FILE:PicCompareTool>"
            COMMENT "Running windeployqt..."
        )
        
        # For installation, run windeployqt and copy all dependencies
        install(CODE "
            message(STATUS \"Running windeployqt for installation...\")
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                    --no-compiler-runtime
                    --no-opengl-sw
                    --no-system-d3d-compiler
                    --dir \"\${CMAKE_INSTALL_PREFIX}/bin\"
                    \"$<TARGET_FILE:PicCompareTool>\"
            )
        ")
    else()
        message(WARNING "windeployqt not found, dependencies will not be deployed")
    endif()
endif()
