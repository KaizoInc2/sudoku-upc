#include <iostream>
#include <vector>

using namespace std;

typedef vector< vector<int> > Matrix;

// Function that returns true if the vector contains the element specified, otherwise returns false
bool includes(const vector<int>& values, int value) {
    bool found = false;
    int iterator = 0;
    // Loops until the element is found or reaches the end of the vector
    while (!found && iterator < (int)values.size()) {
        if (values[iterator] == value) found = true;
        ++iterator;
    }
    return found;
}

// Creates the sudoku from the user input and returns it as a Matrix data type
Matrix createSudoku() {
    Matrix sudoku = {};
    for (int i = 0; i < 9; ++i) {
        sudoku.push_back({});
        for (int j = 0; j < 9; ++j) {
            int val;
            cin >> val;
            sudoku[i].push_back(val);
        }
    }
    return sudoku;
}

// Creates the initial values sudoku (the values that originally existed are 1 and the ones that where initially empty are 0)
// This function must be called inmediatelly after the sudoku creation
// This is useful to know which values we can change or not because it were added by the user at the beginning
Matrix createInitialSudoku(const Matrix& sudoku) {
    Matrix initialSudoku = {};
    for (int i = 0; i < 9; ++i) {
        initialSudoku.push_back({});
        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) initialSudoku[i].push_back(0);
            else initialSudoku[i].push_back(1);
        }
    }
    return initialSudoku;
}

// Prints the sudoku in the desired structure
void printSudoku(const Matrix& sudoku) {
    cout << "   A B C   D E F   G H I" << endl;
    for (int i = 0; i < 9; ++i) {
        cout << i+1 << "  ";
        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) cout << ". ";
            else cout << sudoku[i][j] << " ";
            if (j == 2 || j == 5) cout << "| ";
        }
        cout << endl;
        if (i == 2 || i == 5) cout << "  -------+-------+-------" << endl;
    }
}

// Gets the possible values on the sudoku in a given row and column, if there is already a value in that position it returns a empty vector
vector<int> getPossibleValues(const Matrix& sudoku, const Matrix& initialSudoku, int row, int col) {
    if (initialSudoku[row][col] != 0) return {}; // If the value is a initial one it can not be changed
    int values[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    // Check for rows and columns at the same time
    for (int i = 0; i < 9; ++i) {
        if (sudoku[row][i] != 0 && i != col) {
            --values[sudoku[row][i]-1];
        }
        if (sudoku[i][col] != 0 && i != row) {
            --values[sudoku[i][col]-1];
        }
    }

    // Check for the region of its position
    vector<int> subMatrix = {row/3, col/3};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sudoku[i + (subMatrix[0]*3)][j + (subMatrix[1]*3)] != 0) {
                --values[sudoku[i + (subMatrix[0]*3)][j + (subMatrix[1]*3)]-1];
            }
        }
    }

    // By looking at the values vector all the values equal to 1 are possible values
    vector<int> possible;
    for (int i = 0; i < 9; ++i) {
        if (values[i] == 1) possible.push_back(i+1);
    }
    return possible;
}

// Changes a user input value in the sudoku, if it can not be done it displays it on the console
void changeValue(Matrix& sudoku, const Matrix& initialSudoku, int row, int col, int input_row, char input_col, int value) {
    if (initialSudoku[row][col] != 0) cout << input_row << input_col << ": Casella no modificable" << endl;
    else if (!includes(getPossibleValues(sudoku, initialSudoku, row, col), value)) cout << input_row << input_col << ": " << value << " es un valor no possible" << endl;
    else sudoku[row][col] = value;
}

// Checks if a value is obligatory in a position of the sudoku
bool isObligatory(const Matrix& sudoku, const Matrix& initialSudoku, int row, int col, int value) {
    bool obligatory = true;
    int iterator = 0;

    // First checks for the elements on the same row
    while (obligatory && iterator < 9) {
        if (sudoku[row][iterator] == 0 && iterator != col && includes(getPossibleValues(sudoku, initialSudoku, row, iterator), value)) obligatory = false;
        ++iterator;
    }
    if (obligatory) return true;

    // If its not obligatory starts again but this time for the columns

    obligatory = true;
    iterator = 0;
    while (obligatory && iterator < 9) {
        if (sudoku[iterator][col] == 0 && iterator != row && includes(getPossibleValues(sudoku, initialSudoku, iterator, col), value)) obligatory = false;
        ++iterator;
    }
    if (obligatory) return true;

    // And if its not obligatory on the rows and columns searches on the region

    obligatory = true;
    vector<int> subMatrix = {row/3, col/3};
    for (int i = 0; i < 3 && obligatory; ++i) {
        for (int j = 0; j < 3 && obligatory; ++j) {
            int posX = i + (subMatrix[0]*3);
            int posY = j + (subMatrix[1]*3);
            if (sudoku[posX][posY] == 0 && (posX != row || posY != col) && includes(getPossibleValues(sudoku, initialSudoku, posX, posY), value)) obligatory = false;
        }
    }

    return obligatory;
}

// Solves the sudoku by using all the previous functions
void solveSudoku(Matrix& sudoku, const Matrix& initialSudoku) {
    bool solved = false;
    while (!solved) { // If the sudoku is not solved iterates through all the positions trying to solve it
        bool changed = false;
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (sudoku[i][j] == 0) {
                    vector<int> values = getPossibleValues(sudoku, initialSudoku, i, j);

                    // If there is only one possible value it's changed and printed to the console
                    if (values.size() == 1) {
                        sudoku[i][j] = values[0];
                        cout << "A la casella (" << i+1 << "," << char('A'+j) << ") hi ha d'anar un " << values[0] << endl;
                        changed = true;
                    }

                    // If there is more than one possible value iterates through them until there is one that is obligatory or there are no more values
                    else if (values.size() > 1) {
                        for (int k = 0; k < (int)values.size(); ++k) {
                            if (isObligatory(sudoku, initialSudoku, i, j, values[k])) {
                                sudoku[i][j] = values[k];
                                cout << "A la casella (" << i+1 << "," << char('A'+j) << ") hi ha d'anar un " << values[k] << endl;
                                changed = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        printSudoku(sudoku);
        cout << endl;
        if (!changed) solved = true; // When nothing has been changed the sudoku is considered solved
    }
}

int main() {
    Matrix sudoku = createSudoku();
    Matrix initialSudoku = createInitialSudoku(sudoku);
    char instruction;
    bool endProgram = false;
    while (!endProgram && cin >> instruction) {

        if (instruction == 'Z') endProgram = true;

        else if (instruction == 'C') printSudoku(sudoku);

        else if (instruction == 'R') solveSudoku(sudoku, initialSudoku);

        else if (instruction == 'A' || instruction == 'B') {
            int input_row;
            char input_col;
            cin >> input_row >> input_col;
            // Transforms the input to one the program can understand (integers for the indexes)
            int row = input_row - 1;
            int col = input_col - 'A';

            if (instruction == 'A') {
                cout << input_row << input_col << ": ";
                vector<int> possible = getPossibleValues(sudoku, initialSudoku, row, col);
                cout << "[";
                for (int i = 0; i < (int)possible.size(); ++i) {
                    if (i != 0) cout << ", ";
                    cout << possible[i];
                }
                cout << "]" << endl;
            }

            else if (instruction == 'B') {
                int value;
                cin >> value;
                changeValue(sudoku, initialSudoku, row, col, input_row, input_col, value); // The 'B' instruction also needs the raw user input for rows and columns in order to display the errors
            }
        }
    }
    return 0;
}