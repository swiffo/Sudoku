//
//  engine.c
//  Sudoku
//
//  Created by Rolf on 28/12/2016.
//  Copyright © 2016 Rolf. All rights reserved.
//

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

/**
 The classic sudoku puzzle is divided into 9 squares of 9 cells. That is,
 the it is a 3x3 matrix of 3x3 squares.  For any number, N, one could have 
 a similarly defined puzzle that is an NxN matrix of NxN squares. 
 
 We call this N, kCellsPerSide.
 */
const int kCellsPerSide = 3;

/**
The largest number (and the number of cells in a single square) is the square
 of kCellsPerSide. We assign this to kMaxNumber.
 */
const int kMaxNumber = kCellsPerSide * kCellsPerSide;

/**
 Most of the work to solve the puzzle will be done representing the possible numbers
 in a cell as a bitfield of kMaxNumber bits. In that context the bitfield with all 
 bits set appears frequently and we assign that to kAllBits.
 */
const int kAllBits = (1 << kMaxNumber) - 1;


/**
 Allocate Sudoku board on heap, initialize all cells to be blank (i.e., 
 bitfield = kAllBits) and return pointer to the board.
 
 @return pointer to struct Sudokuboard.
 */
struct SudokuBoard * blank_board() {
    struct SudokuBoard *board_ptr = malloc(sizeof(struct SudokuBoard));
    
    for (int row = 0; row < kMaxNumber; ++row)
        for (int col = 0; col < kMaxNumber; ++col)
            ((board_ptr->cells)[row][col]).possibles = kAllBits; // kMaxNumber 1s
    
    return board_ptr;
}


/**
 Return number of bits set.

 @param bits Bitfield as integer
 @return int specifying number of bits set in bitfield
 */
int BitCount(int bits) {
    int bit_count = 0;
    
    while (bits != 0) {
        bits &= bits - 1;
        ++bit_count;
    }
    
    return bit_count;
}

/**
 If the cell has only one possible number, return that. Otherwise return -1.
 
 @param cell : struct SudokuCell.
 @return -1 if multiple values are possible for the cell; otherwise the only possible value.
 */
int SinglePossible(struct SudokuCell cell) {
    int found = -1;
    int found_count = 0;
    
    for (int digit = 1; digit <= kMaxNumber; ++digit) {
        if (cell.possibles & (1 << (digit - 1))) {
            found = digit;
            ++found_count;
        }
    }
    
    if (found_count > 1)
        found = -1;
    
    return found;
}

/**
 Set the specified cell on the sudoku board to have only the specified 
 value as possible. I.e., assign the value to that cell, removing all 
 uncertainty.

 @param board_ptr Pointer to struct SudokuBoard
 @param row Row on sudoku board (0, ..., kMaxNumber - 1)
 @param col Column on sudoku board (0, ..., kMaxNumber - 1)
 @param value The value to assign to the cell determined by (row, col). 
              Must be between 1 and kMaxNumber.
 */
void SetCell(struct SudokuBoard *board_ptr, int row, int col, unsigned int value) {
    (board_ptr->cells)[row][col].possibles = 1 << (value - 1);
}


/**
 Print a representation of the sudoku board to standard output.

 @param board_ptr Pointer to struct SudokuBoard.
 */
void PrintBoard(struct SudokuBoard *board_ptr) {
    bool is_separator_row, is_separator_col;
    int board_row, board_col;
    
    for (int row = 0; row < kMaxNumber + kCellsPerSide - 1; ++row) {
        for (int col = 0; col < kMaxNumber + kCellsPerSide - 1; ++col) {
            is_separator_row = (row + 1) % (kCellsPerSide + 1) == 0;
            is_separator_col = (col + 1) % (kCellsPerSide + 1) == 0;
            
            if (is_separator_row && is_separator_col) {
                printf("+");
            }
            else if (is_separator_row) {
                printf("-");
            }
            else if (is_separator_col) {
                printf("|");
            }
            else {
                board_row = row - row / (kCellsPerSide + 1);
                board_col = col - col / (kCellsPerSide + 1);
                
                int val = SinglePossible((board_ptr->cells)[board_row][board_col]);
                if (val == -1)
                    printf(".");
                else
                    printf("%d", val);
            }
        }
        printf("\n");
    }
    printf("\n");
}


