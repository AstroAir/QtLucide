# QtLucide library source files
set(QTLUCIDE_SOURCES
    QtLucide.cpp
    QtLucideIconEngine.cpp
    QtLucideIconPainter.cpp
)

set(QTLUCIDE_HEADERS
    ../include/QtLucide/QtLucide.h
    ../include/QtLucide/QtLucideIconEngine.h
    ../include/QtLucide/QtLucideIconPainter.h
    ../include/QtLucide/QtLucideEnums.h
    ../include/QtLucide/QtLucideStrings.h
)

# Create the QtLucide library
add_library(QtLucide ${QTLUCIDE_SOURCES} ${QTLUCIDE_HEADERS})

# Set target properties
set_target_properties(QtLucide PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME QtLucide
)

# Link Qt6 libraries
target_link_libraries(QtLucide
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
)

# Set include directories
target_include_directories(QtLucide
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Auto-generate resources before building
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../resources/icons")
set(SVG_DIR "${RESOURCES_DIR}/svg")
set(QRC_FILE "${RESOURCES_DIR}/lucide_icons.qrc")
set(METADATA_DIR "${RESOURCES_DIR}/metadata")
set(BUILD_RESOURCES_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../tools/build_resources.py")

# Find all SVG files to use as dependencies
file(GLOB SVG_FILES "${SVG_DIR}/*.svg")

# Custom command to generate resources when SVG files change
add_custom_command(
    OUTPUT ${QRC_FILE}
           ${METADATA_DIR}/icons.json
           ${CMAKE_CURRENT_SOURCE_DIR}/../include/QtLucide/QtLucideEnums.h
           ${CMAKE_CURRENT_SOURCE_DIR}/../include/QtLucide/QtLucideStrings.h
    COMMAND ${Python3_EXECUTABLE} ${BUILD_RESOURCES_SCRIPT} ${CMAKE_CURRENT_SOURCE_DIR}/..
    DEPENDS ${SVG_FILES} ${BUILD_RESOURCES_SCRIPT}
    COMMENT "Generating QtLucide resources from SVG files"
    VERBATIM
)

# Create a custom target for resource generation
add_custom_target(generate_qtlucide_resources
    DEPENDS ${QRC_FILE}
            ${METADATA_DIR}/icons.json
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/QtLucide/QtLucideEnums.h
            ${CMAKE_CURRENT_SOURCE_DIR}/../include/QtLucide/QtLucideStrings.h
)

# Add resources - use the auto-generated QRC file with all Lucide icons
qt6_add_resources(QtLucide "qtlucide_icons"
    BASE "../resources/icons"
    FILES ${QRC_FILE}
)

# Make sure resources are generated before building the library
add_dependencies(QtLucide generate_qtlucide_resources)
