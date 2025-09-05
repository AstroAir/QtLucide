#!/usr/bin/env python3
"""
QtLucide Resource Builder for Meson
Wrapper around build_resources.py that creates a stamp file for Meson
"""

import os
import sys
import subprocess
from pathlib import Path

def main():
    if len(sys.argv) < 3:
        print("Usage: build_resources_meson.py <project_root> <stamp_file>")
        sys.exit(1)
    
    project_root = sys.argv[1]
    stamp_file = sys.argv[2]
    
    # Run the original build_resources.py script
    build_script = Path(__file__).parent / "build_resources.py"
    
    try:
        result = subprocess.run([sys.executable, str(build_script), project_root], 
                              check=True, capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print(result.stderr, file=sys.stderr)
        
        # Create stamp file to indicate success
        Path(stamp_file).touch()
        print(f"Resource generation completed, stamp file created: {stamp_file}")
        
    except subprocess.CalledProcessError as e:
        print(f"Error running build_resources.py: {e}")
        if e.stdout:
            print(e.stdout)
        if e.stderr:
            print(e.stderr, file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
