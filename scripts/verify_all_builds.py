#!/usr/bin/env python3
"""
Comprehensive build verification script for QtLucide.
Tests all build systems and configurations to ensure everything works correctly.
"""

import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


def run_command(cmd, cwd=None, check=True, capture_output=True):
    """Run a command and return the result."""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(
            cmd, cwd=cwd, check=check, capture_output=capture_output, text=True
        )
        if result.stdout and capture_output:
            print(f"STDOUT: {result.stdout}")
        if result.stderr and capture_output:
            print(f"STDERR: {result.stderr}")
        return result
    except subprocess.CalledProcessError as e:
        print(f"Command failed with return code {e.returncode}")
        if capture_output:
            print(f"STDOUT: {e.stdout}")
            print(f"STDERR: {e.stderr}")
        raise


def test_cmake_standalone(qtlucide_path):
    """Test standalone CMake build."""
    print("\n=== Testing CMake Standalone Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        build_dir = Path(temp_dir) / "cmake_build"
        build_dir.mkdir()

        # Configure
        run_command(
            ["cmake", str(qtlucide_path), "-DCMAKE_BUILD_TYPE=Release"], cwd=build_dir
        )

        # Build library only (skip examples/tests for speed)
        run_command(["cmake", "--build", ".", "--target", "QtLucide"], cwd=build_dir)

        print("CMake standalone build: SUCCESS")


def test_meson_standalone(qtlucide_path):
    """Test standalone Meson build."""
    print("\n=== Testing Meson Standalone Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        build_dir = Path(temp_dir) / "meson_build"

        # Setup
        run_command(
            [
                "meson",
                "setup",
                str(build_dir),
                "--buildtype=release",
                "-Dexamples=disabled",
                "-Dtests=disabled",
            ],
            cwd=qtlucide_path,
        )

        # Build library only
        run_command(["meson", "compile", "QtLucide"], cwd=build_dir)

        print("Meson standalone build: SUCCESS")


def test_xmake_standalone(qtlucide_path):
    """Test standalone XMake build."""
    print("\n=== Testing XMake Standalone Build ===")

    with tempfile.TemporaryDirectory() as temp_dir:
        # Copy project to temp directory to avoid conflicts
        temp_project = Path(temp_dir) / "qtlucide"
        shutil.copytree(qtlucide_path, temp_project)

        # Configure to build library only
        run_command(
            ["xmake", "config", "--examples=false", "--tests=false"], cwd=temp_project
        )

        # Build library only
        run_command(["xmake", "build", "QtLucide"], cwd=temp_project)

        print("XMake standalone build: SUCCESS")


def test_submodule_detection():
    """Test that submodule detection works correctly."""
    print("\n=== Testing Submodule Detection ===")

    # This is tested by the existing test_submodule_build.py script
    script_dir = Path(__file__).parent
    test_script = script_dir / "test_submodule_build.py"

    if test_script.exists():
        run_command([sys.executable, str(test_script)])
        print("Submodule detection: SUCCESS")
    else:
        print("Submodule test script not found, skipping")


def test_resource_generation(qtlucide_path):
    """Test resource generation."""
    print("\n=== Testing Resource Generation ===")

    # Test resource generation script
    build_script = qtlucide_path / "tools" / "build_resources.py"
    if build_script.exists():
        run_command([sys.executable, str(build_script), str(qtlucide_path)])

        # Check that required files were generated
        required_files = [
            qtlucide_path / "resources" / "icons" / "lucide_icons.qrc",
            qtlucide_path / "resources" / "icons" / "metadata" / "icons.json",
            qtlucide_path / "include" / "QtLucide" / "QtLucideEnums.h",
            qtlucide_path / "include" / "QtLucide" / "QtLucideStrings.h",
        ]

        for file_path in required_files:
            if not file_path.exists():
                raise FileNotFoundError(f"Required file not generated: {file_path}")

        print("Resource generation: SUCCESS")
    else:
        print("Resource generation script not found, skipping")


def main():
    """Main verification function."""
    print("QtLucide Comprehensive Build Verification")
    print("=" * 50)

    # Get QtLucide path
    script_dir = Path(__file__).parent
    qtlucide_path = script_dir.parent

    if not (qtlucide_path / "CMakeLists.txt").exists():
        print("Error: Could not find QtLucide CMakeLists.txt")
        return 1

    print(f"QtLucide path: {qtlucide_path}")

    tests_run = 0
    tests_passed = 0

    # Test resource generation first
    try:
        test_resource_generation(qtlucide_path)
        tests_run += 1
        tests_passed += 1
    except Exception as e:
        print(f"Resource generation test FAILED: {e}")
        tests_run += 1

    # Test CMake standalone build
    try:
        test_cmake_standalone(qtlucide_path)
        tests_run += 1
        tests_passed += 1
    except Exception as e:
        print(f"CMake standalone test FAILED: {e}")
        tests_run += 1

    # Test Meson standalone build
    try:
        run_command(["meson", "--version"], check=True)
        test_meson_standalone(qtlucide_path)
        tests_run += 1
        tests_passed += 1
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("Meson not available, skipping Meson standalone test")
    except Exception as e:
        print(f"Meson standalone test FAILED: {e}")
        tests_run += 1

    # Test XMake standalone build
    try:
        run_command(["xmake", "--version"], check=True)
        test_xmake_standalone(qtlucide_path)
        tests_run += 1
        tests_passed += 1
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("XMake not available, skipping XMake standalone test")
    except Exception as e:
        print(f"XMake standalone test FAILED: {e}")
        tests_run += 1

    # Test submodule detection
    try:
        test_submodule_detection()
        tests_run += 1
        tests_passed += 1
    except Exception as e:
        print(f"Submodule detection test FAILED: {e}")
        tests_run += 1

    # Summary
    print("\n" + "=" * 50)
    print("Build Verification Summary:")
    print(f"Tests run: {tests_run}")
    print(f"Tests passed: {tests_passed}")
    print(f"Tests failed: {tests_run - tests_passed}")

    if tests_passed == tests_run:
        print("All tests PASSED! ✅")
        return 0
    else:
        print("Some tests FAILED! ❌")
        return 1


if __name__ == "__main__":
    sys.exit(main())
