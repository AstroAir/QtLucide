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

# Add resources
qt6_add_resources(QtLucide "qtlucide_icons"
    FILES
        ../resources/icons/lucide_icons.qrc
)

# Make sure resource target is part of the export
set_target_properties(QtLucide_resources_1 PROPERTIES
    EXPORT_NAME QtLucide_resources_1
)
