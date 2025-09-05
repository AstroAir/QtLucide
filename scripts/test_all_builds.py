#!/usr/bin/env python3
"""
Test script to verify all QtLucide build systems are working
"""

import os
import sys
import subprocess
import shutil
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
        if check:
            raise
        return e
    except FileNotFoundError:
        print(f"Command not found: {cmd[0]}")
        return None


def check_build_system(system):
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


def test_build_script(script_path, system_name):
    """Test a build script with --help option"""
    print(f"\n=== Testing {system_name} Build Script ===")

    if not script_path.exists():
        print(f"[-] Script not found: {script_path}")
        return False

    try:
        # Test help option
        result = run_command(
            ['python3', str(script_path), '--help'], check=False)
        if result is None:
            result = run_command(
                ['python', str(script_path), '--help'], check=False)

        if result and result.returncode == 0:
            print(f"[+] {system_name} script help works")
            return True
        else:
            print(f"[-] {system_name} script help failed")
            return False
    except Exception as e:
        print(f"[-] {system_name} script test failed: {e}")
        return False


def test_unified_script(script_path):
    """Test the unified build script"""
    print(f"\n=== Testing Unified Build Script ===")

    if not script_path.exists():
        print(f"[-] Unified script not found: {script_path}")
        return False

    try:
        # Test list systems option
        result = run_command(
            ['python3', str(script_path), '--list-systems'], check=False)
        if result is None:
            result = run_command(
                ['python', str(script_path), '--list-systems'], check=False)

        if result and result.returncode == 0:
            print(f"[+] Unified script --list-systems works")

            # Test help option
            result = run_command(
                ['python3', str(script_path), '--help'], check=False)
            if result is None:
                result = run_command(
                    ['python', str(script_path), '--help'], check=False)

            if result and result.returncode == 0:
                print(f"[+] Unified script help works")
                return True
            else:
                print(f"[-] Unified script help failed")
                return False
        else:
            print(f"[-] Unified script --list-systems failed")
            return False
    except Exception as e:
        print(f"[-] Unified script test failed: {e}")
        return False


def main():
    """Main test function"""
    print("QtLucide Build Scripts Test")
    print("=" * 40)

    script_dir = Path(__file__).parent
    project_root = script_dir.parent

    print(f"Script directory: {script_dir}")
    print(f"Project root: {project_root}")

    # Check available build systems
    print("\n=== Checking Build Systems ===")
    available_systems = []
    for system in ['cmake', 'meson', 'xmake']:
        if check_build_system(system):
            print(f"[+] {system.upper()} is available")
            available_systems.append(system)
        else:
            print(f"[-] {system.upper()} is not available")

    if not available_systems:
        print("\n[-] No build systems available! Please install at least one.")
        sys.exit(1)

    # Test individual build scripts
    print("\n=== Testing Individual Build Scripts ===")
    scripts_to_test = [
        (script_dir / 'build_cmake.py', 'CMake'),
        (script_dir / 'build_meson.py', 'Meson'),
        (script_dir / 'build_xmake.py', 'XMake')
    ]

    script_results = []
    for script_path, system_name in scripts_to_test:
        result = test_build_script(script_path, system_name)
        script_results.append((system_name, result))

    # Test unified build script
    unified_result = test_unified_script(script_dir / 'build.py')

    # Test batch files exist (Windows)
    print("\n=== Checking Batch Files ===")
    batch_files = [
        'build.bat',
        'build_cmake.bat',
        'build_meson.bat',
        'build_xmake.bat'
    ]

    batch_results = []
    for batch_file in batch_files:
        batch_path = script_dir / batch_file
        if batch_path.exists():
            print(f"[+] {batch_file} exists")
            batch_results.append(True)
        else:
            print(f"[-] {batch_file} missing")
            batch_results.append(False)

    # Check README exists
    readme_path = script_dir / 'README.md'
    readme_exists = readme_path.exists()
    if readme_exists:
        print(f"[+] README.md exists")
    else:
        print(f"[-] README.md missing")

    # Summary
    print("\n" + "=" * 40)
    print("TEST SUMMARY")
    print("=" * 40)

    print(f"Available build systems: {len(available_systems)}/3")
    for system in available_systems:
        print(f"  [+] {system.upper()}")

    print(
        f"\nBuild scripts: {sum(1 for _, result in script_results if result)}/{len(script_results)}")
    for system_name, result in script_results:
        status = "[+]" if result else "[-]"
        print(f"  {status} {system_name}")

    print(f"\nUnified script: {'[+]' if unified_result else '[-]'}")

    print(f"\nBatch files: {sum(batch_results)}/{len(batch_results)}")
    for i, batch_file in enumerate(batch_files):
        status = "[+]" if batch_results[i] else "[-]"
        print(f"  {status} {batch_file}")

    print(f"\nDocumentation: {'[+]' if readme_exists else '[-]'} README.md")

    # Overall result
    all_scripts_ok = all(result for _, result in script_results)
    all_batch_ok = all(batch_results)

    if available_systems and all_scripts_ok and unified_result and all_batch_ok and readme_exists:
        print("\n[SUCCESS] ALL TESTS PASSED!")
        print("QtLucide build scripts are ready to use.")
        return 0
    else:
        print("\n[FAILED] SOME TESTS FAILED!")
        print("Please check the issues above.")
        return 1


if __name__ == "__main__":
    sys.exit(main())
