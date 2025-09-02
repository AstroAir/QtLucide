#!/usr/bin/env python3
"""
QtLucide Resource Builder
Automatically builds all resources needed for QtLucide library:
- Scans SVG files
- Generates metadata
- Creates QRC files
- Generates C++ headers
"""

import os
import sys
import json
import subprocess
from pathlib import Path
from typing import List, Dict

class QtLucideResourceBuilder:
    def __init__(self, project_root: str):
        self.project_root = Path(project_root)
        self.tools_dir = self.project_root / "tools"
        self.resources_dir = self.project_root / "resources" / "icons"
        self.svg_dir = self.resources_dir / "svg"
        self.metadata_dir = self.resources_dir / "metadata"
        self.include_dir = self.project_root / "include" / "QtLucide"
        
        # Ensure directories exist
        self.metadata_dir.mkdir(parents=True, exist_ok=True)
        self.include_dir.mkdir(parents=True, exist_ok=True)
    
    def scan_svg_files(self) -> List[str]:
        """Scan for all SVG files in the svg directory"""
        if not self.svg_dir.exists():
            print(f"Warning: SVG directory {self.svg_dir} does not exist")
            return []
        
        svg_files = list(self.svg_dir.glob("*.svg"))
        print(f"Found {len(svg_files)} SVG files")
        return [f.stem for f in svg_files]
    
    def generate_metadata_from_svg(self):
        """Generate metadata from existing SVG files"""
        svg_names = self.scan_svg_files()
        
        # Create basic metadata structure
        icons_data = {}
        for svg_name in svg_names:
            icons_data[svg_name] = {
                'name': svg_name,
                'svg_file': f"svg/{svg_name}.svg",
                'tags': self._generate_tags_from_name(svg_name),
                'categories': self._categorize_icon(svg_name),
                'contributors': []
            }
        
        # Save icons metadata
        icons_metadata = {
            'icons': icons_data,
            'count': len(icons_data),
            'version': '1.0.0'
        }
        
        icons_file = self.metadata_dir / "icons.json"
        with open(icons_file, 'w', encoding='utf-8') as f:
            json.dump(icons_metadata, f, indent=2)
        
        # Generate categories and tags files
        self._generate_categories_and_tags(icons_data)
        
        print(f"Generated metadata for {len(icons_data)} icons")
        return icons_data
    
    def _generate_tags_from_name(self, name: str) -> List[str]:
        """Generate basic tags from icon name"""
        tags = []
        parts = name.replace('-', ' ').split()
        
        # Add parts as tags
        tags.extend(parts)
        
        # Add some common categorizations
        if any(word in name for word in ['arrow', 'chevron']):
            tags.append('navigation')
        if any(word in name for word in ['file', 'folder', 'document']):
            tags.append('files')
        if any(word in name for word in ['user', 'person', 'people']):
            tags.append('users')
        if any(word in name for word in ['heart', 'star', 'like']):
            tags.append('social')
        
        return list(set(tags))  # Remove duplicates
    
    def _categorize_icon(self, name: str) -> List[str]:
        """Categorize icon based on name"""
        categories = []
        
        # Define category mappings
        category_keywords = {
            'navigation': ['arrow', 'chevron', 'menu', 'home', 'back', 'forward'],
            'files': ['file', 'folder', 'document', 'save', 'download', 'upload'],
            'communication': ['mail', 'message', 'phone', 'chat', 'send'],
            'media': ['play', 'pause', 'stop', 'music', 'video', 'camera'],
            'social': ['heart', 'star', 'like', 'share', 'user', 'users'],
            'system': ['settings', 'cog', 'gear', 'power', 'battery', 'wifi'],
            'editing': ['edit', 'pen', 'pencil', 'brush', 'cut', 'copy', 'paste'],
            'business': ['briefcase', 'chart', 'graph', 'money', 'bank', 'credit']
        }
        
        for category, keywords in category_keywords.items():
            if any(keyword in name for keyword in keywords):
                categories.append(category)
        
        if not categories:
            categories.append('general')
        
        return categories
    
    def _generate_categories_and_tags(self, icons_data: Dict):
        """Generate categories.json and tags.json files"""
        categories = {}
        tags = {}
        
        for icon_name, icon_data in icons_data.items():
            # Process categories
            for category in icon_data.get('categories', []):
                if category not in categories:
                    categories[category] = []
                categories[category].append(icon_name)
            
            # Process tags
            for tag in icon_data.get('tags', []):
                if tag not in tags:
                    tags[tag] = []
                tags[tag].append(icon_name)
        
        # Save categories
        categories_file = self.metadata_dir / "categories.json"
        with open(categories_file, 'w', encoding='utf-8') as f:
            json.dump(categories, f, indent=2)
        
        # Save tags
        tags_file = self.metadata_dir / "tags.json"
        with open(tags_file, 'w', encoding='utf-8') as f:
            json.dump(tags, f, indent=2)
    
    def generate_qrc_file(self):
        """Generate Qt resource file"""
        print("Generating QRC file...")
        
        qrc_script = self.tools_dir / "generate_qrc.py"
        qrc_output = self.resources_dir / "lucide_icons.qrc"
        
        cmd = [
            sys.executable, str(qrc_script),
            str(self.metadata_dir),
            str(self.svg_dir),
            str(qrc_output)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error generating QRC: {result.stderr}")
            return False
        
        print("QRC file generated successfully")
        return True
    
    def generate_headers(self):
        """Generate C++ header files"""
        print("Generating C++ headers...")
        
        headers_script = self.tools_dir / "generate_headers.py"
        
        cmd = [
            sys.executable, str(headers_script),
            str(self.metadata_dir),
            str(self.include_dir)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"Error generating headers: {result.stderr}")
            return False
        
        print("C++ headers generated successfully")
        return True
    
    def build_all(self):
        """Build all resources"""
        print("=== QtLucide Resource Builder ===")
        print(f"Project root: {self.project_root}")
        print(f"SVG directory: {self.svg_dir}")
        print(f"Found {len(list(self.svg_dir.glob('*.svg')))} SVG files")
        
        # Step 1: Generate metadata from SVG files
        print("\n1. Generating metadata...")
        self.generate_metadata_from_svg()
        
        # Step 2: Generate QRC file
        print("\n2. Generating QRC file...")
        if not self.generate_qrc_file():
            return False
        
        # Step 3: Generate C++ headers
        print("\n3. Generating C++ headers...")
        if not self.generate_headers():
            return False
        
        print("\n=== Build Complete ===")
        print("All resources have been generated successfully!")
        print("The library will now include all SVG files when compiled.")
        
        return True

def main():
    if len(sys.argv) > 1:
        project_root = sys.argv[1]
    else:
        # Assume script is run from tools directory
        project_root = Path(__file__).parent.parent
    
    builder = QtLucideResourceBuilder(project_root)
    
    if not builder.build_all():
        sys.exit(1)

if __name__ == "__main__":
    main()
