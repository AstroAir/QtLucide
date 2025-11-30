#!/usr/bin/env python3
"""
QtLucide CMake Build Script
Comprehensive build script for CMake build system
"""

import argparse
import shutil
import subprocess
import sys
from pathlib import Path


def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    print(f"Running: {' '.join(cmd)}")
    try:
        result = subprocess.run(
            cmd, cwd=cwd, check=check, capture_output=True, text=True
        )
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


def check_cmake_installed():
    """Check if CMake is installed"""
    try:
        result = run_command(["cmake", "--version"])
        print(f"CMake version: {result.stdout.split()[2]}")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("ERROR: CMake is not installed or not in PATH")
        return False


def check_qt6_available():
    """Check if Qt6 is available"""
    try:
        # Try to find Qt6 using cmake
        result = run_command(
            [
                "cmake",
                "--find-package",
                "-DNAME=Qt6",
                "-DCOMPILER_ID=GNU",
                "-DLANGUAGE=CXX",
                "-DMODE=EXIST",
            ],
            check=False,
        )
        return result.returncode == 0
    except Exception:
        return False


def generate_resources(project_dir):
    """Generate resources using Python script"""
    print("Generating resources...")
    build_script = project_dir / "tools" / "build_resources.py"
    if build_script.exists():
        try:
            python_cmd = "python3" if shutil.which("python3") else "python"
            run_command(
                [python_cmd, str(build_script), str(project_dir)], cwd=project_dir
            )
            print("Resources generated successfully!")
        except subprocess.CalledProcessError:
            print("Warning: Resource generation failed, continuing with build...")
    else:
        print("Warning: Resource generation script not found")


def main():
    parser = argparse.ArgumentParser(description="Build QtLucide with CMake")
    parser.add_argument("--builddir", default="build_cmake", help="Build directory")
    parser.add_argument(
        "--buildtype",
        choices=["Debug", "Release", "RelWithDebInfo", "MinSizeRel"],
        default="Debug",
        help="Build type",
    )
    parser.add_argument(
        "--generator", help='CMake generator (e.g., "Ninja", "Unix Makefiles")'
    )
    parser.add_argument(
        "--examples", action="store_true", default=True, help="Build examples"
    )
    parser.add_argument(
        "--no-examples",
        dest="examples",
        action="store_false",
        help="Don't build examples",
    )
    parser.add_argument(
        "--tests", action="store_true", default=True, help="Build tests"
    )
    parser.add_argument(
        "--no-tests", dest="tests", action="store_false", help="Don't build tests"
    )
    parser.add_argument(
        "--clean", action="store_true", help="Clean build directory first"
    )
    parser.add_argument("--install", action="store_true", help="Install after building")
    parser.add_argument("--test", action="store_true", help="Run tests after building")
    parser.add_argument(
        "--generate-resources",
        action="store_true",
        help="Generate resources before building",
    )
    parser.add_argument("--jobs", "-j", type=int, help="Number of parallel jobs")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")

    args = parser.parse_args()

    # Get project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    build_dir = project_root / args.builddir

    print("QtLucide CMake Build Script")
    print(f"Project root: {project_root}")
    print(f"Build directory: {build_dir}")

    # Check prerequisites
    if not check_cmake_installed():
        sys.exit(1)

    # Clean if requested
    if args.clean and build_dir.exists():
        print("Cleaning build directory...")
        shutil.rmtree(build_dir)

    # Generate resources if requested
    if args.generate_resources:
        generate_resources(project_root)

    # Create build directory
    build_dir.mkdir(exist_ok=True)

    # Configure
    configure_cmd = [
        "cmake",
        str(project_root),
        f"-DCMAKE_BUILD_TYPE={args.buildtype}",
        f'-DQTLUCIDE_BUILD_EXAMPLES={"ON" if args.examples else "OFF"}',
        f'-DQTLUCIDE_BUILD_TESTS={"ON" if args.tests else "OFF"}',
    ]

    if args.generator:
        configure_cmd.extend(["-G", args.generator])

    print("Configuring...")
    run_command(configure_cmd, cwd=build_dir)

    # Build
    build_cmd = ["cmake", "--build", ".", "--config", args.buildtype]
    if args.jobs:
        build_cmd.extend(["--parallel", str(args.jobs)])
    if args.verbose:
        build_cmd.append("--verbose")

    print("Building...")
    run_command(build_cmd, cwd=build_dir)

    # Run tests if requested
    if args.test and args.tests:
        print("Running tests...")
        test_cmd = ["ctest", "--output-on-failure"]
        if args.verbose:
            test_cmd.append("--verbose")
        run_command(test_cmd, cwd=build_dir, check=False)

    # Install if requested
    if args.install:
        print("Installing...")
        install_cmd = ["cmake", "--install", ".", "--config", args.buildtype]
        run_command(install_cmd, cwd=build_dir)

    print("Build completed successfully!")

    # Show summary
    print("\nBuild Summary:")
    print(f"  Build type: {args.buildtype}")
    print(f"  Generator: {args.generator or 'Default'}")
    print(f"  Examples: {args.examples}")
    print(f"  Tests: {args.tests}")
    print(f"  Build directory: {build_dir}")

    # Show built targets
    if build_dir.exists():
        print(f"\nBuilt files in {build_dir}:")
        for pattern in ["*.exe", "*.dll", "*.so", "*.dylib", "*.a", "*.lib"]:
            files = list(build_dir.rglob(pattern))
            if files:
                for file in files[:10]:  # Show first 10 files
                    print(f"  {file.relative_to(build_dir)}")
                if len(files) > 10:
                    print(f"  ... and {len(files) - 10} more files")


if __name__ == "__main__":
    main()
