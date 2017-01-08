//
//  tests.c
//  Sudoku
//
//  Created by Rolf on 28/12/2016.
//  Copyright © 2016 Rolf. All rights reserved.
//

#include "tests.h"
#include "engine.h"

#include <stdio.h>
#include <stdlib.h>

/*
 Regression tests for the Sudoku solver.
 
 It is assumed that kMaxNumber >= 9 in some tests.
 */


/**
 Execute all regression tests.
 */
void TestAll() {
    printf("Begin testing ...\n");
    TestCellsFromSquare();
    TestCellSettingAndRetrieval();
    TestBitCount();
    TestFilterCellsByClusters();
    TestFindLineClusters();
    TestFindLineClusters2();
    TestFindLineClusters3();
    printf("Testing done.\n\n");
}


/**
 Assert that a returned integer is what is expected. Print error statement if not.

 @param actual The actual value.
 @param expected The expected value.
 @param error_message The string to print when the assertion fails.
 */
void AssertIntEqual(int actual, int expected, char *error_message) {
    if (expected != actual) {
        printf("ERROR: %s\n", error_message);
        printf("\tExpected %d, found %d\n", expected, actual);
    }
}


/**
 Print header for regression test to standard output.

 @param header Test identifier.
 */
void PrintTestHeader(char *header) {
    printf(">>> Test: %s\n", header);
}


/**
 Test that setting a cell value and subsequently retrieving it gives 
 the expected result.
 */
void TestCellSettingAndRetrieval() {
    PrintTestHeader("cell setting and retrieval");
    struct SudokuBoard *board_ptr = blank_board();
    
    // Cells to assign value (row, col, value)
    int placements[] = {
        0, 0, 9,
        2, 7, 5,
        3, 4, 2,
        8, 8, 1
    };
    
    // Register which cells have been assigned to
    bool is_assigned[9][9];
    for (int row = 0; row < 9; ++row)
        for (int col = 0; col < 9; ++col)
            is_assigned[row][col] = false;
    
    int placement_len = sizeof placements / sizeof placements[0];
    
    // Check that the assigned cells have the correct value
    for (int offset = 0; offset < placement_len; offset += 3) {
        SetCell(board_ptr, placements[offset], placements[offset + 1], placements[offset + 2]);
        is_assigned[placements[offset]][placements[offset + 1]] = true;
    }
    
    // Check that the unassigned cells have the correct value (i.e., -1)
    for (int offset = 0; offset < placement_len; offset += 3) {
        int retrieved_value = SinglePossible((board_ptr->cells)[placements[offset]][placements[offset + 1]]);
        AssertIntEqual(retrieved_value, placements[offset+2], "Wrong cell value");
    }
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (!is_assigned[row][col]) {
                AssertIntEqual(SinglePossible((board_ptr->cells)[row][col]), -1, "Cell should not have value");
            }
        }
    }
}


/**
 Test the function CellsFromSquare.
 */
void TestCellsFromSquare() {
    PrintTestHeader("CellsFromSquare()");
    
    // We construct two boards for testing.
    // In numbered_squares_board_ptr all cells in each square have the same
    // definite number, the number of the square + 1.
    // In identical_squares_board_ptr, all squares look the same, the cells having
    // numbers from 1 to 9 starting from upper left (row=0, col=0) and then written left to right,
    // up to down to end with 9 in the lower right corner (row=2, col=2).
    struct SudokuBoard *numbered_squares_board_ptr = blank_board();
    struct SudokuBoard *identical_squares_board_ptr = blank_board();
    
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            SetCell(numbered_squares_board_ptr, row, col, (row / 3) * 3 + (col / 3) + 1);
            SetCell(identical_squares_board_ptr, row, col, (row % 3) * 3 + (col % 3) + 1);
        }
    }
    
    struct SudokuCell *identical_squares_cells;
    struct SudokuCell *numbered_squares_cells;
    
    for (int square_num = 0; square_num < 9; ++square_num) {
        identical_squares_cells = CellsFromSquare(identical_squares_board_ptr, square_num);
        numbered_squares_cells = CellsFromSquare(numbered_squares_board_ptr, square_num);
        
        for (int cell_num = 0; cell_num < 9; ++cell_num) {
            AssertIntEqual(identical_squares_cells[cell_num].possibles, 1 << cell_num, "Wrong cell value");
            AssertIntEqual(numbered_squares_cells[cell_num].possibles, 1 << square_num, "Wrong cell value");
        }
        
        free(identical_squares_cells);
        free(numbered_squares_cells);
    }
}


/**
 Test the function FindLineClusters.
 */
