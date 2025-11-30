#!/usr/bin/env python3
"""
Process Lucide icons from the cloned repository.
Extracts SVG files, processes metadata, and prepares for embedding.
"""

import json
import re
from pathlib import Path


class LucideIconProcessor:
    def __init__(self, source_dir: str, output_dir: str):
        self.source_dir = Path(source_dir)
        self.output_dir = Path(output_dir)
        self.icons_data = {}

    def process_icons(self):
        """Main processing function"""
        print("Processing Lucide icons...")

        # Create output directories
        self.output_dir.mkdir(parents=True, exist_ok=True)
        (self.output_dir / "svg").mkdir(exist_ok=True)
        (self.output_dir / "metadata").mkdir(exist_ok=True)

        # Process all icons
        icon_files = list(self.source_dir.glob("*.svg"))
        print(f"Found {len(icon_files)} SVG files")

        processed_count = 0
        for svg_file in icon_files:
            if self.process_single_icon(svg_file):
                processed_count += 1

        print(f"Successfully processed {processed_count} icons")

        # Generate metadata files
        self.generate_metadata()

    def process_single_icon(self, svg_file: Path) -> bool:
        """Process a single icon file"""
        try:
            icon_name = svg_file.stem
            json_file = svg_file.with_suffix(".json")

            # Read SVG content
            svg_content = svg_file.read_text(encoding="utf-8")

            # Read metadata if available
            metadata = {}
            if json_file.exists():
                metadata = json.loads(json_file.read_text(encoding="utf-8"))

            # Optimize SVG content
            optimized_svg = self.optimize_svg(svg_content)

            # Save optimized SVG
            output_svg = self.output_dir / "svg" / f"{icon_name}.svg"
            output_svg.write_text(optimized_svg, encoding="utf-8")

            # Store icon data
            self.icons_data[icon_name] = {
                "name": icon_name,
                "svg_file": f"svg/{icon_name}.svg",
                "tags": metadata.get("tags", []),
                "categories": metadata.get("categories", []),
                "contributors": metadata.get("contributors", []),
            }

            return True

        except Exception as e:
            print(f"Error processing {svg_file}: {e}")
            return False

    def optimize_svg(self, svg_content: str) -> str:
        """Optimize SVG content for embedding"""
        # Remove XML declaration if present
        svg_content = re.sub(r"<\?xml[^>]*\?>", "", svg_content)

        # Remove comments
        svg_content = re.sub(r"<!--.*?-->", "", svg_content, flags=re.DOTALL)

        # Remove unnecessary whitespace
        svg_content = re.sub(r"\s+", " ", svg_content)
        svg_content = svg_content.strip()

        # Ensure consistent attributes
        if "xmlns=" not in svg_content:
            svg_content = svg_content.replace(
                "<svg", '<svg xmlns="http://www.w3.org/2000/svg"'
            )

        return svg_content

    def generate_metadata(self):
        """Generate metadata files for code generation"""
        # Generate icon list
        icon_list = {
            "icons": self.icons_data,
            "count": len(self.icons_data),
            "version": "1.0.0",
        }

        metadata_file = self.output_dir / "metadata" / "icons.json"
        metadata_file.write_text(json.dumps(icon_list, indent=2), encoding="utf-8")

        # Generate categories mapping
        categories = {}
        for icon_name, icon_data in self.icons_data.items():
            for category in icon_data.get("categories", []):
                if category not in categories:
                    categories[category] = []
                categories[category].append(icon_name)

        categories_file = self.output_dir / "metadata" / "categories.json"
        categories_file.write_text(json.dumps(categories, indent=2), encoding="utf-8")

        # Generate tags mapping
        tags = {}
        for icon_name, icon_data in self.icons_data.items():
            for tag in icon_data.get("tags", []):
                if tag not in tags:
                    tags[tag] = []
                tags[tag].append(icon_name)

        tags_file = self.output_dir / "metadata" / "tags.json"
        tags_file.write_text(json.dumps(tags, indent=2), encoding="utf-8")

        print(f"Generated metadata for {len(self.icons_data)} icons")
        print(f"Categories: {len(categories)}")
        print(f"Tags: {len(tags)}")


def main():
    import sys

    if len(sys.argv) != 3:
        print("Usage: python process_lucide_icons.py <source_dir> <output_dir>")
        sys.exit(1)

    source_dir = sys.argv[1]
    output_dir = sys.argv[2]

    processor = LucideIconProcessor(source_dir, output_dir)
    processor.process_icons()


if __name__ == "__main__":
    main()