/**
 Set all values in a Boolean array to the specified value.

 @param array_ptr The array to modify.
 @param length The length of the array.
 @param value The value to be assigned.
 */
void SetBooleanArray(bool *array_ptr, int length, bool value) {
    while (--length != -1) {
        array_ptr[length] = value;
    }
}

/**
 From row and column, output the number of square to which they belong.
 
 We divide the board into kMaxNumber squares, labelled 0, ..., kMaxNumber,
 starting top-left, ending bottom-right, going row by row.

 @param row The row on the board (between 0 and kMaxNumber - 1)
 @param col The column on the board (between 0 and kMaxNumber - 1)
 @return The number of the square (between 0 and kMaxNumber - 1)
 */
int RowAndColToSquare(int row, int col) {
    return kCellsPerSide * (row / kCellsPerSide) + col / kCellsPerSide;
}

/**
 Return all cells in a square as an array.
 
 @param board_ptr The sudoku board.
 @param square_num The number of the square as in description of RowAndColToSquare().
 @return Pointer to first element of array of cells. Length is kMaxNumber.
 */
struct SudokuCell *CellsFromSquare(struct SudokuBoard const *board_ptr, int square_num) {
    int first_row = (square_num / kCellsPerSide) * kCellsPerSide;
    int first_col = (square_num % kCellsPerSide) * kCellsPerSide;
    
    struct SudokuCell *cell_array = calloc(kMaxNumber, sizeof(struct SudokuCell));
    
    for (int row = first_row; row < first_row + kCellsPerSide; ++row) {
        for (int col = first_col; col < first_col + kCellsPerSide; ++col) {
            cell_array[kCellsPerSide * (row - first_row) + (col - first_col)] = board_ptr->cells[row][col];
        }
    }
    
    return cell_array;
}


/**
 Return all cells in a row in the sequence of col = 0, ..., kMaxNumber as
 an array.

 @param board_ptr Pointer to the sudoku board.
 @param row The row from which to return the cells.
 @return Pointer to first element of array of cells. Length is kMaxNumber.
 */
struct SudokuCell *CellsFromRow(struct SudokuBoard *board_ptr, int row) {
    struct SudokuCell *cell_ptr = malloc(kMaxNumber * sizeof(struct SudokuCell));
    
    for (int col = 0; col < kMaxNumber; ++col)
        cell_ptr[col] = board_ptr->cells[row][col];
    
    return cell_ptr;
}

/**
 Return all cells in a column in the sequence of row = 0, ..., kMaxNumber as
 an array.
 
 @param board_ptr Pointer to the sudoku board.
 @param col The column from which to return the cells.
 @return Pointer to first element of array of cells. Length is kMaxNumber.
 */
struct SudokuCell *CellsFromColumn(struct SudokuBoard *board_ptr, int col) {
    struct SudokuCell *cell_ptr = malloc(kMaxNumber * sizeof(struct SudokuCell));
    
    for (int row = 0; row < kMaxNumber; ++row)
        cell_ptr[row] = board_ptr->cells[row][col];
    
    return cell_ptr;
}

/**
 Given an array of sudoku cells and information about the clustering therein,
 filter the possible values of each cell in place.

 @param cells Array of sudoku cells. These will be modified.
 @param clusters LineClusterGroup with information about the clustering.
 */
void FilterCellsByClusters(struct SudokuCell cells[kMaxNumber], struct LineClusterGroups const *clusters) {
    for (int cluster_num = 0; cluster_num < clusters->size; ++cluster_num) {
        struct LineCluster cluster = clusters->clusters[cluster_num];
        bool is_cluster_position[kMaxNumber];
        SetBooleanArray(is_cluster_position, kMaxNumber, false);
        for (int cluster_index = 0; cluster_index < cluster.size; ++cluster_index)
            is_cluster_position[cluster.positions[cluster_index]] = true;
        
        int bitfilter = kAllBits ^ cluster.bitset;
        for (int cell_index = 0; cell_index < kMaxNumber; ++cell_index) {
            if (!is_cluster_position[cell_index])
                cells[cell_index].possibles &= bitfilter;
        }
    }
}


