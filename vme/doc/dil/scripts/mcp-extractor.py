# !/usr/bin/env python3
# Script to extract DIL keyword information from source files to feed to an AI model.
# So that we can generate an MCP that let's AI understand DIL keywords, their implementations, and usage examples.
# And perhaps also generate new up-to-date DIL documentation for humans.
# 
import re
import os
import sys
import subprocess
import json
import textwrap

def extract_keywords_from_diltok_h(diltok_h_path):
    """
    Step 1: Scan diltok.h and extract all entries from dilsym[] struct.
    Returns a list of tuples: (keyword, opcode)
    Fails if file not found or no matches.
    """
    if not os.path.exists(diltok_h_path):
        raise FileNotFoundError(f"diltok.h not found at {diltok_h_path}")
    
    with open(diltok_h_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    
    pattern = r'{"([^"]+)",\s*(DIL[SEIF][A-Z_]+)}'
    matches = re.findall(pattern, content)
    
    ignored_keywords = {
        "dilbegin", "var", "code", "external", "dilend", "recall",
        "aware", "unique", "fnpri", "global", "and", "or", "not", "skip",
        "while", "else", "goto", "break", "continue", "for", "dex_red",
        "unitptr", "integer", "extraptr","string", "stringlist", "intlist",
        "zoneptr", "cmdptr", "in"

    }
    matches = [(kw, op) for kw, op in matches if kw not in ignored_keywords]
    
    if not matches:
        raise ValueError("No dilsym[] entries found in diltok.h after filtering")
    
    return matches  # List of (keyword, opcode)


def extract_yacc_rule(dilpar_y_path, opcode):
    """
    Step 2: Scan dilpar.y for the given opcode and extract the rule block.
    Returns the full yacc rule string.
    Fails if file not found or no matching rule.
    """
    if not os.path.exists(dilpar_y_path):
        raise FileNotFoundError(f"dilpar.y not found at {dilpar_y_path}")
    with open(dilpar_y_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    # Match | OPCODE ... {
    # Match either 'nonterminal : OPCODE ... {' or '| OPCODE ... {', anchored to line starts
    header_pattern = rf'^\s*(?:\w+\s*:\s*|\s*\|\s*){opcode}(?![A-Z0-9_])\s*[^{{]*{{'
    matches = list(re.finditer(header_pattern, content, re.MULTILINE | re.DOTALL))
    if not matches:
        raise ValueError(f"No yacc rule found for opcode {opcode} in dilpar.y")

    rule_codes = []
    for match in matches:
        func_start = match.start()
        start_pos = match.end()
        level = 1
        i = start_pos
        while i < len(content):
            if content[i] == '{':
                level += 1
            elif content[i] == '}':
                level -= 1
                if level == 0:
                    func_end = i + 1
                    rule_code = content[func_start:func_end]
                    rule_code = textwrap.dedent(rule_code).strip()
                    rule_codes.append(rule_code)
                    break
            i += 1
        else:
            raise ValueError(f"Unmatched braces in yacc rule for {opcode}")
    return '\n\n------\n\n'.join(rule_codes)

def extract_function_index(dilrun_cpp_path, opcode):
    """
    Step 3: Scan dilrun.cpp for the function index entry (e.g., {DILE_TOLOWER, dilfe_tolower}).
    Returns the dilfe_ function name.
    Fails if file not found or no matching entry.
    """
    if not os.path.exists(dilrun_cpp_path):
        raise FileNotFoundError(f"dilrun.cpp not found at {dilrun_cpp_path}")
    
    with open(dilrun_cpp_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    
    # Map opcode to DILE_ version (e.g., DILSE_TOLOWER -> DILE_TOLOWER)
    mapped_opcode = re.sub(r'^DIL[A-Z]+_', 'DILE_', opcode)
    
    # Match {OPCODE, dilfe_name},
    pattern = rf'\{{{mapped_opcode},\s*(\w+)\}}'
    match = re.search(pattern, content)
    
    if not match:
        # Map opcode to DILE_ version (e.g., DILSE_TOLOWER -> DILE_TOLOWER)
        mapped_opcode = re.sub(r'^DIL[A-Z]+_', 'DILI_', opcode)
        
        # Match {OPCODE, dilfe_name},
        pattern = rf'\{{{mapped_opcode},\s*(\w+)\}}'
        match = re.search(pattern, content)
        
        if not match:
            # Presume DILF_ and derive the function name
            func_name = 'DILF_' + re.sub(r'^DIL[A-Z]+_', '', opcode).upper()
            return func_name
        
    return match.group(1)  # e.g., 'dilfe_tolower'



def extract_case_from_dilfld(source_dir, dilfe_name):
    """
    Extracts the case statement implementation from dilfld.cpp for presumed DILF_ functions.
    Returns the extracted code snippet for the case.
    """
    dilfld_path = os.path.join(source_dir, "dilfld.cpp")
    if not os.path.exists(dilfld_path):
        raise FileNotFoundError(f"dilfld.cpp not found at {dilfld_path}")
    
    with open(dilfld_path, "r", encoding="utf-8", errors="ignore") as f:
        content = f.read()
    
    # Derive the case label, e.g., 'dilf_info' -> 'DILF_INFO'
    case_label = 'DILF_' + dilfe_name[5:].upper()
    
    # Match 'case DILF_XXX:'
    pattern = rf'case\s*{case_label}\s*:'
    match = re.search(pattern, content)
    if not match:
        raise ValueError(f"No case found for {case_label} in dilfld.cpp")
    
    # Start extracting from the case statement
    func_start = match.start()
    start_pos = match.end()
    level = 0
    i = start_pos
    found_break = False
    while i < len(content):
        if content[i] == '{':
            level += 1
        elif content[i] == '}':
            level -= 1
        elif content[i:i+6] == 'break;' and level == 0:
            func_end = i + 6
            function_code = content[func_start:func_end]
            import textwrap
            function_code = textwrap.dedent(function_code).strip()
            found_break = True
            break
        i += 1
    
    if not found_break:
        raise ValueError(f"No matching break; found for {case_label} in dilfld.cpp")
    
    return function_code


def extract_c_implementation(source_dir, dilfe_name):
    """
    Step 4: Scan source files in source_dir for the dilfe_ function definition.
    Searches all .cpp files.
    Returns the full function string.
    Fails if directory not found, no .cpp files, or no matching function.
    """
    if not os.path.isdir(source_dir):
        raise FileNotFoundError(f"Source directory not found at {source_dir}")
    found = False
    function_code = ""
    for file_name in os.listdir(source_dir):
        if file_name.endswith(".cpp"):
            file_path = os.path.join(source_dir, file_name)
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
                # Match void dilfe_name(dilprg *p) {
                header_pattern = rf'void\s*{dilfe_name}\s*\(dilprg\s*\*p\)\s*{{'
                match = re.search(header_pattern, content, re.DOTALL)
                if match:
                    if found:
                        raise ValueError(f"Multiple implementations found for {dilfe_name}")
                    found = True
                    # Parse the function body by counting braces
                    func_start = match.start()
                    start_pos = match.end()
                    level = 1
                    i = start_pos
                    while i < len(content):
                        if content[i] == '{':
                            level += 1
                        elif content[i] == '}':
                            level -= 1
                            if level == 0:
                                func_end = i + 1
                                function_code = content[func_start:func_end]
                                # New: Clean the extracted code
                                import textwrap
                                function_code = textwrap.dedent(function_code).strip()
                                break
                        i += 1
                    else:
                        raise ValueError(f"Unmatched braces in {dilfe_name}")
    if not found:
        return extract_case_from_dilfld(source_dir, dilfe_name)
        # raise ValueError(f"No C implementation found for {dilfe_name} in {source_dir}")

    return function_code


def extract_dil_functions_from_zones(zone_dir, keyword, overrides=None):
    """
    Step 5 & 6 combined (no grep output): Extract one dilbegin...dilend function containing the keyword.
    Prioritizes specific zones: haon-dor.zon, midgaard.zon, udgaard.zon, cypress.zon (add more to priority_list).
    If override provided (file_name, function_name), extracts that specific function.
    Returns a single extracted DIL function string.
    Fails if directory not found, no .zon files, or no matching function.
    """
    if not os.path.isdir(zone_dir):
        raise FileNotFoundError(f"Zone directory not found at {zone_dir}")
    
    priority_list = ["haon-dor.zon", "midgaard.zon", "udgaard.zon", "cypress.zon"]  # Add more here as needed
    
    # Sort files by priority (files in priority_list first, then others)
    all_files = [f for f in os.listdir(zone_dir) if f.endswith(".zon")]
    if not all_files:
        raise ValueError(f"No .zon files found in {zone_dir}")
    
    prioritized_files = sorted(
        all_files,
        key=lambda f: (priority_list.index(f) if f in priority_list else len(priority_list))
    )
    
    selected_example = None
    
    # Check for override first
    if overrides and keyword in overrides:
        override_file, override_func = overrides[keyword]
        if override_file not in all_files:
            raise ValueError(f"Override file {override_file} not found in {zone_dir}")
        
        file_path = os.path.join(zone_dir, override_file)
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()
        
        # Match specific dilbegin <function_name> ... dilend
        pattern = rf'dilbegin\s*(?:fnpri\([^\)]+\)\s*)?{re.escape(override_func)}\s*\([^\)]*\)\s*;(.*?)\s*dilend'
        match = re.search(pattern, content, re.DOTALL | re.IGNORECASE)
        
        if not match:
            raise ValueError(f"No matching DIL function '{override_func}' found in {override_file}")
        
        selected_example = f"dilbegin {match.group(0)}"

    else:
        # No override: Search prioritized files for any dil block containing keyword
        for file_name in prioritized_files:
            file_path = os.path.join(zone_dir, file_name)
            with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                content = f.read()
            
            # Find all dilbegin ... dilend blocks
            dil_blocks = re.findall(r'dilbegin\s*(.*?)\s*dilend', content, re.DOTALL | re.IGNORECASE)
            for block in dil_blocks:
                if re.search(rf'\b{re.escape(keyword)}\b', block):
                    selected_example = f"dilbegin {block} dilend"
                    break  # Take the first match in the highest priority file
            if selected_example:
                break
    
    if not selected_example:
        raise ValueError(f"No DIL function containing '{keyword}' found in zone files (with priority)")
    
    return selected_example  # Single string, not list

def main():
    # Configure paths (adjust these to your local setup)
    diltok_h_path = "../vmc/diltok.h"  # e.g., "~/diku/DikuMUD3/vme/src/vmc/diltok.h"
    dilpar_y_path = "../vmc/dilpar.y"
    dilrun_cpp_path = "../dilrun.cpp"
    source_dir = "../"  # Directory containing dilexp.cpp, etc.
    zone_dir = "../../zone/"   # Directory containing *.zon files
    output_dir = "./"
    
# Optional overrides: {keyword: (file_name, function_name)}
    overrides = {
        # Example: "can_carry": ("midgaard.zon", "janitors")
    }
    
    os.makedirs(output_dir, exist_ok=True)
    
    try:
        # Step 1: Get all keywords
        keywords = extract_keywords_from_diltok_h(diltok_h_path)
        print(f"Extracted {len(keywords)} keywords from diltok.h")
        
        for keyword, opcode in keywords:
            print(f"\nProcessing keyword: {keyword} ({opcode})")
            data = {
                "keyword": keyword,
                "opcode": opcode,
            }
            
            # Step 2
            print(f"\nProcessing yacc")
            data["yacc_rule"] = extract_yacc_rule(dilpar_y_path, opcode)
            
            # Step 3
            print(f"\nExtracting function index")
            dilfe_name = extract_function_index(dilrun_cpp_path, opcode)
            data["dilfe_name"] = dilfe_name
            
            # Step 4
            print(f"\nExtracting C implementation")
            data["c_implementation"] = extract_c_implementation(source_dir, dilfe_name)
            
            # Steps 5&6: No grep, just one DIL example with priority/override
            print(f"\nExtracting DIL example from zones")
            data["dil_example"] = extract_dil_functions_from_zones(zone_dir, keyword, overrides)

            prompt_file = os.path.join(output_dir, f"{keyword}_prompt.txt")
            with open(prompt_file, "w", encoding="utf-8") as pf:
                pf.write(f"Generate MCP entry for DIL keyword: {keyword}\n\n")
                pf.write("=== Yacc rule ===\n")
                pf.write(data["yacc_rule"])
                pf.write("\n\n=== C implementation ===\n")
                pf.write(data["c_implementation"])
                pf.write("\n\n=== DIL example ===\n")
                pf.write(data["dil_example"])
                pf.write("\n")
            print(f"   Clean prompt saved to {prompt_file}")

            # Save bundled data as JSON for prompting Grok/API later
            output_file = os.path.join(output_dir, f"{keyword}.json")
            with open(output_file, "w") as f:
                json.dump(data, f, indent=4, ensure_ascii=False)

            print(f"Saved data for {keyword} to {output_file}")
    
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
