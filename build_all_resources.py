#!/usr/bin/env python3
"""
QtLucide - Build All Resources
Convenience script to build all QtLucide resources and prepare for compilation.
"""

import subprocess
import sys
from pathlib import Path


def main():
    """Main build script"""
    project_root = Path(__file__).parent
    tools_dir = project_root / "tools"
    build_script = tools_dir / "build_resources.py"

    print("QtLucide - Building All Resources")
    print("=" * 40)

    # Check if build script exists
    if not build_script.exists():
        print(f"Error: Build script not found at {build_script}")
        sys.exit(1)

    # Run the resource builder
    cmd = [sys.executable, str(build_script), str(project_root)]

    try:
        subprocess.run(cmd, check=True)
        print("\n" + "=" * 40)
        print("✅ Resource build completed successfully!")
        print("\nNext steps:")
        print("1. Run 'cmake --build build' to compile the library")
        print("2. All SVG icons will be embedded in the compiled library")
        print("3. Use the generated enums and functions in your Qt application")

    except subprocess.CalledProcessError as e:
        print(f"\n❌ Resource build failed with exit code {e.returncode}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n⚠️  Build interrupted by user")
        sys.exit(1)


if __name__ == "__main__":
    main()
