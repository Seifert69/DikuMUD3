#!/usr/bin/env python3
"""
MediaWiki Bulk Upload Script - Alternative Methods

Provides multiple methods to upload .wiki files to MediaWiki when API editing is restricted.
"""

import os
import requests
import time
from pathlib import Path
import urllib.parse
from typing import Optional, Dict, Any
import subprocess
import tempfile

class MediaWikiUploaderAlternative:
    def __init__(self, base_url: str, username: str, password: str):
        """
        Initialize the MediaWiki uploader with alternative methods.
        
        Args:
            base_url: Base URL of the wiki (e.g., "https://wiki.dikumud.net")
            username: Wiki username
            password: Wiki password
        """
        self.base_url = base_url.rstrip('/')
        self.api_url = f"{self.base_url}/api.php"
        self.username = username
        self.password = password
        self.session = requests.Session()
        
    def check_api_permissions(self) -> Dict[str, Any]:
        """Check what API permissions are available."""
        try:
            params = {
                'action': 'query',
                'meta': 'userinfo',
                'uiprop': 'rights|groups',
                'format': 'json'
            }
            
            response = self.session.get(self.api_url, params=params)
            response.raise_for_status()
            data = response.json()
            
            userinfo = data.get('query', {}).get('userinfo', {})
            rights = userinfo.get('rights', [])
            groups = userinfo.get('groups', [])
            
            print(f"User: {userinfo.get('name', 'Unknown')}")
            print(f"Groups: {', '.join(groups)}")
            print(f"Relevant rights:")
            for right in rights:
                if any(keyword in right.lower() for keyword in ['edit', 'api', 'write', 'bot', 'autoconfirmed']):
                    print(f"  - {right}")
            
            return {'rights': rights, 'groups': groups, 'userinfo': userinfo}
            
        except Exception as e:
            print(f"Error checking permissions: {e}")
            return {}
    
    def try_login_with_cookie(self) -> bool:
        """Try alternative login method using cookies."""
        try:
            # First get login token
            login_token_params = {
                'action': 'query',
                'meta': 'tokens',
                'type': 'login',
                'format': 'json'
            }
            
            response = self.session.get(self.api_url, params=login_token_params)
            response.raise_for_status()
            data = response.json()
            
            login_token = data['query']['tokens']['logintoken']
            
            # Try login with cookies
            login_params = {
                'action': 'login',
                'lgname': self.username,
                'lgpassword': self.password,
                'lgtoken': login_token,
                'format': 'json'
            }
            
            response = self.session.post(self.api_url, data=login_params)
            response.raise_for_status()
            data = response.json()
            
            if data.get('login', {}).get('result') == 'Success':
                print("Login successful with cookies")
                return True
            else:
                print(f"Cookie login failed: {data}")
                return False
                
        except Exception as e:
            print(f"Cookie login error: {e}")
            return False
    
    def generate_xml_import_file(self, directory: str = ".") -> str:
        """
        Generate an XML file suitable for MediaWiki's importImages.php or importDump.php.
        
        Returns:
            Path to generated XML file
        """
        wiki_files = list(Path(directory).glob("*.wiki"))
        
        if not wiki_files:
            print("No .wiki files found!")
            return ""
        
        # Create XML content with proper declaration
        xml_content = '''<?xml version="1.0" encoding="UTF-8"?>
<mediawiki xmlns="http://www.mediawiki.org/xml/export-0.10/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.mediawiki.org/xml/export-0.10/ http://www.mediawiki.org/xml/export-0.10.xsd" version="0.10" xml:lang="en">
'''
        
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
                
                # Escape XML special characters properly
                import html
                cleaned_content = html.escape(cleaned_content, quote=False)
                
                # Use plain format without parentheses
                page_title = f"Manual:DIL Manual/{wiki_file.stem}"
                
                # Remove page and revision IDs to let MediaWiki auto-generate them
                xml_content += f'''  <page>
    <title>{page_title}</title>
    <ns>0</ns>
    <revision>
      <timestamp>{time.strftime('%Y-%m-%dT%H:%M:%SZ')}</timestamp>
      <contributor>
        <username>{self.username}</username>
      </contributor>
      <comment>Automated upload of {wiki_file.name}</comment>
      <model>wikitext</model>
      <format>text/x-wiki</format>
      <text xml:space="preserve">{cleaned_content}</text>
    </revision>
  </page>
'''
                
            except Exception as e:
                print(f"Error processing {wiki_file.name}: {e}")
        
        xml_content += '</mediawiki>'
        
        # Write to temporary file
        temp_file = tempfile.NamedTemporaryFile(mode='w', suffix='.xml', delete=False, encoding='utf-8')
        temp_file.write(xml_content)
        temp_file.close()
        
        print(f"Generated XML import file: {temp_file.name}")
        print(f"Contains {len(wiki_files)} pages")
        
        return temp_file.name
    
    def create_manual_upload_script(self, directory: str = ".") -> str:
        """
        Create a bash script for manual upload using curl or browser automation.
        
        Returns:
            Path to generated script
        """
        wiki_files = list(Path(directory).glob("*.wiki"))
        
        script_content = '''#!/bin/bash
# Manual upload script for MediaWiki pages
# This script provides curl commands for manual upload

WIKI_URL="https://wiki.dikumud.net"
USERNAME="''' + self.username + '''"

echo "MediaWiki Manual Upload Script"
echo "=============================="
echo ""
echo "Instructions:"
echo "1. Log into your wiki in a browser"
echo "2. Copy and paste each curl command below"
echo "3. Or use the URLs provided to manually create pages"
echo ""

'''
        
        for wiki_file in wiki_files[:10]:  # Show first 10 as example
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
                
                page_title = f"Manual:DIL_Manual/{wiki_file.stem}"
                edit_url = f"{WIKI_URL}/index.php?title={urllib.parse.quote(page_title)}&action=edit"
                
                script_content += f'''# {wiki_file.name}
echo "Uploading {wiki_file.name}..."
echo "URL: {edit_url}"
echo "Page title: {page_title}"
echo ""
'''
                
            except Exception as e:
                print(f"Error processing {wiki_file.name}: {e}")
        
        if len(wiki_files) > 10:
            script_content += f'''
# ... and {len(wiki_files) - 10} more files
echo "Total files to upload: {len(wiki_files)}"
'''
        
        # Write script file
        script_file = tempfile.NamedTemporaryFile(mode='w', suffix='.sh', delete=False)
        script_file.write(script_content)
        script_file.close()
        
        os.chmod(script_file.name, 0o755)
        
        print(f"Created manual upload script: {script_file.name}")
        return script_file.name

