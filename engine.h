//
//  engine.h
//  Sudoku
//
//  Created by Rolf on 28/12/2016.
//  Copyright © 2016 Rolf. All rights reserved.
//

#ifndef engine_h
#define engine_h

#include <stdbool.h>

/**
 Constants
 */

extern const int kCellsPerSide;
extern const int kMaxNumber;
extern const int kAllBits;

/**
 Structs
 */

struct SudokuCell {
    unsigned int possibles : 9; // bitfield marking which numbers are still in the mix
};

struct SudokuBoard {
    struct SudokuCell cells[9][9];
};

struct LineCluster {
    int size;
    int bitset;
    int *positions;
};

struct LineClusterGroups {
    int size;
    struct LineCluster *clusters;
};

struct Cluster {
    int size;
    int *numbers;
    struct Position *positions;
};

struct ClusterGroups {
    int size;
    struct Cluster *clusters;
};

/**
 Functions
 */

struct SudokuBoard * blank_board();
int SinglePossible(struct SudokuCell cell);
void SetCell(struct SudokuBoard *board_ptr, int row, int col, unsigned int value);
void PrintBoard(struct SudokuBoard *board_ptr);
void SetBooleanArray(bool *array_ptr, int length, bool value);
int RowAndColToSquare(int row, int col);
struct SudokuCell *CellsFromSquare(struct SudokuBoard const *board_ptr, int square_num);
void SolveBoard(struct SudokuBoard *board_ptr);
struct LineClusterGroups FindLineClusters(struct SudokuCell const cells[kMaxNumber]);
void SetCellsOnRow(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber], int row);
void SetCellsOnColumn(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber], int col);
void SetCellsOnSquare(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber],
                      int square_num);
int BitCount(int bits);
void FilterCellsByClusters(struct SudokuCell cells[kMaxNumber], struct LineClusterGroups const *clusters);

#endif /* engine_h */
