#!/usr/bin/env python3
"""
Generate C++ header files from Lucide icon metadata.
Creates enum definitions and string mappings for QtLucide.
"""

import json
import re
from pathlib import Path


class QtLucideHeaderGenerator:
    def __init__(self, metadata_dir: str, output_dir: str):
        self.metadata_dir = Path(metadata_dir)
        self.output_dir = Path(output_dir)
        self.icons_data = {}

    def load_metadata(self):
        """Load icon metadata from JSON files"""
        icons_file = self.metadata_dir / "icons.json"
        with open(icons_file, "r", encoding="utf-8") as f:
            data = json.load(f)
            self.icons_data = data["icons"]

        print(f"Loaded metadata for {len(self.icons_data)} icons")

    def generate_headers(self):
        """Generate all header files"""
        self.output_dir.mkdir(parents=True, exist_ok=True)

        self.generate_enums_header()
        self.generate_strings_header()

        print("Header generation complete")

    def generate_enums_header(self):
        """Generate QtLucideEnums.h with icon enum definitions"""
        header_content = self._get_enum_header_template()

        # Generate enum values
        enum_values = []
        for i, icon_name in enumerate(sorted(self.icons_data.keys())):
            enum_name = self._name_to_enum(icon_name)
            enum_values.append(f"    {enum_name} = {i}")

        enum_content = ",\n".join(enum_values)
        header_content = header_content.replace("{{ENUM_VALUES}}", enum_content)
        header_content = header_content.replace(
            "{{ICON_COUNT}}", str(len(self.icons_data))
        )

        output_file = self.output_dir / "QtLucideEnums.h"
        output_file.write_text(header_content, encoding="utf-8")

        print(f"Generated {output_file} with {len(self.icons_data)} enum values")

    def generate_strings_header(self):
        """Generate QtLucideStrings.h with string mappings"""
        header_content = self._get_strings_header_template()

        # Generate string array
        string_entries = []
        for icon_name in sorted(self.icons_data.keys()):
            enum_name = self._name_to_enum(icon_name)
            string_entries.append(f'    {{Icons::{enum_name}, "{icon_name}"}}')

        strings_content = ",\n".join(string_entries)
        header_content = header_content.replace("{{STRING_MAPPINGS}}", strings_content)

        output_file = self.output_dir / "QtLucideStrings.h"
        output_file.write_text(header_content, encoding="utf-8")

        print(f"Generated {output_file} with {len(self.icons_data)} string mappings")

    def _name_to_enum(self, name: str) -> str:
        """Convert icon name to enum constant name"""
        # Replace hyphens with underscores and ensure valid C++ identifier
        enum_name = re.sub(r"[^a-zA-Z0-9_]", "_", name)
        enum_name = re.sub(
            r"^(\d)", r"Icon_\1", enum_name
        )  # Prefix if starts with digit

        # Handle C++ reserved keywords
        cpp_keywords = {
            "delete",
            "new",
            "class",
            "struct",
            "enum",
            "union",
            "typedef",
            "static",
            "const",
            "volatile",
            "inline",
            "virtual",
            "explicit",
            "operator",
            "template",
            "typename",
            "namespace",
            "using",
            "public",
            "private",
            "protected",
            "friend",
            "extern",
            "register",
            "auto",
            "void",
            "char",
            "short",
            "int",
            "long",
            "float",
            "double",
            "signed",
            "unsigned",
            "bool",
            "true",
            "false",
            "if",
            "else",
            "for",
            "while",
            "do",
            "switch",
            "case",
            "default",
            "break",
            "continue",
            "return",
            "goto",
            "try",
            "catch",
            "throw",
            "sizeof",
        }

        if enum_name in cpp_keywords:
            enum_name = f"icon_{enum_name}"

        return enum_name

    def _get_enum_header_template(self) -> str:
        return """/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * Generated from Lucide icon metadata
 *
 * MIT Licensed
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDEENUMS_H
#define QTLUCIDEENUMS_H

namespace lucide {

/**
 * @brief Enumeration of all available Lucide icons
 */
enum class Icons {
{{ENUM_VALUES}}
};

/**
 * @brief Total number of available icons
 */
constexpr int ICON_COUNT = {{ICON_COUNT}};

} // namespace lucide

#endif // QTLUCIDEENUMS_H"""

    def _get_strings_header_template(self) -> str:
        return """/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * AUTO-GENERATED FILE - DO NOT EDIT MANUALLY
 * Generated from Lucide icon metadata
 *
 * MIT Licensed
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDESTRINGS_H
#define QTLUCIDESTRINGS_H

#include "QtLucideEnums.h"
#include <QHash>
#include <QString>

namespace lucide {

/**
 * @brief Mapping from icon enum to string name
 */
static const QHash<Icons, QString> ICON_TO_STRING_MAP = {
{{STRING_MAPPINGS}}
};

/**
 * @brief Mapping from string name to icon enum
 */
static QHash<QString, Icons> createStringToIconMap() {
    QHash<QString, Icons> map;
    for (auto it = ICON_TO_STRING_MAP.begin(); it != ICON_TO_STRING_MAP.end(); ++it) {
        map[it.value()] = it.key();
    }
    return map;
}

static const QHash<QString, Icons> STRING_TO_ICON_MAP = createStringToIconMap();

} // namespace lucide

#endif // QTLUCIDESTRINGS_H"""


def main():
    import sys

    if len(sys.argv) != 3:
        print("Usage: python generate_headers.py <metadata_dir> <output_dir>")
        sys.exit(1)

    metadata_dir = sys.argv[1]
    output_dir = sys.argv[2]

    generator = QtLucideHeaderGenerator(metadata_dir, output_dir)
    generator.load_metadata()
    generator.generate_headers()


if __name__ == "__main__":
    main()
