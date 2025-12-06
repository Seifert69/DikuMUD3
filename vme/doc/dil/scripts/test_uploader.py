#!/usr/bin/env python3
"""
Test script for MediaWiki uploader - dry run mode
"""

import os
from pathlib import Path

def test_wiki_files():
    """Test reading and processing .wiki files without uploading."""
    print("Testing .wiki file processing...")
    print("=" * 50)
    
    wiki_files = list(Path(".").glob("*.wiki"))
    
    if not wiki_files:
        print("No .wiki files found!")
        return
    
    print(f"Found {len(wiki_files)} .wiki files:\n")
    
    for i, wiki_file in enumerate(wiki_files[:5], 1):  # Show first 5 files
        print(f"[{i}] {wiki_file.name}")
        
        try:
            with open(wiki_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Remove line numbers if present
            lines = content.split('\n')
            cleaned_lines = []
            for line in lines:
                if '|' in line and line[:6].isdigit():
                    cleaned_lines.append(line.split('|', 1)[1] if '|' in line else line)
                else:
                    cleaned_lines.append(line)
            
            cleaned_content = '\n'.join(cleaned_lines)
            
            # Show first few lines
            preview = cleaned_content[:200].replace('\n', ' ')
            print(f"    Preview: {preview}...")
            print(f"    Length: {len(cleaned_content)} characters")
            print(f"    Target page: Manual:DIL_Manual/{wiki_file.stem}")
            print()
            
        except Exception as e:
            print(f"    Error reading file: {e}")
            print()
    
    if len(wiki_files) > 5:
        print(f"... and {len(wiki_files) - 5} more files")
    
    print(f"\nTotal files ready for upload: {len(wiki_files)}")

def test_url_construction():
    """Test URL construction for sample files."""
    print("\nTesting URL construction:")
    print("=" * 30)
    
    sample_files = ["string.wiki", "abilities.wiki", "act.wiki"]
    base_url = "https://wiki.dikumud.net"
    
    for filename in sample_files:
        page_title = f"Manual:DIL_Manual/{Path(filename).stem}"
        wiki_url = f"{base_url}/wiki/{page_title.replace(' ', '_')}"
        print(f"{filename} -> {wiki_url}")

if __name__ == "__main__":
    test_wiki_files()
    test_url_construction()