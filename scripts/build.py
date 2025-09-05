#!/usr/bin/env python3
"""
QtLucide Unified Build Script
Automatically detects and uses the best available build system
"""

import os
import sys
import subprocess
import argparse
import shutil
from pathlib import Path


def run_command(cmd, cwd=None, check=True):
    """Run a command and return the result"""
    try:
        result = subprocess.run(cmd, cwd=cwd, check=check,
                                capture_output=True, text=True)
        return result
    except subprocess.CalledProcessError as e:
        if check:
            print(f"Error running command: {e}")
            if e.stderr:
                print(f"Error output: {e.stderr}")
            sys.exit(1)
        return e
    except FileNotFoundError:
        return None


def check_build_system_available(system):
    """Check if a build system is available"""
    commands = {
        'cmake': ['cmake', '--version'],
        'meson': ['meson', '--version'],
        'xmake': ['xmake', '--version']
    }

    if system not in commands:
        return False

    result = run_command(commands[system], check=False)
    return result is not None and result.returncode == 0


def detect_best_build_system():
    """Detect the best available build system"""
    # Priority order: CMake (most common), Meson (fast), XMake (modern)
    systems = ['cmake', 'meson', 'xmake']

    for system in systems:
        if check_build_system_available(system):
            return system

    return None


def normalize_build_type(build_type, target_system):
    """Normalize build type for different build systems"""
    # Normalize to lowercase first
    build_type = build_type.lower()

    # Map common build types
    type_mapping = {
        'cmake': {
            'debug': 'Debug',
            'release': 'Release',
            'relwithdebinfo': 'RelWithDebInfo',
            'minsizerel': 'MinSizeRel'
        },
        'meson': {
            'debug': 'debug',
            'release': 'release',
            'relwithdebinfo': 'debugoptimized',
            'debugoptimized': 'debugoptimized',
            'minsizerel': 'release'
        },
        'xmake': {
            'debug': 'debug',
            'release': 'release',
            'relwithdebinfo': 'releasedbg',
            'releasedbg': 'releasedbg',
            'minsizerel': 'minsizerel'
        }
    }

    if target_system in type_mapping and build_type in type_mapping[target_system]:
        return type_mapping[target_system][build_type]

    # Default fallback
    defaults = {'cmake': 'Debug', 'meson': 'debug', 'xmake': 'debug'}
    return defaults.get(target_system, 'debug')


def build_with_system(system, args, script_dir):
    """Build using the specified system"""
    build_scripts = {
        'cmake': script_dir / 'build_cmake.py',
        'meson': script_dir / 'build_meson.py',
        'xmake': script_dir / 'build_xmake.py'
    }

    if system not in build_scripts:
        print(f"ERROR: Unknown build system: {system}")
        sys.exit(1)

    script_path = build_scripts[system]
    if not script_path.exists():
        print(f"ERROR: Build script not found: {script_path}")
        sys.exit(1)

    # Build command arguments
    cmd = ['python3', str(script_path)]

    # Add build type
    build_type = normalize_build_type(args.buildtype, system)
    if system == 'cmake':
        cmd.extend(['--buildtype', build_type])
    elif system == 'meson':
        cmd.extend(['--buildtype', build_type])
    elif system == 'xmake':
        cmd.extend(['--mode', build_type])

    # Add common options
    if not args.examples:
        cmd.append('--no-examples')

    if not args.tests:
        cmd.append('--no-tests')

    if args.clean:
        cmd.append('--clean')

    if args.install:
        cmd.append('--install')

    if args.test:
        cmd.append('--test')

    if args.verbose:
        cmd.append('--verbose')

    if args.generate_resources:
        cmd.append('--generate-resources')

    # System-specific options
    if system == 'cmake':
        if args.builddir:
            cmd.extend(['--builddir', args.builddir])
        if args.generator:
            cmd.extend(['--generator', args.generator])
        if args.jobs:
            cmd.extend(['--jobs', str(args.jobs)])
    elif system == 'meson':
        if args.builddir:
            cmd.extend(['--builddir', args.builddir])
        if hasattr(args, 'debug_executables') and args.debug_executables:
            cmd.append('--debug-executables')
    elif system == 'xmake':
        if args.jobs:
            cmd.extend(['--jobs', str(args.jobs)])
        if args.target:
            cmd.extend(['--target', args.target])

    print(f"Building with {system.upper()}...")
    print(f"Command: {' '.join(cmd)}")

    # Execute the build script
    result = subprocess.run(cmd)
    return result.returncode


def main():
    parser = argparse.ArgumentParser(
        description='QtLucide Unified Build Script')
    parser.add_argument('--system', choices=['cmake', 'meson', 'xmake'],
                        help='Force specific build system')
    parser.add_argument('--buildtype', default='debug',
                        help='Build type (debug, release, relwithdebinfo, minsizerel)')
    parser.add_argument('--builddir', help='Build directory')
    parser.add_argument('--generator', help='CMake generator')
    parser.add_argument('--examples', action='store_true',
                        default=True, help='Build examples')
    parser.add_argument('--no-examples', dest='examples',
                        action='store_false', help='Don\'t build examples')
    parser.add_argument('--tests', action='store_true',
                        default=True, help='Build tests')
    parser.add_argument('--no-tests', dest='tests',
                        action='store_false', help='Don\'t build tests')
    parser.add_argument('--clean', action='store_true',
                        help='Clean build first')
    parser.add_argument('--install', action='store_true',
                        help='Install after building')
    parser.add_argument('--test', action='store_true',
                        help='Run tests after building')
    parser.add_argument('--generate-resources', action='store_true',
                        help='Generate resources before building')
    parser.add_argument('--jobs', '-j', type=int,
                        help='Number of parallel jobs')
    parser.add_argument('--verbose', '-v',
                        action='store_true', help='Verbose output')
    parser.add_argument('--target', help='Specific target to build (XMake)')
    parser.add_argument('--debug-executables', action='store_true',
                        help='Build debug executables (Meson)')
    parser.add_argument('--list-systems', action='store_true',
                        help='List available build systems')

    args = parser.parse_args()

    script_dir = Path(__file__).parent

    # List available systems if requested
    if args.list_systems:
        print("Available build systems:")
        for system in ['cmake', 'meson', 'xmake']:
            available = "[+]" if check_build_system_available(
                system) else "[-]"
            print(f"  {available} {system.upper()}")
        return

    # Determine build system to use
    if args.system:
        if not check_build_system_available(args.system):
            print(f"ERROR: {args.system.upper()} is not available")
            sys.exit(1)
        build_system = args.system
    else:
        build_system = detect_best_build_system()
        if not build_system:
            print("ERROR: No build system available!")
            print("Please install one of: CMake, Meson, or XMake")
            sys.exit(1)

    print(f"QtLucide Unified Build Script")
    print(f"Using build system: {build_system.upper()}")
    print(f"Build type: {args.buildtype}")
    print()

    # Execute build
    exit_code = build_with_system(build_system, args, script_dir)
    sys.exit(exit_code)


if __name__ == '__main__':
    main()