void TestFindLineClusters() {
    PrintTestHeader("FindLineClusters");
    
    struct SudokuCell cells[kMaxNumber];
    
    // Assume kMaxNumber >= 9
    for (int index = 0; index < kMaxNumber; ++index)
        cells[index].possibles = (1 << kMaxNumber) - 1; // initialize to all being possible
    
    // 3-cluster
    cells[0].possibles = 3; // 1 and 2
    cells[2].possibles = 6; // 2 and 3
    cells[4].possibles = 5; // 1 and 3
    
    // 2-cluster
    int bits_for_two = (1 << (4 - 1)) | (1 << (6 - 1)); // 4 and 6
    cells[1].possibles = bits_for_two;
    cells[8].possibles = bits_for_two;
    
    // 1-cluster
    cells[7].possibles = 1 << 6; // That is cell 8 has value 7.
    
    struct LineClusterGroups groups = FindLineClusters(cells);
    
    AssertIntEqual(groups.size, 3, "Incorrect number of clusters");
    int sum_group_sizes = 0;
    
    for (int group_index = 0; group_index < groups.size; ++group_index) {
        struct LineCluster group = groups.clusters[group_index];
        
        sum_group_sizes += group.size;
        
        switch(group.size) {
            case 1:
                AssertIntEqual(group.bitset, 1 << 6, "Incorrect 1-cluster value bitset");
                AssertIntEqual(group.positions[0], 7, "Wrong 1-cluster position");
                break;
            case 2:
                AssertIntEqual(group.bitset, bits_for_two, "Incorrect 2-cluster value bitset");
                AssertIntEqual(group.positions[0], 1, "Wrong 2-cluster position");
                AssertIntEqual(group.positions[1], 8, "Wrong 2-cluster position");
                break;
            case 3:
                AssertIntEqual(group.bitset, 7, "Incorrect 3-cluster value bitset");
                AssertIntEqual(group.positions[0], 0, "Wrong 3-cluster position");
                AssertIntEqual(group.positions[1], 2, "Wrong 3-cluster position");
                AssertIntEqual(group.positions[2], 4, "Wrong 3-cluster position");
                break;
            default:
                printf("Incorrect cluster size: %d\n", group.size);
                break;
        }
    }
    
    AssertIntEqual(sum_group_sizes, 6, "Incorrect group size checksum");
}


/**
 Test the function FindLineClusters.
 */
void TestFindLineClusters2() {
    PrintTestHeader("FindLineClusters (2)");
    
    // only works on size 9
    
    struct SudokuCell cells[kMaxNumber];
    int cell_values[] = {32, 2, 8, 4, 256, 144, 1, 64, 16};
    
    // Assume kMaxNumber >= 9
    for (int index = 0; index < kMaxNumber; ++index)
        cells[index].possibles = cell_values[index];
    
    struct LineClusterGroups groups = FindLineClusters(cells);
    
    AssertIntEqual(groups.size, 8, "Incorrect number of cluster groups");
}


/**
 Test the function FindLineClusters.
 */
void TestFindLineClusters3() {
    PrintTestHeader("FindLineClusters (3)");
    
    struct SudokuCell cells[kMaxNumber];
    
    // Assume kMaxNumber >= 9
    for (int index = 0; index < kMaxNumber; ++index)
        cells[index].possibles = kAllBits;
    cells[kMaxNumber - 1].possibles = 16;
    
    struct LineClusterGroups groups = FindLineClusters(cells);
    
    AssertIntEqual(groups.size, 1, "Incorrect number of cluster groups");
}



/**
 Test the function BitCount.
 */
void TestBitCount() {
    PrintTestHeader("BitCount");
    
    char *err_msg = "Incorrect bitcount";
    
    AssertIntEqual(BitCount(5), 2, err_msg);
    AssertIntEqual(BitCount(0), 0, err_msg);
    AssertIntEqual(BitCount(64), 1, err_msg);
    AssertIntEqual(BitCount(kAllBits), kMaxNumber, err_msg);
}


/**
 Test the function FilterCellsByClusters.
 */
void TestFilterCellsByClusters() {
    PrintTestHeader("FilterCellsByClusters");
    
    struct SudokuCell cells[kMaxNumber];    // The cells we will work on
    struct SudokuCell expected[kMaxNumber]; // These will contain the expected outcome

    // Assume kMaxNumber >= 9
    for (int index = 0; index < kMaxNumber; ++index) {
        cells[index].possibles = (1 << kMaxNumber) - 1; // initialize to all being possible
        expected[index].possibles = (1 << kMaxNumber) - 1;
    }
    // 3-cluster
    cells[0].possibles = 3; // 1 and 2
    cells[2].possibles = 6; // 2 and 3
    cells[4].possibles = 5; // 1 and 3
    
    // 2-cluster
    int bits_for_two = (1 << (4 - 1)) | (1 << (6 - 1)); // 4 and 6
    cells[1].possibles = bits_for_two;
    cells[8].possibles = bits_for_two;
    
    // Add a singlet
    cells[7].possibles = 1 << 6; // That is cell 8 has value 7.
    
    // Construct the expected values after filtering
    int bit_filter = 7 | bits_for_two | 1 << 6;
    for (int cell_index = 0; cell_index < kMaxNumber; ++cell_index) {
        expected[cell_index].possibles ^= bit_filter; // remove the known clusters
    }
    
    // Now set the clusters
    expected[0].possibles = 3;
    expected[2].possibles = 6;
    expected[4].possibles = 5;
    expected[1].possibles = bits_for_two;
    expected[8].possibles = bits_for_two;
    expected[7].possibles = 1 << 6;
    
    struct LineClusterGroups groups = FindLineClusters(cells);
    FilterCellsByClusters(cells, &groups);
    
    char err_msg[60];
    for (int cell_index = 0; cell_index < kMaxNumber; ++cell_index) {
        sprintf(err_msg, "Incorrect cell value (cell %d)", cell_index);
        AssertIntEqual(cells[cell_index].possibles, expected[cell_index].possibles, err_msg);
    }
}
