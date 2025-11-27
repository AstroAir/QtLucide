# Basic Usage Example

set(EXAMPLE_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

add_executable(QtLucideExample ${EXAMPLE_SOURCES})

target_link_libraries(QtLucideExample
    PRIVATE
        QtLucide
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

# Set target properties
set_target_properties(QtLucideExample PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Add smoke test for automated testing (if tests are enabled)
if(QTLUCIDE_BUILD_TESTS)
    add_test(NAME QtLucideExampleSmoke COMMAND $<TARGET_FILE:QtLucideExample> --smoke)
    set_tests_properties(QtLucideExampleSmoke PROPERTIES LABELS "example;smoke")
endif()
