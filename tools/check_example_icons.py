#!/usr/bin/env python3
"""
QtLucide Example Icon Checker

This script specifically checks icon usage in examples and provides
compile-time validation with suggestions for fixes.
"""

import re
import sys
from pathlib import Path
from typing import List, Set, Tuple


def load_available_icons(project_root: Path) -> Set[str]:
    """Load available icons from QtLucideStrings.h"""
    strings_file = project_root / "include" / "QtLucide" / "QtLucideStrings.h"

    if not strings_file.exists():
        print(f"Error: {strings_file} not found")
        return set()

    try:
        with open(strings_file, "r", encoding="utf-8") as f:
            content = f.read()

        # Extract icon names from the mapping
        pattern = r'\{Icons::\w+,\s*"([^"]+)"\}'
        matches = re.findall(pattern, content)
        return set(matches)
    except Exception as e:
        print(f"Error loading icons: {e}")
        return set()


def extract_icon_list_from_cpp(file_path: Path) -> List[Tuple[str, int]]:
    """Extract icon names from C++ string lists like m_sampleIcons << "icon1" << "icon2" """
    icons = []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Look for patterns like: << "icon-name"
        # This is more specific than the general validator
        lines = content.split("\n")
        in_icon_list = False

        for line_num, line in enumerate(lines, 1):
            # Start of icon list - look for specific patterns
            if ("m_sampleIcons" in line or "sampleIcons" in line) and "<<" in line:
                in_icon_list = True

            if in_icon_list:
                # Extract quoted strings from this line, but only if they look like icon names
                pattern = r'<<\s*"([a-z][a-z0-9-]*[a-z0-9])"'
                matches = re.findall(pattern, line)
                for match in matches:
                    # Additional filter: should look like an icon name
                    if (
                        len(match) >= 2
                        and len(match) <= 30
                        and "-" in match
                        or len(match) <= 15
                    ):
                        icons.append((match, line_num))

                # End of icon list (semicolon without <<)
                if ";" in line and "<<" not in line:
                    in_icon_list = False

    except Exception as e:
        print(f"Error reading {file_path}: {e}")

    return icons


def suggest_similar_icons(invalid_icon: str, available_icons: Set[str]) -> List[str]:
    """Suggest similar icon names"""
    suggestions = []

    # Common mappings first
    common_mappings = {
        "edit": ["pencil", "pen", "square-pen"],
        "home": ["house", "house-plus"],
        "delete": ["trash", "trash-2"],
        "remove": ["x", "minus", "trash"],
        "add": ["plus", "plus-circle"],
        "close": ["x", "x-circle"],
        "options": ["settings", "cog"],
        "config": ["settings", "cog"],
        "preferences": ["settings", "sliders-horizontal"],
    }

    if invalid_icon in common_mappings:
        # Filter to only include icons that actually exist
        mapped_suggestions = [
            icon for icon in common_mappings[invalid_icon] if icon in available_icons
        ]
        suggestions.extend(mapped_suggestions)

    # Exact substring matches
    for icon in available_icons:
        if invalid_icon in icon or icon in invalid_icon:
            if icon not in suggestions:
                suggestions.append(icon)

    # Word-based similarity
    invalid_words = set(invalid_icon.split("-"))
    for icon in available_icons:
        icon_words = set(icon.split("-"))
        if invalid_words.intersection(icon_words):
            if icon not in suggestions:
                suggestions.append(icon)

    # Remove duplicates and limit
    return suggestions[:5]


def check_examples(project_root: Path) -> bool:
    """Check icon usage in examples"""
    print("QtLucide Example Icon Checker")
    print("=" * 40)

    # Load available icons
    available_icons = load_available_icons(project_root)
    if not available_icons:
        return False

    print(f"Loaded {len(available_icons)} available icons")

    # Check examples
    examples_dir = project_root / "examples"
    if not examples_dir.exists():
        print("No examples directory found")
        return True

    all_valid = True
    total_checked = 0

    # Check specific example files
    example_files = [
        examples_dir / "basic_usage" / "MainWindow.cpp",
        examples_dir / "gallery" / "main.cpp",
    ]

    for file_path in example_files:
        if not file_path.exists():
            continue

        print(f"\nChecking {file_path.relative_to(project_root)}...")
        icons = extract_icon_list_from_cpp(file_path)

        if not icons:
            print("  No icon lists found")
            continue

        invalid_icons = []
        for icon_name, line_num in icons:
            total_checked += 1
            if icon_name not in available_icons:
                invalid_icons.append((icon_name, line_num))
                all_valid = False

        if invalid_icons:
            print(f"  ❌ Found {len(invalid_icons)} invalid icons:")
            for icon_name, line_num in invalid_icons:
                print(f"    Line {line_num}: '{icon_name}'")
                suggestions = suggest_similar_icons(icon_name, available_icons)
                if suggestions:
                    print(f"      Suggestions: {', '.join(suggestions[:3])}")
        else:
            print(f"  ✅ All {len(icons)} icons are valid")

    print("\nSummary:")
    print(f"  Total icons checked: {total_checked}")
    print(f"  Result: {'PASS' if all_valid else 'FAIL'}")

    return all_valid


def generate_icon_fix_suggestions(project_root: Path) -> None:
    """Generate a file with suggested fixes"""
    available_icons = load_available_icons(project_root)
    if not available_icons:
        return

    # Common icon name mappings that users might expect
    common_mappings = {
        "edit": ["pencil", "pen", "square-pen"],
        "home": ["house", "house-plus"],
        "delete": ["trash", "trash-2"],
        "remove": ["x", "minus", "trash"],
        "add": ["plus", "plus-circle"],
        "close": ["x", "x-circle"],
        "menu": ["menu", "more-horizontal"],
        "options": ["settings", "cog"],
        "config": ["settings", "cog"],
        "preferences": ["settings", "sliders-horizontal"],
    }

    fix_file = project_root / "ICON_NAME_FIXES.md"

    with open(fix_file, "w", encoding="utf-8") as f:
        f.write("# QtLucide Icon Name Reference\n\n")
        f.write("Common icon name mappings for QtLucide (based on Lucide icons):\n\n")

        for expected, actual_options in common_mappings.items():
            # Filter to only include icons that actually exist
            existing_options = [
                icon for icon in actual_options if icon in available_icons
            ]
            if existing_options:
                f.write(f"- `{expected}` → `{existing_options[0]}`")
                if len(existing_options) > 1:
                    f.write(f" (alternatives: {', '.join(existing_options[1:])})")
                f.write("\n")

        f.write(f"\n## All Available Icons ({len(available_icons)} total)\n\n")
        f.write("```\n")
        for icon in sorted(available_icons):
            f.write(f"{icon}\n")
        f.write("```\n")

    print(f"Generated icon reference: {fix_file}")


def main():
    if len(sys.argv) < 2:
        print("Usage: check_example_icons.py <project_root> [stamp_file]")
        sys.exit(1)

    project_root = Path(sys.argv[1])
    stamp_file = Path(sys.argv[2]) if len(sys.argv) > 2 else None

    # Check examples
    success = check_examples(project_root)

    # Generate reference file
    generate_icon_fix_suggestions(project_root)

    if success:
        print("\n✅ All example icons are valid!")

        # Create stamp file if requested (for build system integration)
        if stamp_file:
            stamp_file.touch()
            print(f"Created stamp file: {stamp_file}")

        sys.exit(0)
    else:
        print("\n❌ Some example icons need fixing!")
        print("See ICON_NAME_FIXES.md for suggestions.")
        sys.exit(1)


if __name__ == "__main__":
    main()
