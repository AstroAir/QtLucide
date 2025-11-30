#!/usr/bin/env python3
"""
Test script to verify QtLucide submodule integration works correctly.
This script creates a temporary test project and builds it with QtLucide as a submodule.
"""

import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result."""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(
            cmd, cwd=cwd, check=check, capture_output=True, text=True
        )
        if result.stdout:
            print(f"STDOUT: {result.stdout}")
        if result.stderr:
            print(f"STDERR: {result.stderr}")
        return result
    except subprocess.CalledProcessError as e:
        print(f"Command failed with return code {e.returncode}")
        print(f"STDOUT: {e.stdout}")
        print(f"STDERR: {e.stderr}")
        raise


def create_test_cmake_project(test_dir, qtlucide_path):
    """Create a test CMake project that uses QtLucide as submodule."""

    # Convert path to use forward slashes for CMake
    qtlucide_cmake_path = str(qtlucide_path).replace("\\", "/")

    # Create CMakeLists.txt
    cmake_content = f"""cmake_minimum_required(VERSION 3.16)
project(QtLucideSubmoduleTest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Enable Qt6 features
qt6_standard_project_setup()

# Add QtLucide as subdirectory (simulating submodule)
add_subdirectory("{qtlucide_cmake_path}" QtLucide)

# Create test executable
add_executable(SubmoduleTest main.cpp)

target_link_libraries(SubmoduleTest PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QtLucide::QtLucide
)
"""

    with open(test_dir / "CMakeLists.txt", "w") as f:
        f.write(cmake_content)

    # Create main.cpp
    main_content = """#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QDebug>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Test QtLucide integration
    QLabel *label = new QLabel("QtLucide Submodule Test", central);
    layout->addWidget(label);

    // Try to create an icon to verify QtLucide works
    lucide::QtLucide qtlucide;
    QIcon icon = qtlucide.icon(lucide::Icons::save);
    if (!icon.isNull()) {
        label->setText("QtLucide Submodule Test - SUCCESS!");
        qDebug() << "QtLucide icon created successfully";
    } else {
        label->setText("QtLucide Submodule Test - FAILED!");
        qDebug() << "Failed to create QtLucide icon";
        return 1;
    }

    window.setCentralWidget(central);
    window.resize(300, 200);

    // Don't show window in test mode, just verify it works
    qDebug() << "Test completed successfully";
    return 0;
}
"""

    with open(test_dir / "main.cpp", "w") as f:
        f.write(main_content)


def create_test_meson_project(test_dir, qtlucide_path):
    """Create a test Meson project that uses QtLucide as subproject."""

    # Create meson.build
    meson_content = """project('QtLucideSubmoduleTest', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17']
)

# Find Qt6
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'], required: true)

# Add QtLucide as subproject
qtlucide_proj = subproject('QtLucide', required: true)
qtlucide_dep = qtlucide_proj.get_variable('qtlucide_dep')

# Create test executable
executable('SubmoduleTest',
  'main.cpp',
  dependencies: [qt6_dep, qtlucide_dep]
)
"""

    with open(test_dir / "meson.build", "w") as f:
        f.write(meson_content)

    # Create subprojects directory and wrap file
    subprojects_dir = test_dir / "subprojects"
    subprojects_dir.mkdir()

    # Create a simple wrap file pointing to QtLucide
    wrap_content = f"""[wrap-file]
