//
//  examples.c
//  Sudoku
//
//  Created by Rolf on 08/01/2017.
//  Copyright © 2017 Rolf. All rights reserved.
//

#include "engine.h"
#include "examples.h"


/**
 Return pointer to struct SudokuBoard for the puzzle with the 
 following setup:
 
 ..5 .7. 8..
 ..4 ... 1..
 8.7 ... .96
 
 ... 1.. .28
 ... 2.. 9..
 4.. 95. .1.
 
 ... .39 .51
 356 8.2 ...
 .1. ... ...
 
 @return pointer to struct SudokuBoard.
 */
struct SudokuBoard * example1() {
    struct SudokuBoard *board_ptr = blank_board();
    int placements[] = {
        0, 2, 5,
        1, 2, 4,
        2, 2, 7,
        2, 0, 8,
        
        0, 4, 7,
        
        0, 6, 8,
        1, 6, 1,
        2, 7, 9,
        2, 8, 6,
        
        5, 0, 4,
        
        3, 3, 1,
        4, 3, 2,
        5, 3, 9,
        5, 4, 5,
        
        3, 7, 2,
        3, 8, 8,
        4, 6, 9,
        5, 7, 1,
        
        7, 0, 3,
        7, 1, 5,
        7, 2, 6,
        8, 1, 1,
        
        6, 4, 3,
        6, 5, 9,
        7, 3, 8,
        7, 5, 2,
        
        6, 7, 5,
        6, 8, 1
    };
    
    for (int base_index = 0; base_index < sizeof placements / sizeof(int); base_index += 3)
        SetCell(board_ptr, placements[base_index], placements[base_index + 1], (unsigned int) placements[base_index + 2]);

    return board_ptr;
}
