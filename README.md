# Sudoku
Sudoku - the ancient Japanese art of wasting time by putting numbers in a grid.

The code implements a solver for the more basic puzzles. It works entirely by elimination of possibilities. 
Puzzles with multiple solutions will leave it nonplussed.

There is no interface for interacting with the solver. It simply runs on the one example it knows (stolen from some article).
Specifically this example:
```
..5|.7.|8..
..4|...|1..
8.7|...|.96
---+---+---
...|1..|.28
...|2..|9..
4..|95.|.1.
---+---+---
...|.39|.51
356|8.2|...
.1.|...|...
```
The solution which it finds is below:
```
195|674|832
624|398|175
837|521|496
---+---+---
963|147|528
571|283|964
482|956|317
---+---+---
248|739|651
356|812|749
719|465|283
```
