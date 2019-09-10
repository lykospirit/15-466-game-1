# Tested on Python 2.7.12
import os, sys
from collections import OrderedDict

f_dialog   = open("dialog.list", "r")
f_varmap   = open("variable-to-sprite.list", "r")
f_compiled = open("../dist/dialog.cpl", "w")

# Extract variable-to-sprite mapping
var_to_sprite = dict()
for line in f_varmap:
  if line.find(':') >= 0:
    variable = line[:line.find(':')].strip()
    sprite = line[line.find(':')+1:].strip()
    var_to_sprite[variable] = sprite

# Extract information from dialog file
dialog = []
for line in f_dialog:
  if line.find(':') >= 0:
    # Sort rules into pred(icates), add(itions), rem(ovals)
    event = line[:line.find(':')].strip()
    check = line[line.find(':')+1:].strip()
    pred, add, rem = [], [], []
    while len(check) > 0:
      if check[0] in ['?','+','-']:
        next = check[1:check.find(' ')].strip()
        if check[0] == '?': pred.append(next)
        elif check[0] == '+': add.append(next)
        else: rem.append(next)
        check = check[check.find(' ')+1:].strip()
      elif check[0] == '"':
        next = check
        check = ""
    # Replace with sprite names if they exist; split into other list otherwise
    find = lambda x: var_to_sprite[x] if x in var_to_sprite else "**" + x
    remove = lambda x: len(x) >= 2 and x[:2] == "**"
    keep = lambda x: not remove(x)
    clean = lambda x: x[2:]
    add, rem = map(find, add), map(find, rem)
    dialog.append( (event, (pred,
                     map(clean, filter(remove, add)),
                     map(clean, filter(remove, rem)),
                     filter(keep, add),
                     filter(keep, rem),
                     next)) )

# Print into compiled format:
# $event: {$queries} <$set_to_true> !$set_to_false! ($sprite_turn_on) [$sprite_turn_off] "$text"
# EG: room/gotoExit: {isWashed,isDressed} <> !! (exit-bg) [room-bg] "You leave."
#     room/interactCloset: {} <isDressed> !isMessy! (clothes) [] "You get dressed."
for event in dialog:
  f_compiled.write("{}: {{{}}} <{}> !{}! ({}) [{}] {}\n".format(
    event[0],
    ','.join(event[1][0]),
    ','.join(event[1][1]),
    ','.join(event[1][2]),
    ','.join(event[1][3]),
    ','.join(event[1][4]),
    event[1][5]
  ))
