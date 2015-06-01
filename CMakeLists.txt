cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0020 OLD)

project(software_quality_model)
set(EXECUTABLE_NAME software_quality_model)

message(STATUS ${CMAKE_PREFIX_PATH})

set(QT_USE_QTMAIN TRUE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

include_directories(
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/Netica_API_504/src
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Netica_API_504/lib
)

#http://cmake.3232098.n2.nabble.com/AUTOMOC-feature-skips-sources-of-executable-targets-td7584290.html
# headers
file(
    GLOB 
    SQM_headers 
    src/*.h*
)

# sources
file(
    GLOB 
    SQM_sources
    src/*.c*
)

# forms
file(
    GLOB 
    SQM_forms 
    forms/*.ui
)

# resources
SET(SQM_resources resources/images/resources.qrc)
SET(SQM_icon resources/images/app_icon.rc)

QT5_WRAP_UI(SQM_uis ${SQM_forms})

qt5_add_resources(SQM_resources_headers ${SQM_resources})

# group in filters
source_group("Forms" FILES ${SQM_forms})
source_group("Resources" FILES ${SQM_resources} ${SQM_icon})

add_executable(
    ${EXECUTABLE_NAME}
    WIN32
    ${SQM_sources} ${SQM_uis} ${SQM_resources_headers} ${SQM_headers} ${SQM_icon}
)

target_link_libraries(
	${EXECUTABLE_NAME}
	netica
)

# Create dependencies

qt5_use_modules(${EXECUTABLE_NAME} Widgets)

install(TARGETS ${EXECUTABLE_NAME}
    RUNTIME
    DESTINATION bin
    COMPONENT Applications
)

file(
    GLOB
    qt5_runtime_root
    ${CMAKE_PREFIX_PATH}/bin/Qt5Core.dll
    ${CMAKE_PREFIX_PATH}/bin/Qt5Gui.dll
    ${CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll
    ${CMAKE_PREFIX_PATH}/bin/libEGL.dll
    ${CMAKE_PREFIX_PATH}/bin/libGLESv2.dll
)

install(FILES ${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll
    DESTINATION bin/platforms
    COMPONENT Applications
)

install(FILES ${qt5_runtime_root}
    DESTINATION bin
    COMPONENT Applications
)
