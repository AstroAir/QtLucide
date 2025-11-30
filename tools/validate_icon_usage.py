#!/usr/bin/env python3
"""
QtLucide Icon Usage Validator

This script validates that all icon names used in examples and tests
actually exist in the QtLucide icon set. It provides compile-time
checking to prevent runtime icon loading failures.
"""

import json
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple


class IconUsageValidator:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.available_icons: Set[str] = set()
        self.icon_usage: Dict[str, List[Tuple[str, int]]] = {}
        self.invalid_icons: Dict[str, List[Tuple[str, int]]] = {}

    def load_available_icons(self) -> bool:
        """Load the list of available icons from metadata or generated files."""
        # Try to load from QtLucideStrings.h
        strings_file = self.project_root / "include" / "QtLucide" / "QtLucideStrings.h"
        if strings_file.exists():
            return self._load_from_strings_header(strings_file)

        # Try to load from metadata
        metadata_file = (
            self.project_root / "resources" / "icons" / "metadata" / "icons.json"
        )
        if metadata_file.exists():
            return self._load_from_metadata(metadata_file)

        # Try to load from SVG files directly
        svg_dir = self.project_root / "resources" / "icons" / "svg"
        if svg_dir.exists():
            return self._load_from_svg_files(svg_dir)

        print("Error: Could not find icon definitions")
        return False

    def _load_from_strings_header(self, strings_file: Path) -> bool:
        """Load icons from QtLucideStrings.h file."""
        try:
            with open(strings_file, "r", encoding="utf-8") as f:
                content = f.read()

            # Extract icon names from the mapping
            pattern = r'\{Icons::\w+,\s*"([^"]+)"\}'
            matches = re.findall(pattern, content)
            self.available_icons = set(matches)
            print(f"Loaded {len(self.available_icons)} icons from {strings_file}")
            return True
        except Exception as e:
            print(f"Error loading from {strings_file}: {e}")
            return False

    def _load_from_metadata(self, metadata_file: Path) -> bool:
        """Load icons from metadata JSON file."""
        try:
            with open(metadata_file, "r", encoding="utf-8") as f:
                metadata = json.load(f)

            if isinstance(metadata, dict):
                self.available_icons = set(metadata.keys())
            elif isinstance(metadata, list):
                self.available_icons = set(
                    item.get("name", "") for item in metadata if "name" in item
                )

            print(f"Loaded {len(self.available_icons)} icons from {metadata_file}")
            return True
        except Exception as e:
            print(f"Error loading from {metadata_file}: {e}")
            return False

    def _load_from_svg_files(self, svg_dir: Path) -> bool:
        """Load icons from SVG files in directory."""
        try:
            svg_files = list(svg_dir.glob("*.svg"))
            self.available_icons = set(f.stem for f in svg_files)
            print(f"Loaded {len(self.available_icons)} icons from {svg_dir}")
            return True
        except Exception as e:
            print(f"Error loading from {svg_dir}: {e}")
            return False

    def scan_file_for_icons(self, file_path: Path) -> List[Tuple[str, int]]:
        """Scan a source file for icon usage patterns."""
        icon_usages = []

        try:
            with open(file_path, "r", encoding="utf-8") as f:
                lines = f.readlines()

            for line_num, line in enumerate(lines, 1):
                # Pattern 1: String literals that look like icon names
                string_patterns = [
                    r'"([a-z][a-z0-9-]*[a-z0-9])"',  # "icon-name"
                    r"'([a-z][a-z0-9-]*[a-z0-9])'",  # 'icon-name'
                ]

                for pattern in string_patterns:
                    matches = re.findall(pattern, line)
                    for match in matches:
                        # Filter out obvious non-icon strings
                        if self._looks_like_icon_name(match):
                            icon_usages.append((match, line_num))

                # Pattern 2: QtLucide method calls
                qtlucide_patterns = [
                    r'\.icon\s*\(\s*["\']([^"\']+)["\']',  # .icon("name")
                    r'\.setIcon\s*\(\s*["\']([^"\']+)["\']',  # .setIcon("name")
                    r'QtLucide::\w+\s*\(\s*["\']([^"\']+)["\']',  # QtLucide::method("name")
                ]

                for pattern in qtlucide_patterns:
                    matches = re.findall(pattern, line)
                    for match in matches:
                        icon_usages.append((match, line_num))

        except Exception as e:
            print(f"Error scanning {file_path}: {e}")

        return icon_usages

    def _looks_like_icon_name(self, name: str) -> bool:
        """Check if a string looks like an icon name."""
        # Basic heuristics for icon names
        if len(name) < 2 or len(name) > 50:
            return False

        # Should contain only lowercase letters, numbers, and hyphens
        if not re.match(r"^[a-z][a-z0-9-]*[a-z0-9]$", name):
            return False

        # Should not be common non-icon strings
        non_icon_words = {
            # Common programming terms
            "error",
            "warning",
            "info",
            "debug",
            "test",
            "example",
            "sample",
            "main",
            "window",
            "widget",
            "button",
            "label",
            "layout",
            "dialog",
            "application",
            "version",
            "author",
            "license",
            "copyright",
            "true",
            "false",
            "null",
            "undefined",
            "none",
            "empty",
            # UI/CSS terms
            "color",
            "size",
            "width",
            "height",
            "opacity",
            "scale-factor",
            "geometry",
            "icon",
            "svg",
            "png",
            "ico",
            "pdf",
            "icns",
            "selected",
            "hovered",
            "favorite",
            "name",
            "format",
            "mode",
            "clicked",
            "about",
            "value",
            "count",
            "icons",
            "px",
            "ms",
            # Theme/appearance terms
            "theme",
            "system",
            "light",
            "dark",
            "compact",
            "grid",
            "language",
            "en",
            "black",
            "white",
            # Test-specific terms
            "non-existent-icon",
            "test-icon",
            "invalid-icon",
            "example-icon",
            "heart-icon",
            "icon-heart",
            "test-null",
            "not-a-color",
            "not-a-number",
            "corrupted-test",
            "null-painter",
            "invalid-painter",
            "exception-painter",
            "test-painter",
            "memory-test-painter",
            "replacement-painter",
            "lifetime-painter",
            "non-existent",
            "invalid",
            "unknown",
            "high-contrast",
            "color-disabled",
            "test-option",
            "test-value",
            "race-painter",
            "extreme-painter",
            "memory-painter",
            "slow-painter",
            # Data/metadata terms
            "categories",
            "contributors",
            "favorites",
            "timestamp",
            "usage",
            "recent",
            "criteria",
            "quotes",
            "with",
            "nested",
            "level1",
            "level2",
            "level3",
            "value1",
            "value2",
            "inner",
            "x64",
            # Generic terms
            "arrows",
            "communication",
            "media",
            "cal",
            "arrow",
        }

        if name in non_icon_words:
            return False

        return True

    def scan_directory(
        self, directory: Path, extensions: Optional[List[str]] = None
    ) -> None:
        """Scan a directory for icon usage."""
        if extensions is None:
            extensions = [".cpp", ".h", ".hpp", ".cc", ".cxx"]

        for ext in extensions:
            for file_path in directory.rglob(f"*{ext}"):
                if file_path.is_file():
                    usages = self.scan_file_for_icons(file_path)
                    if usages:
                        rel_path = str(file_path.relative_to(self.project_root))
                        for icon_name, line_num in usages:
                            if icon_name not in self.icon_usage:
                                self.icon_usage[icon_name] = []
                            self.icon_usage[icon_name].append((rel_path, line_num))

    def validate_usage(self) -> bool:
        """Validate that all used icons exist."""
        all_valid = True

        for icon_name, usages in self.icon_usage.items():
            if icon_name not in self.available_icons:
                self.invalid_icons[icon_name] = usages
                all_valid = False

        return all_valid

    def suggest_corrections(self, invalid_icon: str) -> List[str]:
        """Suggest possible corrections for invalid icon names."""
        suggestions = []

        # Look for similar names
        for available in self.available_icons:
            # Exact substring match
            if invalid_icon in available or available in invalid_icon:
                suggestions.append(available)
            # Similar words (split by hyphens)
            elif self._words_similarity(invalid_icon, available) > 0.5:
                suggestions.append(available)

        return suggestions[:5]  # Limit to 5 suggestions

    def _words_similarity(self, name1: str, name2: str) -> float:
        """Calculate similarity between two icon names based on words."""
        words1 = set(name1.split("-"))
        words2 = set(name2.split("-"))

        if not words1 or not words2:
            return 0.0

        intersection = words1.intersection(words2)
        union = words1.union(words2)

        return len(intersection) / len(union)

    def print_report(self) -> None:
        """Print validation report."""
        print("\n" + "=" * 60)
        print("QtLucide Icon Usage Validation Report")
        print("=" * 60)

        print(f"\nAvailable icons: {len(self.available_icons)}")
        print(f"Icons used in code: {len(self.icon_usage)}")
        print(f"Invalid icons found: {len(self.invalid_icons)}")

        if not self.invalid_icons:
            print("\n✅ All icon names are valid!")
            return

        print(f"\n❌ Found {len(self.invalid_icons)} invalid icon names:")

        for invalid_icon, usages in self.invalid_icons.items():
            print(f"\n  '{invalid_icon}' used in:")
            for file_path, line_num in usages:
                print(f"    {file_path}:{line_num}")

            suggestions = self.suggest_corrections(invalid_icon)
            if suggestions:
                print(f"    Suggestions: {', '.join(suggestions)}")

    def run_validation(self) -> bool:
        """Run the complete validation process."""
        print("QtLucide Icon Usage Validator")
        print("-" * 40)

        # Load available icons
        if not self.load_available_icons():
            return False

        # Scan for icon usage
        print("\nScanning for icon usage...")
        examples_dir = self.project_root / "examples"
        tests_dir = self.project_root / "tests"

        if examples_dir.exists():
            self.scan_directory(examples_dir)

        if tests_dir.exists():
            self.scan_directory(tests_dir)

        # Validate usage
        is_valid = self.validate_usage()

        # Print report
        self.print_report()

        return is_valid


def main():
    if len(sys.argv) < 2:
        print("Usage: validate_icon_usage.py <project_root>")
        sys.exit(1)

    project_root = sys.argv[1]
    validator = IconUsageValidator(project_root)

    if validator.run_validation():
        print("\n✅ Validation passed!")
        sys.exit(0)
    else:
        print("\n❌ Validation failed!")
        sys.exit(1)


if __name__ == "__main__":
    main()
