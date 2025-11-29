#!/usr/bin/env python3
"""
Script to convert DIL prompt txt files to MCP JSON format.
Processes all *_prompt.txt files in the current directory and generates corresponding .json files.
"""

import os
import re
import json
import textwrap

def extract_sections_from_txt(txt_content):
    """
    Extract the three sections from a prompt txt file:
    - Yacc rule
    - C implementation  
    - DIL example
    """
    sections = {}
    
    # Split by the section headers
    parts = re.split(r'=== (Yacc rule|C implementation|DIL example) ===', txt_content)
    
    if len(parts) >= 7:  # Should have: intro, Yacc header, yacc content, C header, C content, DIL header, DIL content
        sections['yacc_rule'] = parts[2].strip()
        sections['c_implementation'] = parts[4].strip()
        sections['dil_example'] = parts[6].strip()
    
    return sections

def extract_opcode_from_yacc(yacc_rule):
    """
    Extract the opcode from the yacc rule, e.g., DILSF_ABL from the rule
    """
    # Look for patterns like DILSF_ABL, DILSI_ACT, etc.
    match = re.search(r'\b(DIL[SEIF][A-Z_]+)\b', yacc_rule)
    if match:
        return match.group(1)
    return None

def extract_dilfe_name_from_c(c_implementation):
    """
    Extract the dilfe function name from C implementation, or derive from case statement
    """
    # Look for case DILF_XXX: pattern
    case_match = re.search(r'case\s+(DILF_[A-Z_]+):', c_implementation)
    if case_match:
        case_label = case_match.group(1)
        # Convert to function name format: DILF_XXX -> dilf_xxx
        func_name = case_label.lower()
        return func_name
    
    # Look for void dilfe_name(dilprg *p) pattern
    func_match = re.search(r'void\s+(\w+)\s*\(dilprg\s*\*p\)', c_implementation)
    if func_match:
        return func_match.group(1)
    
    return None

def process_txt_file(txt_path):
    """
    Process a single txt file and return MCP data
    """
    with open(txt_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    # Extract keyword from filename
    filename = os.path.basename(txt_path)
    keyword = filename.replace('_prompt.txt', '')
    
    # Extract sections
    sections = extract_sections_from_txt(content)
    
    if not sections:
        print(f"Warning: Could not extract sections from {filename}")
        return None
    
    # Extract opcode from yacc rule
    opcode = extract_opcode_from_yacc(sections['yacc_rule'])
    
    # Extract dilfe name from C implementation
    dilfe_name = extract_dilfe_name_from_c(sections['c_implementation'])
    
    # Build MCP data structure
    mcp_data = {
        "keyword": keyword,
        "opcode": opcode,
        "yacc_rule": sections['yacc_rule'],
        "dilfe_name": dilfe_name,
        "c_implementation": sections['c_implementation'],
        "dil_example": sections['dil_example']
    }
    
    return mcp_data

def main():
    """
    Process all *_prompt.txt files in the current directory
    """
    current_dir = '.'
    
    # Find all txt files ending with _prompt.txt
    txt_files = [f for f in os.listdir(current_dir) if f.endswith('_prompt.txt')]
    
    if not txt_files:
        print("No *_prompt.txt files found in current directory")
        return
    
    print(f"Found {len(txt_files)} prompt files to process")
    
    processed_count = 0
    
    for txt_file in txt_files:
        txt_path = os.path.join(current_dir, txt_file)
        
        try:
            mcp_data = process_txt_file(txt_path)
            
            if mcp_data:
                # Generate JSON filename
                json_filename = txt_file.replace('_prompt.txt', '.json')
                json_path = os.path.join(current_dir, json_filename)
                
                # Write JSON file
                with open(json_path, 'w', encoding='utf-8') as f:
                    json.dump(mcp_data, f, indent=4, ensure_ascii=False)
                
                print(f"Generated {json_filename} from {txt_file}")
                processed_count += 1
            else:
                print(f"Failed to process {txt_file}")
                
        except Exception as e:
            print(f"Error processing {txt_file}: {e}")
    
    print(f"\nProcessing complete. Generated {processed_count} MCP JSON files.")

if __name__ == "__main__":
    main()