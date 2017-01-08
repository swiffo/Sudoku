//
//  main.c
//  Sudoku
//
//  Created by Rolf on 25/12/2016.
//  Copyright © 2016 Rolf. All rights reserved.
//

#include <stdio.h>
#include "engine.h"
#include "examples.h"
#include "tests.h"

int main(int argc, const char * argv[]) {
    TestAll();
    
    struct SudokuBoard *board_ptr = example1();
    PrintBoard(board_ptr);
    
    SolveBoard(board_ptr);
    PrintBoard(board_ptr);
    
    return 0;
}
