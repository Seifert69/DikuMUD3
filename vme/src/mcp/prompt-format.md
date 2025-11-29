The file:

has incorrect WikiMedia Markdown. Please fix it, observe samples below:

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

