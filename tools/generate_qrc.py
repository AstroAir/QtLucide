#!/usr/bin/env python3
"""
Generate Qt resource file (.qrc) for embedding Lucide SVG icons.
"""

import json
from pathlib import Path
from typing import List

class QrcGenerator:
    def __init__(self, metadata_dir: str, svg_dir: str, output_file: str):
        self.metadata_dir = Path(metadata_dir)
        self.svg_dir = Path(svg_dir)
        self.output_file = Path(output_file)
        self.icons_data = {}
        
    def load_metadata(self):
        """Load icon metadata"""
        icons_file = self.metadata_dir / "icons.json"
        with open(icons_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
            self.icons_data = data['icons']
    
    def generate_qrc(self):
        """Generate the .qrc file"""
        self.output_file.parent.mkdir(parents=True, exist_ok=True)
        
        qrc_content = self._get_qrc_template()
        
        # Generate file entries
        file_entries = []
        for icon_name in sorted(self.icons_data.keys()):
            svg_file = f"svg/{icon_name}.svg"
            file_entries.append(f'        <file alias="{icon_name}">{svg_file}</file>')
        
        files_content = "\n".join(file_entries)
        qrc_content = qrc_content.replace("{{FILE_ENTRIES}}", files_content)
        
        self.output_file.write_text(qrc_content, encoding='utf-8')
        print(f"Generated {self.output_file} with {len(self.icons_data)} icon files")
    
    def _get_qrc_template(self) -> str:
        return '''<RCC>
    <qresource prefix="/lucide">
{{FILE_ENTRIES}}
    </qresource>
</RCC>'''

def main():
    import sys
    
    if len(sys.argv) != 4:
        print("Usage: python generate_qrc.py <metadata_dir> <svg_dir> <output_qrc>")
        sys.exit(1)
    
    metadata_dir = sys.argv[1]
    svg_dir = sys.argv[2]
    output_file = sys.argv[3]
    
    generator = QrcGenerator(metadata_dir, svg_dir, output_file)
    generator.load_metadata()
    generator.generate_qrc()

if __name__ == "__main__":
    main()
