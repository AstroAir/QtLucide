#!/usr/bin/env python3
"""
QtLucide Meson Build Script
Equivalent to the existing CMake and XMake build scripts
"""

import os
import sys
import subprocess
import argparse
from pathlib import Path

def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(cmd, cwd=cwd, check=check, capture_output=True, text=True)
        if result.stdout:
            print(result.stdout)
        return result
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}")
        if e.stderr:
            print(f"Error output: {e.stderr}")
        if check:
            sys.exit(1)
        return e

def main():
    parser = argparse.ArgumentParser(description='Build QtLucide with Meson')
    parser.add_argument('--builddir', default='build_meson', help='Build directory')
    parser.add_argument('--buildtype', choices=['debug', 'release', 'debugoptimized'], 
                       default='debug', help='Build type')
    parser.add_argument('--examples', action='store_true', default=True, help='Build examples')
    parser.add_argument('--no-examples', dest='examples', action='store_false', help='Don\'t build examples')
    parser.add_argument('--tests', action='store_true', default=True, help='Build tests')
    parser.add_argument('--no-tests', dest='tests', action='store_false', help='Don\'t build tests')
    parser.add_argument('--debug-executables', action='store_true', help='Build debug executables')
    parser.add_argument('--clean', action='store_true', help='Clean build directory first')
    parser.add_argument('--install', action='store_true', help='Install after building')
    parser.add_argument('--test', action='store_true', help='Run tests after building')
    
    args = parser.parse_args()
    
    # Get project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    build_dir = project_root / args.builddir
    
    print(f"QtLucide Meson Build Script")
    print(f"Project root: {project_root}")
    print(f"Build directory: {build_dir}")
    
    # Clean if requested
    if args.clean and build_dir.exists():
        print("Cleaning build directory...")
        import shutil
        shutil.rmtree(build_dir)
    
    # Setup build directory
    setup_cmd = [
        'meson', 'setup', str(build_dir),
        f'--buildtype={args.buildtype}',
        f'-Dexamples={str(args.examples).lower()}',
        f'-Dtests={str(args.tests).lower()}',
        f'-Ddebug_executables={str(args.debug_executables).lower()}'
    ]
    
    if not build_dir.exists():
        print("Setting up build directory...")
        run_command(setup_cmd, cwd=project_root)
    else:
        print("Build directory exists, reconfiguring...")
        run_command(['meson', 'configure'] + setup_cmd[3:], cwd=build_dir)
    
    # Build
    print("Building...")
    run_command(['meson', 'compile'], cwd=build_dir)
    
    # Run tests if requested
    if args.test and args.tests:
        print("Running tests...")
        run_command(['meson', 'test'], cwd=build_dir, check=False)
    
    # Install if requested
    if args.install:
        print("Installing...")
        run_command(['meson', 'install'], cwd=build_dir)
    
    print("Build completed successfully!")
    
    # Show summary
    print("\nBuild Summary:")
    print(f"  Build type: {args.buildtype}")
    print(f"  Examples: {args.examples}")
    print(f"  Tests: {args.tests}")
    print(f"  Debug executables: {args.debug_executables}")
    print(f"  Build directory: {build_dir}")

if __name__ == '__main__':
    main()
