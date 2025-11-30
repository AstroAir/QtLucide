#!/usr/bin/env python3
"""
QtLucide XMake Build Script
Comprehensive build script for XMake build system
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
    except Exception:
        return False


def generate_resources(project_dir):
    """Generate resources using XMake task"""
    print("Generating resources...")
    try:
        run_command(["xmake", "generate-resources"], cwd=project_dir)
        print("Resources generated successfully!")
    except subprocess.CalledProcessError:
        print("Warning: Resource generation failed, trying Python script...")
        build_script = project_dir / "tools" / "build_resources.py"
        if build_script.exists():
            try:
                python_cmd = "python3" if shutil.which("python3") else "python"
                run_command(
                    [python_cmd, str(build_script), str(project_dir)], cwd=project_dir
                )
                print("Resources generated successfully using Python script!")
            except subprocess.CalledProcessError:
                print("Warning: Resource generation failed, continuing with build...")
        else:
            print("Warning: Resource generation script not found")


def main():
    parser = argparse.ArgumentParser(description="Build QtLucide with XMake")
    parser.add_argument(
        "--mode",
        choices=["debug", "release", "releasedbg", "minsizerel"],
        default="debug",
        help="Build mode",
    )
    parser.add_argument("--arch", help="Target architecture (e.g., x86_64, x86)")
    parser.add_argument("--plat", help="Target platform (e.g., windows, linux, macosx)")
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
    parser.add_argument("--clean", action="store_true", help="Clean build first")
    parser.add_argument("--rebuild", action="store_true", help="Rebuild all targets")
    parser.add_argument("--install", action="store_true", help="Install after building")
    parser.add_argument("--test", action="store_true", help="Run tests after building")
    parser.add_argument(
        "--generate-resources",
        action="store_true",
        help="Generate resources before building",
    )
    parser.add_argument("--jobs", "-j", type=int, help="Number of parallel jobs")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--target", help="Specific target to build")

    args = parser.parse_args()

    # Get project root directory
    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    print("QtLucide XMake Build Script")
    print(f"Project root: {project_root}")

    # Check prerequisites
    if not check_xmake_installed():
        sys.exit(1)

    # Check if xmake.lua exists
    xmake_file = project_root / "xmake.lua"
    if not xmake_file.exists():
        print(f"ERROR: {xmake_file} not found")
        sys.exit(1)

    # Clean if requested
    if args.clean:
        print("Cleaning...")
        run_command(["xmake", "clean"], cwd=project_root, check=False)

    # Generate resources if requested
    if args.generate_resources:
        generate_resources(project_root)

    # Configure
    config_cmd = ["xmake", "config"]
    config_cmd.append(f"--mode={args.mode}")

    if args.arch:
        config_cmd.append(f"--arch={args.arch}")

    if args.plat:
        config_cmd.append(f"--plat={args.plat}")

    config_cmd.append(f"--examples={str(args.examples).lower()}")
    config_cmd.append(f"--tests={str(args.tests).lower()}")

    if args.verbose:
        config_cmd.append("--verbose")

    print("Configuring...")
    run_command(config_cmd, cwd=project_root)

    # Build
    build_cmd = ["xmake", "build"]

    if args.rebuild:
        build_cmd = ["xmake", "rebuild"]

    if args.target:
        build_cmd.append(args.target)

    if args.jobs:
        build_cmd.extend(["-j", str(args.jobs)])

    if args.verbose:
        build_cmd.append("-v")

    print("Building...")
    run_command(build_cmd, cwd=project_root)

    # Run tests if requested
    if args.test and args.tests:
        print("Running tests...")
        try:
            run_command(
                ["xmake", "run", "QtLucideTests"], cwd=project_root, check=False
            )
        except subprocess.CalledProcessError:
            print("Tests failed or test target not found")

    # Install if requested
    if args.install:
        print("Installing...")
        run_command(["xmake", "install"], cwd=project_root)

    print("Build completed successfully!")

    # Show summary
    print("\nBuild Summary:")
    print(f"  Build mode: {args.mode}")
    print(f"  Architecture: {args.arch or 'Default'}")
    print(f"  Platform: {args.plat or 'Default'}")
    print(f"  Examples: {args.examples}")
    print(f"  Tests: {args.tests}")

    # Show built targets
    build_dir = project_root / "build"
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
