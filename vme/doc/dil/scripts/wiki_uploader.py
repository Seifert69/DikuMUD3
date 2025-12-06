#!/usr/bin/env python3
"""
MediaWiki Bulk Upload Script

Uploads all .wiki files from the current directory to a MediaWiki site.
Each file is uploaded to Manual:DIL_Manual/{filename} format.
"""

import os
import requests
import time
from pathlib import Path
import urllib.parse
from typing import Optional, Dict, Any

class MediaWikiUploader:
    def __init__(self, base_url: str, username: str, password: str):
        """
        Initialize the MediaWiki uploader.
        
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
        self.csrf_token = None
        
    def login(self) -> bool:
        """Log in to the wiki and get CSRF token."""
        try:
            # Get login token
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
            
            # Perform login
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
                print(f"Successfully logged in as {self.username}")
                return self.get_csrf_token()
            else:
                print(f"Login failed: {data}")
                return False
                
        except Exception as e:
            print(f"Login error: {e}")
            return False
    
    def get_csrf_token(self) -> bool:
        """Get CSRF token for editing."""
        try:
            params = {
                'action': 'query',
                'meta': 'tokens',
                'format': 'json'
            }
            
            response = self.session.get(self.api_url, params=params)
            response.raise_for_status()
            data = response.json()
            
            self.csrf_token = data['query']['tokens']['csrftoken']
            return True
            
        except Exception as e:
            print(f"Error getting CSRF token: {e}")
            return False
    
    def upload_page(self, filename: str, content: str) -> bool:
        """
        Upload a single page to the wiki.
        
        Args:
            filename: Name of the .wiki file (without extension)
            content: Content to upload
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Create page title in Manual:DIL_Manual/{filename} format
            page_title = f"Manual:DIL_Manual/{filename}"
            
            # Prepare edit parameters
            edit_params = {
                'action': 'edit',
                'title': page_title,
                'text': content,
                'summary': f'Automated upload of {filename}.wiki',
                'token': self.csrf_token,
                'format': 'json',
                'bot': True,  # Mark as bot edit
                'nocreate': False  # Allow page creation
            }
            
            response = self.session.post(self.api_url, data=edit_params)
            response.raise_for_status()
            data = response.json()
            
            if 'edit' in data and data['edit'].get('result') == 'Success':
                page_url = f"{self.base_url}/wiki/{urllib.parse.quote(page_title)}"
                print(f"✓ Uploaded: {filename} -> {page_url}")
                return True
            else:
                print(f"✗ Failed to upload {filename}: {data}")
                return False
                
        except Exception as e:
            print(f"✗ Error uploading {filename}: {e}")
            return False
    
    def upload_all_wiki_files(self, directory: str = ".") -> Dict[str, int]:
        """
        Upload all .wiki files from the specified directory.
        
        Args:
            directory: Directory containing .wiki files (default: current directory)
            
        Returns:
            Dictionary with upload statistics
        """
        wiki_files = list(Path(directory).glob("*.wiki"))
        
        if not wiki_files:
            print("No .wiki files found in the directory.")
            return {'total': 0, 'success': 0, 'failed': 0}
        
        print(f"Found {len(wiki_files)} .wiki files to upload...")
        
        stats = {'total': len(wiki_files), 'success': 0, 'failed': 0}
        
        for i, wiki_file in enumerate(wiki_files, 1):
            print(f"\n[{i}/{len(wiki_files)}] Processing {wiki_file.name}...")
            
            try:
                # Read file content
                with open(wiki_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # Remove line numbers if present (format: "00001| ")
                lines = content.split('\n')
                cleaned_lines = []
                for line in lines:
                    if '|' in line and line[:6].isdigit():
                        # Remove line number prefix
                        cleaned_lines.append(line.split('|', 1)[1] if '|' in line else line)
                    else:
                        cleaned_lines.append(line)
                cleaned_content = '\n'.join(cleaned_lines)
                
                # Upload the page
                if self.upload_page(wiki_file.stem, cleaned_content):
                    stats['success'] += 1
                else:
                    stats['failed'] += 1
                    
                # Add small delay to avoid overwhelming the server
                time.sleep(0.5)
                
            except Exception as e:
                print(f"✗ Error processing {wiki_file.name}: {e}")
                stats['failed'] += 1
        
        return stats

def main():
    """Main function to run the uploader."""
    print("MediaWiki Bulk Upload Script")
    print("=" * 40)
    
    # Configuration - UPDATE THESE VALUES
    WIKI_URL = "https://wiki.dikumud.net"
    USERNAME = input("Enter wiki username: ").strip()
    PASSWORD = input("Enter wiki password: ").strip()
    
    # Create uploader instance
    uploader = MediaWikiUploader(WIKI_URL, USERNAME, PASSWORD)
    
    # Login
    if not uploader.login():
        print("Failed to login. Exiting.")
        return
    
    # Upload all files
    print("\nStarting upload...")
    stats = uploader.upload_all_wiki_files()
    
    # Print summary
    print("\n" + "=" * 40)
    print("Upload Summary:")
    print(f"Total files: {stats['total']}")
    print(f"Successful: {stats['success']}")
    print(f"Failed: {stats['failed']}")
    
    if stats['failed'] == 0:
        print("\n🎉 All files uploaded successfully!")
    else:
        print(f"\n⚠️  {stats['failed']} files failed to upload.")

if __name__ == "__main__":
    main()