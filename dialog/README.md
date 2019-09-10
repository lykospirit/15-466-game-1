# 15-466 Dialog Pipeline

Dialog is represented by two files:

 - `dialog.list`: Encodes the **logic** of the dialog. Contains **logic variables** which are queried, added or removed.
 - `variable-to-sprite.list`: Encodes the **sprite** associated with specific **logic variables**

This dialog pipeline compiles these two files as `dialog.cpl`, as a form that is easier to use at runtime.

## Compilation

Dialog compilation is done by `compile-dialog.py`:

```
python compile-dialog.py
```

`dialog.list` encodes dialog logic as a series of linear logic statements. Logic variables are added to the global scope, queried for existence, or removed from the global scope. Logic variables are represented by alphanumeric strings. It uses the following standard format:

```
$Event: $Variable-Queries $Variable-Changes $Text
```
 - `$Event` is a string encoding an event, normally triggered by some player interaction.
 - `$Variables-Queries` take the form: `"?" [a-zA-Z0-9]`, and lists the logic variables that must be present for the rule to be executed.
 - `$Variables-Changes` take the form: `"+" [a-zA-Z0-9]` or `"-" [a-zA-Z0-9]`. Respectively, the logic variables are added to, or removed from, the global scope if the event is triggered.
 - `$Text` is a string, surrounded by double quotation marks (""), that represents what the text becomes if the event is triggered.

Reserved symbols include :, ?, +, - and "".

`variable-to-sprite.list` encodes a variable-to-sprite mapping. Each line encodes one such mapping. It uses the following standard format:

```
$Variable: $Sprite
```
Reserved symbols include :. Variables that are not found in `dialog.list` are silently ignored.


These files are compiled into `dialog.cpl` by `compile-dialog.py` in the following format:
```
$Event: {$Queries} <$Set_to_true> !$Set_to_false! ($Sprite_turn_on) [$Sprite_turn_off] "$Text"
```
`dialog.cpl` copies the names of events, logic variables and sprite names as-is from `dialog.list` and `variable-to-sprite.list`.

 - `$Event` is a string encoding an event, normally triggered by some player interaction. These are copied from `dialog.list`.
 - `$Queries` lists the logic variables that must be present for the rule to be executed.
 - `$Set_to_true` lists the logic variables that are added to the global scope if the rule is executed.
 - `$Set_to_false` lists the logic variables that are removed from the global scope if the rule is executed.
 - `$Sprite_turn_on` lists the sprites that are to be additionally displayed if the rule is executed.
 - `$Sprite_turn_off` lists the sprites that should no longer be displayed if the rule is executed.
 - `$Text` represents what the text becomes if the rule is executed.
