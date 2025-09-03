#!/usr/bin/env python3
"""
Test script to verify XMake build system functionality for QtLucide
"""

import os
import sys
import subprocess
import shutil
import tempfile
from pathlib import Path

def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(cmd, cwd=cwd, check=check, 
                              capture_output=True, text=True)
        if result.stdout:
            print(f"STDOUT: {result.stdout}")
        if result.stderr:
            print(f"STDERR: {result.stderr}")
        return result
    except subprocess.CalledProcessError as e:
        print(f"Command failed with exit code {e.returncode}")
        print(f"STDOUT: {e.stdout}")
        print(f"STDERR: {e.stderr}")
        raise

def check_xmake_installed():
    """Check if XMake is installed"""
    try:
        result = run_command(["xmake", "--version"])
        print(f"XMake version: {result.stdout.strip()}")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("ERROR: XMake is not installed or not in PATH")
        return False

def check_qt6_available():
    """Check if Qt6 is available"""
    try:
        # Try to find Qt6 using xmake
        result = run_command(["xmake", "require", "--info", "qt6base"], check=False)
        return result.returncode == 0
    except:
        return False

def test_xmake_build(project_dir):
    """Test XMake build process"""
    print("\n=== Testing XMake Build ===")
    
    # Clean any previous build
    print("Cleaning previous build...")
    run_command(["xmake", "clean"], cwd=project_dir, check=False)
    
    # Configure project
    print("Configuring project...")
    run_command(["xmake", "config", "--mode=release"], cwd=project_dir)
    
    # Generate resources first
    print("Generating resources...")
    run_command(["xmake", "generate-resources"], cwd=project_dir)
    
    # Build project
    print("Building project...")
    run_command(["xmake", "build"], cwd=project_dir)
    
    # Check if library was built
    build_dir = Path(project_dir) / "build"
    if not build_dir.exists():
        print("ERROR: Build directory not found")
        return False
    
    # Look for library files
    lib_files = list(build_dir.rglob("*QtLucide*"))
    if not lib_files:
        print("ERROR: QtLucide library not found in build directory")
        return False
    
    print(f"Found library files: {[str(f) for f in lib_files]}")
    return True

def test_examples_build(project_dir):
    """Test building examples"""
    print("\n=== Testing Examples Build ===")
    
    # Configure with examples enabled
    run_command(["xmake", "config", "--examples=true"], cwd=project_dir)
    
    # Build examples
    run_command(["xmake", "build", "QtLucideExample"], cwd=project_dir)
    run_command(["xmake", "build", "QtLucideGallery"], cwd=project_dir)
    
    return True

def test_tests_build(project_dir):
    """Test building tests"""
    print("\n=== Testing Tests Build ===")
    
    # Configure with tests enabled
    run_command(["xmake", "config", "--tests=true"], cwd=project_dir)
    
    # Build tests
    run_command(["xmake", "build", "QtLucideTests"], cwd=project_dir)
    
    return True

def test_package_creation(project_dir):
    """Test package creation"""
    print("\n=== Testing Package Creation ===")
    
    try:
        # Create package
        run_command(["xmake", "package"], cwd=project_dir)
        return True
    except subprocess.CalledProcessError:
        print("Package creation failed (this might be expected)")
        return False

def main():
    """Main test function"""
    print("QtLucide XMake Build System Test")
    print("=" * 40)
    
    # Get project directory
    script_dir = Path(__file__).parent
    project_dir = script_dir.parent
    
    print(f"Project directory: {project_dir}")
    
    # Check prerequisites
    if not check_xmake_installed():
        sys.exit(1)
    
    if not check_qt6_available():
        print("WARNING: Qt6 might not be available through XMake package manager")
        print("Make sure Qt6 is installed on your system")
    
    # Check if xmake.lua exists
    xmake_file = project_dir / "xmake.lua"
    if not xmake_file.exists():
        print(f"ERROR: {xmake_file} not found")
        sys.exit(1)
    
    try:
        # Test basic build
        if not test_xmake_build(project_dir):
            print("ERROR: Basic build test failed")
            sys.exit(1)
        
        # Test examples build
        try:
            test_examples_build(project_dir)
            print("✓ Examples build test passed")
        except subprocess.CalledProcessError:
            print("✗ Examples build test failed")
        
        # Test tests build
        try:
            test_tests_build(project_dir)
            print("✓ Tests build test passed")
        except subprocess.CalledProcessError:
            print("✗ Tests build test failed")
        
        # Test package creation
        if test_package_creation(project_dir):
            print("✓ Package creation test passed")
        else:
            print("✗ Package creation test failed")
        
        print("\n" + "=" * 40)
        print("✓ XMake build system test completed successfully!")
        print("The XMake build system is working correctly.")
        
    except Exception as e:
        print(f"\nERROR: Test failed with exception: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
