# generate_dil_types.py – single script version
# Parses dilpar.y for types and diltok.h for names, combines them
import re
import sys
from datetime import datetime
from collections import defaultdict

if len(sys.argv) != 3:
    print("Usage: python generate_dil_types.py path/to/dilpar.y path/to/diltok.h")
    sys.exit(1)

yacc_file = sys.argv[1]
dilsym_file = sys.argv[2]

# Step 1: Parse dilsym[] from diltok.h to get opcode → full name
dilsym_content = open(dilsym_file, "r", encoding="utf-8", errors="ignore").read()
pattern = r'{"([^"]+)",\s*(DIL[SEIF][A-Z_]+)}'
matches = re.findall(pattern, dilsym_content)
opcode_to_name = {opcode: name for name, opcode in matches}

# Step 2: Parse grammar from dilpar.y
content = open(yacc_file, "r", encoding="utf-8", errors="ignore").read()

# Get the rules section after %%
parts = re.split(r'%%', content)
rules_content = parts[1] if len(parts) > 1 else content

# Remove everything up to the first :
rules_content = re.sub(r'^.*?:', '', rules_content, flags=re.DOTALL)

# Split into subrules on |
subrules = re.split(r'\s*\|\s*', rules_content)

rule_matches = []

for subrule in subrules:
    if not subrule.strip():
        continue
    m = re.match(r'\s*(DIL[SEIF][A-Z_]+)\s*(.*?)\s*\{(.*)\}\s*;?\s*$', subrule, re.DOTALL)
    if m:
        opcode, rule_part, action_part = m.groups()
        rule_part = rule_part.strip() if rule_part else ""
        rule_matches.append((opcode, rule_part, action_part))

# Step 3: Generate the header
print("// AUTO-GENERATED – DO NOT EDIT")
print("// Generated from dilpar.y and diltok.h on", datetime.now().strftime("%Y-%m-%d %H:%M"))
print("#pragma once")
print("")
print("struct dil_type_map { const char *keyword; const char *type; const char *params; };")
print("")
print("static const struct dil_type_map dil_types[] = {")
for opcode, rule_part, action_part in rule_matches:
    action_part = action_part.strip()
    # Find the return type
    typ_m = re.search(r'\$\$\.typ\s*=\s*DilVarType_e::(DILV_[A-Z0-9_]+)|\$\$\s*=\s*DilVarType_e::(DILV_[A-Z0-9_]+)', action_part)
    if typ_m:
        typ = typ_m.group(1) if typ_m.group(1) else typ_m.group(2)
        keyword = opcode_to_name.get(opcode, opcode.lower().replace("dilse_", "").replace("dili_", "").replace("dilsf_", ""))
        type_map = {
            "DILV_UP": "unitptr",
            "DILV_SP": "string",
            "DILV_SLP": "stringlist",
            "DILV_INT": "int",
            "DILV_EDP": "extraptr",
            "DILV_ZP": "zoneptr",
            "DILV_CP": "cmdptr",
            "DILV_ILP": "intlist",
            "DILV_NULL": "null",
        }
        human_type = type_map.get(typ, typ.lower())
        params_str = ""
        arg_count = len(re.findall(r'\bdilexp\b', rule_part))
        if arg_count > 0:
            positions = [3 + 2 * k for k in range(arg_count)]  # yacc $ positions for args
            param_types = defaultdict(list)
            type_matches = re.findall(r'\$(\d+)\.typ\s*([!=]=?)\s*DilVarType_e::(DILV_[A-Z_]+)', action_part)
            for n_str, op, ptyp in type_matches:
                if op == '!=':
                    param_types[int(n_str)].append(ptyp)
            param_list = []
            for pos in positions:
                if pos in param_types and param_types[pos]:
                    humans = [type_map.get(t, t.lower()) for t in param_types[pos]]
                    param_list.append('|'.join(sorted(set(humans))))
                else:
                    param_list.append("unknown")
            params_str = ', '.join(param_list)
        print(f'    {{"{keyword}", "{human_type}", "{params_str}"}},  // {opcode} → {typ}')
print("    {nullptr, nullptr, nullptr}")
print("};")