#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_set>
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>

using namespace std;

// Directions for moving the empty tile (up, down, left, right)
const vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const vector<string> directionNames = {"UP", "DOWN", "LEFT", "RIGHT"};
const vector<vector<int>> goalState = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};

// Struct for representing the puzzle state
struct State {
    vector<vector<int>> puzzle;
    int x, y;  // Position of the empty tile
    string path;  // Path to reach this state

    // Define comparison operator for unordered_set
    bool operator==(const State& other) const {
        return puzzle == other.puzzle;
    }
};

// Hash function for State
struct Hash {
    size_t operator()(const State& state) const {
        size_t hashValue = 0;
        for (const auto& row : state.puzzle) {
            for (int val : row) {
                hashValue = hashValue * 31 + val;
            }
        }
        return hashValue;
    }
};

// Utility function to print a puzzle
void printPuzzle(const vector<vector<int>>& puzzle) {
    for (const auto& row : puzzle) {
        for (int val : row) {
            cout << val << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Check if a puzzle is solvable
bool isSolvable(const vector<vector<int>>& puzzle) {
    vector<int> flat;
    for (const auto& row : puzzle) {
        for (int num : row) {
            if (num != 0) {
                flat.push_back(num);
            }
        }
    }
    
    int inversions = 0;
    for (int i = 0; i < flat.size(); ++i) {
        for (int j = i + 1; j < flat.size(); ++j) {
            if (flat[i] > flat[j]) {
                ++inversions;
            }
        }
    }
    
    // For 3x3 puzzle, the number of inversions must be even for it to be solvable
    return inversions % 2 == 0;
}

// Check if a state is the goal state
bool isGoal(const vector<vector<int>>& puzzle) {
    return puzzle == goalState;
}

// Generate a random initial state
State generateRandomState() {
    vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    random_shuffle(numbers.begin(), numbers.end());

    vector<vector<int>> puzzle(3, vector<int>(3));
    int emptyX, emptyY;
    for (int i = 0, k = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j, ++k) {
            puzzle[i][j] = numbers[k];
            if (puzzle[i][j] == 0) {
                emptyX = i;
                emptyY = j;
            }
        }
    }

    return {puzzle, emptyX, emptyY, ""};
}

// Perform Iterative Deepening Search
bool depthLimitedSearch(State currentState, int limit, unordered_set<State, Hash>& visited, string& resultPath) {
    if (isGoal(currentState.puzzle)) {
        resultPath = currentState.path;
        return true;
    }
    
    if (limit == 0) return false;

    visited.insert(currentState);

    for (int i = 0; i < 4; ++i) {
        int newX = currentState.x + directions[i].first;
        int newY = currentState.y + directions[i].second;

        if (newX >= 0 && newX < 3 && newY >= 0 && newY < 3) {
            // Move the empty tile
            swap(currentState.puzzle[currentState.x][currentState.y], currentState.puzzle[newX][newY]);
            State nextState = {currentState.puzzle, newX, newY, currentState.path + directionNames[i] + " "};

            if (visited.find(nextState) == visited.end()) {
                if (depthLimitedSearch(nextState, limit - 1, visited, resultPath)) {
                    return true;
                }
            }
            // Undo the move
            swap(currentState.puzzle[currentState.x][currentState.y], currentState.puzzle[newX][newY]);
        }
    }

    visited.erase(currentState);
    return false;
}

// Iterative Deepening Search to solve the 8-tile puzzle
string iterativeDeepeningSearch(State initialState) {
    int depth = 0;
    string resultPath;

    while (true) {
        unordered_set<State, Hash> visited;
        if (depthLimitedSearch(initialState, depth, visited, resultPath)) {
            return resultPath;
        }
        ++depth;
    }
}

// Apply a move to the puzzle and print the state after the move
void applyMove(State& state, const string& move) {
    int dirIndex = find(directionNames.begin(), directionNames.end(), move) - directionNames.begin();
    if (dirIndex < 0 || dirIndex >= 4) return;  // Invalid move
    
    int newX = state.x + directions[dirIndex].first;
    int newY = state.y + directions[dirIndex].second;

    // Move the empty tile and print the puzzle after the move
    if (newX >= 0 && newX < 3 && newY >= 0 && newY < 3) {
        swap(state.puzzle[state.x][state.y], state.puzzle[newX][newY]);
        state.x = newX;
        state.y = newY;
        cout << "Move: " << move << endl;
        printPuzzle(state.puzzle);
    }
}

int main() {
    srand(time(0));

    State initialState;
    do {
        initialState = generateRandomState();
    } while (!isSolvable(initialState.puzzle));

    cout << "Initial State:" << endl;
    printPuzzle(initialState.puzzle);

    string solution = iterativeDeepeningSearch(initialState);

    if (!solution.empty()) {
        cout << "Solution found!" << endl;
        cout << "Sequence of moves: " << solution << endl;

        // Apply each move and print the matrix
        State currentState = initialState;
        size_t pos = 0;
        while ((pos = solution.find(' ')) != string::npos) {
            string move = solution.substr(0, pos);
            solution.erase(0, pos + 1);
            applyMove(currentState, move);
        }
    } else {
        cout << "No solution found." << endl;
    }

    return 0;
}