/**
 Return simple hash for sudoku board. If s1 and s2 are states of the same board 
 but s2 is in a more solved state than s1 (i.e., for each cell, the number of 
 possible values in s2 is less than or equal to that of s1 with strict inequality
 in at least one case) the checksums are guaranteed to be different.
 
 @param board_ptr Pointer to sudoku board.
 @return Integer (hash).
 */
int BoardCheckSum(struct SudokuBoard const *board_ptr) {
    int checksum = 0;
    
    for (int row = 0; row < kMaxNumber; ++row) {
        for (int col = 0; col < kMaxNumber; ++col) {
            checksum += BitCount(board_ptr->cells[row][col].possibles);
        }
    }
    
    return checksum;
}


/**
 Reduce possibilities in each cell as much as possibly with the hope
 (but not guarantee) of reducing each cell to a single possible number.

 @param board_ptr Pointer to the suduko puzzle to be solved. The board will be modified.
 */
void SolveBoard(struct SudokuBoard *board_ptr) {
    int checksum, last_checksum;
    checksum = BoardCheckSum(board_ptr);
    do {
        last_checksum = checksum;
        
        for (int row = 0; row < kMaxNumber; ++row) {
            struct SudokuCell *cells = CellsFromRow(board_ptr, row);
            struct LineClusterGroups clusters = FindLineClusters(cells);
            FilterCellsByClusters(cells, &clusters);
            
            SetCellsOnRow(board_ptr, cells, row);
            free(cells);
        }
        
        for (int col = 0; col < kMaxNumber; ++col) {
            struct SudokuCell *cells = CellsFromColumn(board_ptr, col);
            struct LineClusterGroups clusters = FindLineClusters(cells);
            FilterCellsByClusters(cells, &clusters);
            
            SetCellsOnColumn(board_ptr, cells, col);
            free(cells);
        }
        
        for (int square_num = 0; square_num < kMaxNumber; ++square_num) {
            struct SudokuCell *cells = CellsFromSquare(board_ptr, square_num);
            struct LineClusterGroups clusters = FindLineClusters(cells);
            FilterCellsByClusters(cells, &clusters);
            
            SetCellsOnSquare(board_ptr, cells, square_num);
            free(cells);
        }
        checksum = BoardCheckSum(board_ptr);
    } while (checksum != last_checksum);
}


/**
 Given an array of kMaxNumber cells, overwrite the specified row in the given
 sudoku board with those.
 
 Cell i is assigned to board[row][i].

 @param board_ptr Pointer to sudoku board. The board will be modified.
 @param cells Array of kMaxNumber cells
 @param row Row of sudoku board (0, ..., kMaxNumber - 1)
 */
void SetCellsOnRow(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber], int row) {
    for (int col = 0; col < kMaxNumber; ++col)
        board_ptr->cells[row][col].possibles = cells[col].possibles;
}

/**
 Given an array of kMaxNumber cells, overwrite the specified column in the given
 sudoku board with those.
 
 Cell i is assigned to board[i][col].
 
 @param board_ptr Pointer to sudoku board. The board will be modified.
 @param cells Array of kMaxNumber cells
 @param col Column of sudoku board (0, ..., kMaxNumber - 1)
 */
void SetCellsOnColumn(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber], int col) {
    for (int row = 0; row < kMaxNumber; ++row)
        board_ptr->cells[row][col] = cells[row];
}


/**
 Assign to the square identified by square_num the specified cells. Squares are numbered 0, ..., kMaxNumber - 1
 starting from row = 0, col = 0 and proceeding with incrementing first row and then col (till 
 row = kMaxNumber - 1 and col = kMaxNumber - 1).

 @param board_ptr Pointer to suduko board. Will be modified.
 @param cells Array of cells to set on the sudoku board.
 @param square_num The square to affect on the sudoku board.
 */
