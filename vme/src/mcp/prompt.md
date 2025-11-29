Run this command:

for file in *_prompt.txt; do name="${file%_prompt.txt}"; if [[ ! -f "${name}.wiki" ]]; then echo "$file"; break; fi; done

The resulting file is the <command> that we want to process. 
Then read the <command>_prompt.txt file, which is based on this automated extract from DIKUMUD DIL. 

Do NOT create any output about YACC: only use YACC understand the DIL function, procedure, or field.
Do NOT create any output about C implementation: only use the C code to understand the DIL function, procedure, or field, and to make relevant notes to the user.

Then lookup any DIL manual .hlp files that are relevant to understand the function, procedure, or field being described - the DIL manual files might be incorrect as
they are very dated.

Finally with all this knowledge create a high quality WikiMedia markdown article for the function, procedure, or field described in the <command>_prompt.txt file. Output and save the article as a .wiki file with the same name as the function in question.

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

The function heading must be 
= <command> =

Do NOT use these for markdown:
# something  -> instead use =
## something  -> instead use ==
{{something}}
<pre>something</pre> -> instead use space for preformatted text. Do NOT add blank lines in code blocks / preformatted text.
``` is NOT for code indentation -> use space instead

In this folder there are help files form the DIL manual: grep "text" *.hlp to find which files you might want to read.
In this folder there are code extracts on disk from YACC & C++. grep "text" *.txt to find which files you might want to read.

As a minimum you definitely want to load the <command>.hlp and <command>_prompt.txt files for <command> file you're currently invesstigating.
