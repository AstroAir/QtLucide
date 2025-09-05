cmake_minimum_required(VERSION 3.16)

project(QtLucideSubmoduleExample VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Enable Qt6 features
qt6_standard_project_setup()

# Add QtLucide as subdirectory (in real project, this would be a submodule)
# For this example, we use relative path to the parent QtLucide project
add_subdirectory(../.. QtLucide)

# Create the example executable
add_executable(QtLucideSubmoduleExample
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

# Link libraries
target_link_libraries(QtLucideSubmoduleExample PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QtLucide::QtLucide
)

# Set target properties
set_target_properties(QtLucideSubmoduleExample PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