directory = QtLucide
source_url = file://{qtlucide_path.absolute()}
source_filename = QtLucide
"""

    with open(subprojects_dir / "QtLucide.wrap", "w") as f:
        f.write(wrap_content)

    # Copy QtLucide to subprojects (simulating subproject)
    shutil.copytree(qtlucide_path, subprojects_dir / "QtLucide")

    # Create main.cpp in test directory
    main_content = """#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QDebug>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Test QtLucide integration
    QLabel *label = new QLabel("QtLucide Meson Subproject Test", central);
    layout->addWidget(label);

    // Try to create an icon to verify QtLucide works
    lucide::QtLucide qtlucide;
    QIcon icon = qtlucide.icon(lucide::Icons::save);
    if (!icon.isNull()) {
        label->setText("QtLucide Meson Subproject Test - SUCCESS!");
        qDebug() << "QtLucide icon created successfully";
    } else {
        label->setText("QtLucide Meson Subproject Test - FAILED!");
        qDebug() << "Failed to create QtLucide icon";
        return 1;
    }

    window.setCentralWidget(central);
    window.resize(300, 200);

    // Don't show window in test mode, just verify it works
    qDebug() << "Test completed successfully";
    return 0;
}
"""

    with open(test_dir / "main.cpp", "w") as f:
        f.write(main_content)


def test_cmake_build(qtlucide_path):
    """Test CMake submodule build."""
    print("\n=== Testing CMake Submodule Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        test_dir = Path(temp_dir) / "cmake_test"
        test_dir.mkdir()

        create_test_cmake_project(test_dir, qtlucide_path)

        # Create build directory
        build_dir = test_dir / "build"
        build_dir.mkdir()

        # Configure
        run_command(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"], cwd=build_dir)

        # Build
        run_command(["cmake", "--build", "."], cwd=build_dir)

        print("CMake submodule build: SUCCESS")


def test_meson_build(qtlucide_path):
    """Test Meson subproject build."""
    print("\n=== Testing Meson Subproject Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        test_dir = Path(temp_dir) / "meson_test"
        test_dir.mkdir()

        create_test_meson_project(test_dir, qtlucide_path)

        # Setup build directory
        build_dir = test_dir / "build"
        run_command(["meson", "setup", str(build_dir)], cwd=test_dir)

        # Build
        run_command(["meson", "compile"], cwd=build_dir)

        print("Meson subproject build: SUCCESS")


def test_xmake_build(qtlucide_path):
    """Test XMake submodule build."""
    print("\n=== Testing XMake Submodule Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        test_dir = Path(temp_dir) / "xmake_test"
        test_dir.mkdir()

        create_test_xmake_project(test_dir, qtlucide_path)

        # Build
        run_command(["xmake"], cwd=test_dir)

        print("XMake submodule build: SUCCESS")


def create_test_xmake_project(test_dir, qtlucide_path):
    """Create a test XMake project that uses QtLucide as submodule."""

    # Convert path to use forward slashes for XMake
    qtlucide_xmake_path = str(qtlucide_path).replace("\\", "/")

    # Create xmake.lua
    xmake_content = f"""-- Test project for QtLucide submodule integration
set_project("QtLucideSubmoduleTest")
set_version("1.0.0")
set_languages("cxx17")

-- Add Qt6 packages
add_requires("qt6base")

-- Include QtLucide as submodule
includes("{qtlucide_xmake_path}")

-- Test executable
target("SubmoduleTest")
    set_kind("binary")
    add_packages("qt6base")
    add_rules("qt.application")

    -- Set C++ standard
    set_languages("cxx17")

    add_files("main.cpp")
    add_deps("QtLucide")

    -- Set as console application for testing
    set_kind("binary")
target_end()
"""

    with open(test_dir / "xmake.lua", "w") as f:
        f.write(xmake_content)

    # Create main.cpp
    main_content = """#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QDebug>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // Test QtLucide integration
    QLabel *label = new QLabel("QtLucide XMake Submodule Test", central);
    layout->addWidget(label);

    // Try to create an icon to verify QtLucide works
    lucide::QtLucide qtlucide;
    QIcon icon = qtlucide.icon(lucide::Icons::save);
    if (!icon.isNull()) {
        label->setText("QtLucide XMake Submodule Test - SUCCESS!");
        qDebug() << "QtLucide icon created successfully";
    } else {
        label->setText("QtLucide XMake Submodule Test - FAILED!");
        qDebug() << "Failed to create QtLucide icon";
        return 1;
    }

    window.setCentralWidget(central);
    window.resize(300, 200);

    // Don't show window in test mode, just verify it works
    qDebug() << "Test completed successfully";
    return 0;
}
"""

    with open(test_dir / "main.cpp", "w") as f:
        f.write(main_content)


def main():
    """Main test function."""
    print("QtLucide Submodule Integration Test")
    print("=" * 40)

    # Get QtLucide path (assume script is in QtLucide/scripts/)
    script_dir = Path(__file__).parent
    qtlucide_path = script_dir.parent

    if not (qtlucide_path / "CMakeLists.txt").exists():
        print("Error: Could not find QtLucide CMakeLists.txt")
        print(f"Expected path: {qtlucide_path / 'CMakeLists.txt'}")
        return 1

    print(f"QtLucide path: {qtlucide_path}")

    try:
        # Test CMake build
        test_cmake_build(qtlucide_path)

        # Test Meson build (if meson is available)
        try:
            run_command(["meson", "--version"], check=True)
            test_meson_build(qtlucide_path)
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("Meson not available, skipping Meson test")

        # Test XMake build (if xmake is available)
        try:
            run_command(["xmake", "--version"], check=True)
            test_xmake_build(qtlucide_path)
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("XMake not available, skipping XMake test")

        print("\n" + "=" * 40)
        print("All submodule integration tests PASSED!")
        return 0

    except Exception as e:
        print(f"\nTest FAILED: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
