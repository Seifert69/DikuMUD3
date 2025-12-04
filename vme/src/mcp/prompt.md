Execute the  shell script:

./check_wiki_status.sh | tail -10

For each of the 10 <file>.wiki do this:

Ensure that the styling adheres to the WikiMedia markdown principles below.

Critically evaluate the code examples so that there is just the amount needed - not too much - not too little. Adjust the examples as necessary.

If you need to you can cross reference the other .wiki files in this folder. 

It shouldn't be necessary, but you can also cross reference the zone files in ../../zone/*.zon if you need to check a real live working example.

If a .wiki file does NOT need modification then please execute this script once per file to add a spacec so that the file is "modified" (and thus will not be repeated in your next batch of files):

echo -n " " >> <file>.wiki

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
- Include multiple example categories only if relevant
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

