Execute the shell script getfile.sh which will output <command>_prompt.txt which is the file you must process.

Then read the <command>_prompt.txt file, which is based on this automated extract from DIKUMUD DIL. 

As a minimum you definitely want to load the <command>.hlp and <command>_prompt.txt files for <command> you're currently invesstigating.

In this folder there are many help files form the DIL manual: grep "text" *.hlp to find which files you might want to read.
In this folder there are many code extracts on disk from YACC & C++. grep "text" *.txt to find which files you might want to read.

Do NOT create any output about YACC: only use YACC understand the DIL function, procedure, or field.
Do NOT create any output about C implementation: only use the C code to understand the DIL function, procedure, or field, and to make relevant DIL notes to the user.

Then lookup any DIL manual .hlp files that are relevant to understand the function, procedure, or field being described - the DIL manual files might be incorrect as they are very dated.

Finally with all this knowledge create a high quality WikiMedia markdown article for the function, procedure, or field described in the <command>_prompt.txt file. Output and save the article as a .wiki file with the name <command>.wiki


When creating or editing DikuMUD3 .wiki documentation files, follow this standard:

= <command> =
 function: [return-type] '''[function-name]'''([list of {function-parameter-name : type}]);
 field: [type] '''.loadcount''' [eead-only or read-write]
 type: '''[type]'''
 constant: '''[constant]'''
 variable: '''[variable]'''
 
If a parameter or return type is a certain unitptr type use:
  'unit', 'pc', 'npc', 'char', 'room' or 'obj' to indicate the kind
  For example: 
  function: '''addequip'''(char : unitptr, i : integer); 

Add a very brief introductory sentence about the <command>.

STANDARD SECTIONS (use in this order)
2. == Description == 
3. == PARAMETERS == (for functions only)
5. == Field Access == (for fields - use table format)
6. == Availability == (for fields - list applicable unit types)
7. == Examples ==
8. == Usage Notes ==
9. == Error Handling ==
10. == Related Functions/Fields ==
11. == See Also ==

PARAMETER DOCUMENTATION
ALWAYS use table format:
{| class="wikitable"
! Parameter !! Type !! Description
|-
| param_name || type || description
|}

UNIT TYPE DOCUMENTATION
For fields, always include an Availability section:
== Availability ==
This field is available on the following unit types:
* UNIT_ST_PC (Player Character)
* UNIT_ST_NPC (Non-Player Character)
* UNIT_ST_OBJ (Object)
* UNIT_ST_ROOM (Room)
* cmdptr (Command pointer)
* extraptr (Extra description pointer)
List only the applicable types.

## TYPE INDICATORS
Always specify:
- Parameter types: unitptr, integer, string, extraptr, cmdptr
- Read/Write access: (Read-only) or (Read/Write)
- Context-dependent behavior

## EXAMPLES
- Use complete DIL code blocks with dilbegin...dilend
- Include multiple example categories when relevant
- Show real-world usage scenarios
## CONSISTENCY REQUIREMENTS
- Use '''bold''' for function/field names in descriptions
- Use consistent terminology (PC vs Player Character, etc.)
- Always include error handling section for functions
- Cross-reference related entries using [[double brackets]]

WikiMedia VALID Markdown

= Heading Level 1 =
== Heading Level 2 ==
=== Heading Level 3 ===
==== Heading Level 4 ====

'''Bold text'''
''Italic text''
'''''Bold italic text'''''

 Preformatted text / code block
 leading space on each line
 Example line 1
 Example line 2

:Indented line (1 level)
::Indented line (2 levels)
:::Indented line (3 levels)

; spell_index : integer (this will be bold on the line before)
: The explanation is then indented

{| class="wikitable"
! Header 1 !! Header 2 !! Header 3
|-
| Cell 1-1 || Cell 1-2 || Cell 1-3
|-
| Cell 2-1 || Cell 2-2 || Cell 2-3
|-
| Cell 3-1 || Cell 3-2 || Cell 3-3
|}

INVALID WikiMedia markdown - DO NOT USE THIS:
# something  -> instead use =
## something  -> instead use ==
{{something}} -> do not use
<pre>incorrect</pre> -> instead use space for preformatted text. Do NOT add blank lines in code blocks / preformatted text.
``` is NOT for code indentation -> use space instead

