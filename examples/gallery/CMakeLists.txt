# QtLucide Gallery Application

set(GALLERY_SOURCES
    main.cpp
    GalleryMainWindow.cpp
    GalleryMainWindow.h
    IconGridWidget.cpp
    IconGridWidget.h
    IconDetailsPanel.cpp
    IconDetailsPanel.h
    SearchWidget.cpp
    SearchWidget.h
    CategoryFilterWidget.cpp
    CategoryFilterWidget.h
    IconMetadataManager.cpp
    IconMetadataManager.h
    IconItem.cpp
    IconItem.h
)

add_executable(QtLucideGallery ${GALLERY_SOURCES})

target_link_libraries(QtLucideGallery
    PRIVATE
        QtLucide
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
)

# Set target properties
set_target_properties(QtLucideGallery PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "QtLucide Gallery"
)

# Set application icon and metadata
if(WIN32)
    set_target_properties(QtLucideGallery PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()

# Install target
install(TARGETS QtLucideGallery
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
