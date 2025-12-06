#!/usr/bin/env python3
"""
Generate XML import file for MediaWiki from .wiki files
"""

import os
import time
from pathlib import Path
import tempfile

def generate_xml_import_file(directory: str = ".", username: str = "Admin") -> str:
    """
    Generate an XML file suitable for MediaWiki's importDump.php.
    
    Args:
        directory: Directory containing .wiki files
        username: Username to attribute edits to
        
    Returns:
        Path to generated XML file
    """
    wiki_files = list(Path(directory).glob("*.wiki"))
    
    if not wiki_files:
        print("No .wiki files found!")
        return ""
    
    print(f"Processing {len(wiki_files)} .wiki files...")
    
    # Create XML content
    xml_content = '''<mediawiki xmlns="http://www.mediawiki.org/xml/export-0.10/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.mediawiki.org/xml/export-0.10/ http://www.mediawiki.org/xml/export-0.10.xsd" version="0.10" xml:lang="en">
'''
    
    processed = 0
    for wiki_file in wiki_files:
        try:
            with open(wiki_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Remove line numbers
            lines = content.split('\n')
            cleaned_lines = []
            for line in lines:
                if '|' in line and line[:6].isdigit():
                    cleaned_lines.append(line.split('|', 1)[1] if '|' in line else line)
                else:
                    cleaned_lines.append(line)
            cleaned_content = '\n'.join(cleaned_lines)
            
            # Escape XML special characters
            cleaned_content = cleaned_content.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')
            
            page_title = f"Manual:DIL_Manual/{wiki_file.stem}"
            
            xml_content += f'''  <page>
    <title>{page_title}</title>
    <ns>0</ns>
    <id>{abs(hash(page_title)) % 1000000}</id>
    <revision>
      <id>{abs(hash(page_title + 'rev')) % 1000000}</id>
      <timestamp>{time.strftime('%Y-%m-%dT%H:%M:%SZ')}</timestamp>
      <contributor>
        <username>{username}</username>
        <id>1</id>
      </contributor>
      <comment>Automated upload of {wiki_file.name}</comment>
      <model>wikitext</model>
      <format>text/x-wiki</format>
      <text xml:space="preserve">{cleaned_content}</text>
    </revision>
  </page>
'''
            processed += 1
            if processed % 50 == 0:
                print(f"Processed {processed}/{len(wiki_files)} files...")
                
        except Exception as e:
            print(f"Error processing {wiki_file.name}: {e}")
    
    xml_content += '</mediawiki>'
    
    # Write to file
    output_file = "dil_manual_import.xml"
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(xml_content)
    
    print(f"Generated XML import file: {output_file}")
    print(f"Contains {processed} pages")
    
    return output_file

def create_upload_instructions():
    """Create instructions for uploading the XML file."""
    instructions = """
MediaWiki XML Import Instructions
==================================

You now have a file called 'dil_manual_import.xml' that contains all your .wiki files
in MediaWiki's XML import format. Here are several ways to upload it:

METHOD 1: Using MediaWiki's importDump.php (Recommended)
-------------------------------------------------------
If you have server access to your MediaWiki installation:

1. Upload the XML file to your MediaWiki server (e.g., via scp/ftp)
2. SSH into your server
3. Navigate to your MediaWiki directory
4. Run: php maintenance/importDump.php dil_manual_import.xml
5. Run: php maintenance/rebuildrecentchanges.php (to update recent changes)

METHOD 2: Using Special:Import in the wiki interface
----------------------------------------------------
1. Log in as an admin user on your wiki
2. Go to Special:Import (https://wiki.dikumud.net/wiki/Special:Import)
3. Upload the XML file through the web interface
4. Note: This may require admin privileges and file upload permissions

METHOD 3: Enable API editing
---------------------------
If you want to use the original API uploader, you may need to edit LocalSettings.php:

1. Add or uncomment these lines:
   $wgEnableAPI = true;
   $wgEnableWriteAPI = true;
   
2. Ensure your user has API editing permissions:
   $wgGroupPermissions['user']['writeapi'] = true;
   $wgGroupPermissions['bot']['writeapi'] = true;

3. Restart your web server

METHOD 4: Manual page creation
------------------------------
As a last resort, you can create pages manually:
1. For each .wiki file, go to: https://wiki.dikumud.net/wiki/Manual:DIL_Manual/filename
2. Click "Create" or "Edit"
3. Copy and paste the content from the .wiki file (after removing line numbers)

FILE FORMAT NOTES
-----------------
- Line numbers (like "00001| ") have been automatically removed
- Pages are created under "Manual:DIL_Manual/" namespace
- All content is properly escaped for XML
- Each page includes attribution and timestamps

NEXT STEPS
----------
1. Try Method 1 first if you have server access
2. If not, try Method 2 through the web interface
3. For future uploads, consider enabling API editing (Method 3)
"""
    
    with open("UPLOAD_INSTRUCTIONS.txt", 'w') as f:
        f.write(instructions)
    
    print("Created UPLOAD_INSTRUCTIONS.txt with detailed instructions")

def main():
    """Generate XML import file and instructions."""
    print("MediaWiki XML Import Generator")
    print("=" * 40)
    
    # Generate XML file
    xml_file = generate_xml_import_file()
    
    # Create instructions
    create_upload_instructions()
    
    print("\n" + "=" * 40)
    print("Files created:")
    print(f"- {xml_file} (XML import file)")
    print("- UPLOAD_INSTRUCTIONS.txt (detailed instructions)")
    print("\nSee UPLOAD_INSTRUCTIONS.txt for next steps!")

if __name__ == "__main__":
    main()