void SetCellsOnSquare(struct SudokuBoard *board_ptr, struct SudokuCell const cells[kMaxNumber], int square_num) {
    int base_row = (square_num / kCellsPerSide) * kCellsPerSide;
    int base_col = (square_num % kCellsPerSide) * kCellsPerSide;
    
    int row, col;
    for (int cell_index = 0; cell_index < kMaxNumber; ++cell_index) {
        row = base_row + (cell_index / kCellsPerSide);
        col = base_col + (cell_index % kCellsPerSide);
        
        board_ptr->cells[row][col] = cells[cell_index];
    }
}


/**
 Identify and return the clusters in the array of cells inputted. E.g., 
 if cell 0 and 1 can only have the values 3 and 5 each, then the cluster
 (3,5) is assigned to the cells [0, 1].

 @param cells Array of kMaxNumber cells.
 @return struct LineClusterGroups specifying clusters of possible values on 
    the specified cells.
 */
struct LineClusterGroups FindLineClusters(struct SudokuCell const cells[kMaxNumber]) {
    struct LineClusterGroups groups;
    groups.clusters = malloc(kMaxNumber * sizeof(struct LineCluster)); // Generally an overallocation
    
    int group_count = 0;
    bool is_in_cluster[kMaxNumber];
    SetBooleanArray(is_in_cluster, kMaxNumber, false);
    int num_in_cluster = 0;
    
    int bits_set;
    int index;

    for (int group_size = 1; group_size <= 3; ++group_size) {
        // At this point we should group_size >= kMaxNumber - num_in_cluster.
        // Otherwise there is no possible cluster of size group_size obviously.
        // We could break here or the following code could/should execute trivially.
        if (group_size >= kMaxNumber - num_in_cluster)
            break;

        // We use only the set of indices not yet assigned to a cluster. That will a limited set 0, ..., k
        // called apparent indices. These will be mapped to the real index using an int array, apparent_to_real_index.
        int num_apparent_indices = kMaxNumber - num_in_cluster;
        int apparent_to_real_index[num_apparent_indices];
        int real_index = 0;
        int apparent_index = 0;
        while (real_index < kMaxNumber) {
            while (is_in_cluster[real_index]) {
                ++real_index;
            }
            
            apparent_to_real_index[apparent_index++] = real_index++;
        }
        
        // keep track of the apparent indices for the group
        int apparent_indices[group_size];
        for (index = 0; index < group_size; ++index)
            apparent_indices[index] = index;
        int last_index = group_size - 1; // keeps track of the index we increment
        
        while (true) {
            bits_set = 0; // The total bits of the possible cluster (given the apparent indices)
            for (index = 0; index < group_size; ++index) {
                bits_set |= cells[apparent_to_real_index[apparent_indices[index]]].possibles;
            }
            if (BitCount(bits_set) == group_size) {
                int *positions = malloc(group_size * sizeof(int)); // use calloc?
                
                for (index = 0; index < group_size; ++index) {
                    int real_index = apparent_to_real_index[apparent_indices[index]];
                    
                    positions[index] = real_index;
                    is_in_cluster[real_index] = true;
                    ++num_in_cluster;
                }
                
                struct LineCluster cluster;
                cluster.size = group_size;
                cluster.positions = positions;
                cluster.bitset = bits_set;
                
                groups.clusters[group_count++] = cluster;
            }
            
            // increment indices
            while (last_index != -1) {
                // the highest number of apparent_indices[last_index] should the highest number
                // of an apparent index len(apparent_to_real_index)
                // = kMaxNumber - num_in_cluster adjusted for the position
                // since each subsequent apparent index must increase by (at least) 1.
                // That is, we must subtract further (#apparent indices - 1) - last_index
                // = group_size - 1 - last_index.
                if (apparent_indices[last_index] < num_apparent_indices - 1 - (group_size - 1 - last_index)) {
                    // increment the last_index position
                    ++apparent_indices[last_index];
                    // and set the remaining to follow that
                    for (index = last_index + 1; index < group_size; ++index)
                        apparent_indices[index] = apparent_indices[last_index] + (index - last_index);

                    // next time we want to increment the end index
                    last_index = group_size - 1;
                    break;
                }
                else {
                    --last_index;
                }
            }
            
            if (last_index == -1) {
                break;
            }
        }
    }
    
    groups.size = group_count;

    return groups;
}
