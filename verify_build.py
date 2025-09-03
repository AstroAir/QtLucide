#!/usr/bin/env python3
"""
QtLucide Build Verification Script

This script verifies that all components of QtLucide can be built successfully
using both CMake and xmake build systems.
"""

import os
import sys
import subprocess
import time
from pathlib import Path

def run_command(cmd, cwd=None, timeout=300):
    """Run a command and return success status and output."""
    try:
        print(f"Running: {cmd}")
        result = subprocess.run(
            cmd,
            shell=True,
            cwd=cwd,
            capture_output=True,
            text=True,
            timeout=timeout
        )
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        print(f"Command timed out: {cmd}")
        return False, "", "Timeout"
    except Exception as e:
        print(f"Error running command: {e}")
        return False, "", str(e)

def verify_cmake_build():
    """Verify CMake build system."""
    print("\n=== Verifying CMake Build ===")

    # Check if CMake build exists
    if not Path("build/QtLucideTests.exe").exists():
        print("CMake build not found, building...")
        success, stdout, stderr = run_command("cmake --build build --config Release")
        if not success:
            print(f"CMake build failed: {stderr}")
            return False

    # Run tests
    print("Running CMake-built tests...")
    success, stdout, stderr = run_command("build\\QtLucideTests.exe")
    if not success:
        print(f"CMake tests failed: {stderr}")
        return False

    # Check for test results
    if "passed" in stdout and "0 failed" in stdout:
        print("✓ CMake build and tests successful")
        return True
    else:
        print("✗ CMake tests failed")
        return False

def verify_xmake_build():
    """Verify xmake build system."""
    print("\n=== Verifying xmake Build ===")

    # Clean and rebuild with xmake
    print("Cleaning xmake build...")
    run_command("xmake clean")

    print("Configuring xmake...")
    success, stdout, stderr = run_command("xmake config --toolchain=mingw")
    if not success:
        print(f"xmake config failed: {stderr}")
        return False

    print("Building with xmake...")
    success, stdout, stderr = run_command("xmake build")
    if not success:
        print(f"xmake build failed: {stderr}")
        return False

    # Run tests
    print("Running xmake-built tests...")
    success, stdout, stderr = run_command("build\\windows\\x64\\release\\QtLucideTests.exe")
    if not success:
        print(f"xmake tests failed: {stderr}")
        return False

    # Check for test results
    if "passed" in stdout and "0 failed" in stdout:
        print("✓ xmake build and tests successful")
        return True
    else:
        print("✗ xmake tests failed")
        return False

def verify_resources():
    """Verify resource generation."""
    print("\n=== Verifying Resource Generation ===")

    # Check if resources exist
    qrc_file = Path("resources/icons/lucide_icons.qrc")
    if not qrc_file.exists():
        print("Generating resources...")
        success, stdout, stderr = run_command("python tools/build_resources.py .")
        if not success:
            print(f"Resource generation failed: {stderr}")
            return False

    # Check resource file content
    with open(qrc_file, 'r') as f:
        content = f.read()
        if "/lucide" in content and ".svg" in content and "RCC" in content:
            print("✓ Resources generated successfully")
            return True
        else:
            print("✗ Resource file appears invalid")
            print(f"Content preview: {content[:200]}...")
            return False

def main():
    """Main verification function."""
    print("QtLucide Build Verification")
    print("=" * 50)

    # Change to project directory
    project_dir = Path(__file__).parent
    os.chdir(project_dir)

    results = []

    # Verify resources
    results.append(("Resource Generation", verify_resources()))

    # Verify CMake build
    results.append(("CMake Build", verify_cmake_build()))

    # Verify xmake build
    results.append(("xmake Build", verify_xmake_build()))

    # Print summary
    print("\n" + "=" * 50)
    print("BUILD VERIFICATION SUMMARY")
    print("=" * 50)

    all_passed = True
    for name, passed in results:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"{name:20} {status}")
        if not passed:
            all_passed = False

    print("=" * 50)
    if all_passed:
        print("🎉 All builds verified successfully!")
        return 0
    else:
        print("❌ Some builds failed verification")
        return 1

if __name__ == "__main__":
    sys.exit(main())