def main():
    """Main function to test alternative upload methods."""
    print("MediaWiki Alternative Upload Methods")
    print("=" * 50)
    
    WIKI_URL = "https://wiki.dikumud.net"
    USERNAME = input("Enter wiki username: ").strip()
    PASSWORD = input("Enter wiki password: ").strip()
    
    uploader = MediaWikiUploaderAlternative(WIKI_URL, USERNAME, PASSWORD)
    
    print("\n1. Checking API permissions...")
    uploader.try_login_with_cookie()
    permissions = uploader.check_api_permissions()
    
    print("\n2. Generating XML import file...")
    xml_file = uploader.generate_xml_import_file()
    
    print("\n3. Creating manual upload script...")
    script_file = uploader.create_manual_upload_script()
    
    print("\n" + "=" * 50)
    print("Alternative Solutions:")
    print("=" * 50)
    print("""
Since API editing is restricted, try these alternatives:

1. XML IMPORT METHOD:
   - Upload the generated XML file to your wiki server
   - Use MediaWiki's importDump.php maintenance script:
     php maintenance/importDump.php <filename>.xml
   
2. MANUAL UPLOAD:
   - Run the generated manual upload script
   - It provides URLs for each page that you can create manually
   
3. SERVER-SIDE SCRIPT:
   - If you have server access, use MediaWiki's maintenance scripts
   - Or create a PHP script that uses MediaWiki's internal functions
   
4. WIKI CONFIGURATION:
   - Check if API editing needs to be enabled in LocalSettings.php
   - Look for $wgEnableAPI = true; and $wgEnableWriteAPI = true;
   - Verify user permissions for API editing

Files created:
- XML import: """ + xml_file + """
- Manual script: """ + script_file + """
""")

if __name__ == "__main__":
    main()