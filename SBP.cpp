/* Author: Saumya Saxena
CS510: Introduction to AI - Assignment 1
Date: October 17, 2018
Description: Sliding Brick Puzzle
*/

/* Header Files */
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stdlib.h>
#include <queue>
#include <stack>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Class that reads input from the file and creates a sliding block matrix
class SlidingBrick
{
    int w, h;

public:

    vector<vector<int> > puzzleMatrix;

    void setW(int w)
    {
        this->w = w;
    }

    int getW()
    {
        return w;
    }

    void setH(int h)
    {
        this->h = h;
    }

    int getH()
    {
        return h;
    }

    //Funtion to read input from file
    void readInputFile(string fileName)
    {
        int rowNumber = 0;
        int colNumber = 0;
        ifstream in;
        in.open(fileName);

        int x;
        int readW, readH;
        in >> readW;
        in.ignore();
        in >> readH;
        in.ignore();

        setW(readW);
        setH(readH);

        puzzleMatrix.resize(getH());

        for (int i = 0; i < getH(); i++)
        {
            puzzleMatrix[i].resize(getW());
        }

        int j = 0;
        while ((in >> x) && in.ignore()) //read number, ignore comma, repeat
        {
            puzzleMatrix[rowNumber][colNumber] = x;

            if (colNumber < getW() - 1)
            {
                colNumber++;
            }
            else
            {
                colNumber = 0;
                rowNumber++;
            }
        }
    }
};

//Structure to store piece number and direction
struct PieceMovement
{
    int piece;
    int direction;
};

//Class to generate nodes for BFS, DFS and IDS
class Node
{
public:
    vector<vector<int>> state;
    Node *parent;
    struct PieceMovement move;
};

//Class defining movement specific funtiona for a matrix
class PieceMove
{
public:
    //Initialize left -> 0 , right -> 1 , up -> 2 , down ->  3
    int movement[4];
    int sRow = -1, sCol = -1, eRow = -1, eCol = -1;
    vector<vector<int> > allPieceMovementVector;

    map<int, vector<int>> allPieceMovement;
    map<int, vector<int>> sizeOfAllPieces;

    char direction;
    //Function to find the size of a piece and store in a map
    void sizeOfPiece(vector<vector<int> > puzzleMatrix, int piece)
    {
        int totalMoves = 4;
        for (int i = 0; i<puzzleMatrix.size(); i++)
        {
            for (int j = 0; j<puzzleMatrix[i].size(); j++)
            {
                if (puzzleMatrix[i][j] == piece)
                {
                    if (sRow == -1)
                    {
                        sRow = i;
                        sCol = j;
                    }
                    if (sRow != i && sRow != -1)
                    {
                        eRow = i;
                    }
                    if (sCol != j && sCol != -1)
                    {
                        eCol = j;
                    }
                }
            }
        }
        sizeOfAllPieces[piece].resize(totalMoves);
        sizeOfAllPieces[piece][0] = sRow;
        sizeOfAllPieces[piece][1] = sCol;
        sizeOfAllPieces[piece][2] = eRow;
        sizeOfAllPieces[piece][3] = eCol;
        //cout << "Size of piece:" << piece << ">>" << sRow << " " << sCol << " " << eRow << " " << eCol << "\n";
    }

    //Function to print the puzzle matrix in the given format
    void printPuzzleMatrix(vector<vector<int>> puzzleMatrixPrint)
    {
        cout << puzzleMatrixPrint[0].size() << "," << puzzleMatrixPrint.size() << "," << "\n";
        for (int i = 0; i < puzzleMatrixPrint.size(); i++)
        {
            for (int j = 0; j < puzzleMatrixPrint[i].size(); j++)
            {
                cout << puzzleMatrixPrint[i][j] << ",";
            }
            cout << "\n";
        }
    }

    //Function to check if the puzzle is solved or not
    bool isPuzzleSolved(vector<vector<int>> puzzleMatrix)
    {
        for (int i = 0; i < puzzleMatrix.size(); i++)
        {
            for (int j = 0; j < puzzleMatrix[i].size(); j++)
            {
                if (puzzleMatrix[i][j] == -1)
                {
                    return false;
                }
            }
        }
        return true;
    }

    //Function to create another copy of the matrix
    vector<vector<int>> cloneMatrix(vector<vector<int>> puzzleMatrix)
    {
        return puzzleMatrix;
    }

    //Function to check if two states are identical or not (1D)
    bool compareStates(vector<vector<int>> puzzleMatrixState1, vector<vector<int>> puzzleMatrixState2)
    {
        for (int i = 0; i < puzzleMatrixState2.size(); i++)
        {
            for (int j = 0; j < puzzleMatrixState2[i].size(); j++)
            {
                if (puzzleMatrixState1[i][j] == puzzleMatrixState2[i][j])
                {
                    continue;
                }
                else return 0;
            }
        }
        return 1;
    }

    //Function to swap the piece in a matrix
    vector<vector<int>> swapPieces(int piece1, int piece2, vector<vector<int>> swapPuzzleMatrix)
    {
        for (int i = 0; i < swapPuzzleMatrix.size(); i++)
        {
            for (int j = 0; j < swapPuzzleMatrix[i].size(); j++)
            {
                if (swapPuzzleMatrix[i][j] == piece1)
                {
                    swapPuzzleMatrix[i][j] = piece2;
                }
                else if (swapPuzzleMatrix[i][j] == piece2)
                {
                    swapPuzzleMatrix[i][j] = piece1;
                }
            }
        }
        return swapPuzzleMatrix;

    }

    //Function to list all possible moves of a piece
    int *listOfAllMoves(vector<vector<int> > puzzleMatrix, int piece)
    {
        sizeOfPiece(puzzleMatrix, piece);
        // movement[4] = { 0 };
        if (sRow == -1 && sCol == -1)
        {
            cout << "Piece does not exist in the Puzzle\n";
            return 0;
        }
        //printPuzzleMatrix(puzzleMatrix);

        //if piece size greater than or equal to 1x1
        if (eRow == -1 || sRow == eRow)
        {
            //Left right up down (Single Column) i.e piec size is equal to 1x1
            if (eCol == -1 || eCol == sCol)
            {
                if (puzzleMatrix[sRow][sCol - 1] == 0 || (puzzleMatrix[sRow][sCol - 1] == -1 && piece == 2))
                {
                    movement[0] = 1;
                }

                if (puzzleMatrix[sRow][sCol + 1] == 0 || (puzzleMatrix[sRow][sCol + 1] == -1 && piece == 2))
                {
                    movement[1] = 1;
                }

                if (sRow != 0)
                {
                    if (puzzleMatrix[sRow - 1][sCol] == 0 || (puzzleMatrix[sRow - 1][sCol] == -1 && piece == 2))
                    {
                        movement[2] = 1;
                    }
                }
                if (puzzleMatrix[sRow + 1][sCol] == 0 || (puzzleMatrix[sRow + 1][sCol] == -1 && piece == 2))
                {
                    movement[3] = 1;
                }
            }
            //Up Down left right i.e piece size is greater than or equal to 1x2
            else if (eCol != -1)
            {
                int flagUp = 1, flagDown = 1;
                for (int i = sCol; i <= eCol; i++)
                {
                    if (puzzleMatrix[sRow - 1][i] != 0)
                    {
                        flagUp = 0;
                    }
                    if ((puzzleMatrix[sRow + 1][i] != 0))
                    {
                        flagDown = 0;
                    }
                    if (puzzleMatrix[sRow - 1][i] == -1 && piece == 2)
                        flagUp = 1;

                    if (puzzleMatrix[sRow + 1][i] == -1 && piece == 2)
                    {
                        flagDown = 1;
                    }
                    if (flagUp == 0 && flagDown == 0)
                        break;
                }
                if (puzzleMatrix[sRow][sCol - 1] == 0 || (puzzleMatrix[sRow][sCol - 1] == -1 && piece == 2))
                {
                    movement[0] = 1;
                }
                if (puzzleMatrix[sRow][eCol + 1] == 0 || (puzzleMatrix[sRow][eCol + 1] == -1 && piece == 2))
                {
                    movement[1] = 1;
                }
                if (flagUp == 1)
                    movement[2] = 1;
                if (flagDown == 1)
                    movement[3] = 1;
            }
        }

        else if (eRow != -1)
        {
            //if piece size greater than or equal to 2x1
            if (eCol == -1 || eCol == sCol)
            {
                int flagRight = 1, flagLeft = 1;

                if (sRow != 0)
                {
                    if (puzzleMatrix[sRow - 1][sCol] == 0 || (puzzleMatrix[sRow - 1][sCol] == -1 && piece == 2))
                    {
                        movement[2] = 1;
                    }
                }
                if (puzzleMatrix[eRow + 1][sCol] == 0 || (puzzleMatrix[eRow + 1][sCol] == -1 && piece == 2))
                {
                    movement[3] = 1;
                }
                for (int i = sRow; i <= eRow; i++)
                {
                    if (puzzleMatrix[i][sCol - 1] != 0)
                    {
                        flagLeft = 0;
                    }
                    if (puzzleMatrix[i][sCol + 1] != 0)
                    {
                        flagRight = 0;
                    }
                    if (puzzleMatrix[i][sCol - 1] != -1 && piece == 2)
                    {
                        flagLeft = 0;
                    }
                    if (puzzleMatrix[i][sCol + 1] != -1 && piece == 2)
                    {
                        flagRight = 0;
                    }
                }

                if (flagLeft == 1)
                    movement[0] = 1;
                if (flagRight == 2)
                    movement[1] = 1;

            }
            //if piece size greater than or equal to 2x2
            else if (eCol != -1)
            {
                int flagUp = 1, flagDown = 1;
                int flagRight = 1, flagLeft = 1;
                for (int i = sCol; i <= eCol; i++)
                {
                    if (sRow != 0)
                    {
                        if (puzzleMatrix[sRow - 1][i] != 0 && puzzleMatrix[sRow - 1][i] != -1)
                        {
                            flagUp = 0;
                        }
                    }
                    if (puzzleMatrix[eRow + 1][i] != 0 && puzzleMatrix[eRow + 1][i] != -1)
                    {
                        flagDown = 0;
                    }

                }

                for (int i = sRow; i <= eRow; i++)
                {
                    if (puzzleMatrix[i][sCol - 1] != 0 && puzzleMatrix[i][sCol - 1] != -1)
                    {
                        flagLeft = 0;
                    }
                    if (puzzleMatrix[i][eCol + 1] != 0 && puzzleMatrix[i][eCol + 1] != -1)
                    {
                        flagRight = 0;
                    }
                }

                if (flagLeft == 1)
                    movement[0] = 1;
                if (flagRight == 2)
                    movement[1] = 1;
                if (flagUp == 1)
                    movement[2] = 1;
                if (flagDown == 1)
                    movement[3] = 1;
            }
        }
        // printPossibleMoves(piece);
        return movement;
    }

    //Function to print all possible moves of a piece
    void printPossibleMoves(int piece)
    {
        cout << "Possible moves for the piece " << piece << ": \n";
        if (movement[0] == 1){
            cout << "( " << piece << ", left )\n";
        }
        else
        {
            movement[0] = 0;
        }
        if (movement[1] == 1){
            cout << "( " << piece << " , right )\n";
        }
        else
        {
            movement[1] = 0;
        }
        if (movement[2] == 1){
            cout << "( " << piece << " , up )\n";
        }
        else
        {
            movement[2] = 0;
        }
        if (movement[3] == 1){
            cout << "( " << piece << " , down )\n";
        }
        else
        {
            movement[3] = 0;
        }
    }

    //Function to print all possible moves for all the pieces
    void printUnionOfAllMoves()
    {
        cout << "print union of all moves:\n";
        for (int i = 0; i < allPieceMovement.size(); i++)
        {
            if (allPieceMovement[i].size() != 0)
            {
                if (allPieceMovement[i][0] == 1){
                    cout << "( " << i << ", left )\n";
                }
                else
                {
                    allPieceMovement[i][0] = 0;
                }
                if (allPieceMovement[i][1] == 1){
                    cout << "( " << i << " , right )\n";
                }
                else
                {
                    allPieceMovement[i][1] = 0;
                }
                if (allPieceMovement[i][2] == 1){
                    cout << "( " << i << " , up )\n";
                }
                else
                {
                    allPieceMovement[i][2] = 0;
                }
                if (allPieceMovement[i][3] == 1){
                    cout << "( " << i << " , down )\n";
                }
                else
                {
                    allPieceMovement[i][3] = 0;
                }
            }
        }
    }

    //Function to return all moves in a PieceMovement vector
    vector <PieceMovement> allMovesList(vector<vector<int>> puzzleMatrix)
    {
        vector<PieceMovement> moves;
        printAllPossibleMoves(puzzleMatrix);
        //printUnionOfAllMoves();
        //cout << "All moves lists:\n";
        for (int i = 0; i < allPieceMovement.size(); i++)
        {
            if (allPieceMovement[i].size() != 0)
            {

                if (allPieceMovement[i][0] == 1){
                    PieceMovement pieceMove;
                    pieceMove.piece = i;
                    pieceMove.direction = 0;
                    moves.push_back(pieceMove);
                    //cout << "( " << i << ", left )\n";
                }
                else
                {
                    allPieceMovement[i][0] = 0;
                }
                if (allPieceMovement[i][1] == 1){
                    PieceMovement pieceMove;
                    pieceMove.piece = i;
                    pieceMove.direction = 1;
                    moves.push_back(pieceMove);
                    //cout << "( " << i << " , right )\n";
                }
                else
                {
                    allPieceMovement[i][1] = 0;
                }
                if (allPieceMovement[i][2] == 1){
                    PieceMovement pieceMove;
                    pieceMove.piece = i;
                    pieceMove.direction = 2;
                    moves.push_back(pieceMove);
                    //cout << "( " << i << " , up )\n";
                }
                else
                {
                    allPieceMovement[i][2] = 0;
                }
                if (allPieceMovement[i][3] == 1){
                    PieceMovement pieceMove;
                    pieceMove.piece = i;
                    pieceMove.direction = 3;
                    moves.push_back(pieceMove);
                    // cout << "( " << i << " , down )\n";
                }
                else
                {
                    allPieceMovement[i][3] = 0;
                }
            }
        }
        return moves;
    }

    /*
    bool checkIfNextMoveIsGoal(vector<vector<int> > puzzleMatrix)
    {
        int *movementOfAPiece;
        movementOfAPiece = listOfAllMoves(puzzleMatrix, 2);


        if (movementOfAPiece[0] == 1 || movementOfAPiece[1] == 1 || movementOfAPiece[2] == 1 || movementOfAPiece[3] == 1)
        {
            return true;
        }
        else
            return false;

    }
    */

    //Function to evaluate all possible moves in a matrix
    void printAllPossibleMoves(vector<vector<int> > puzzleMatrix)
    {
        int totalMoves = 4;

        map<int, int> movementSuccess;
        for (int i = 0; i<puzzleMatrix.size(); i++)
        {
            for (int j = 0; j<puzzleMatrix[i].size(); j++)
            {
                if (puzzleMatrix[i][j] != 1 && puzzleMatrix[i][j] != 0 && puzzleMatrix[i][j] != -1 && movementSuccess[puzzleMatrix[i][j]] == 0)
                {
                    allPieceMovement[puzzleMatrix[i][j]].resize(totalMoves);

                    int *movementOfAPiece;
                    movementOfAPiece = listOfAllMoves(puzzleMatrix, puzzleMatrix[i][j]);

                    allPieceMovement[puzzleMatrix[i][j]][0] = movementOfAPiece[0];
                    allPieceMovement[puzzleMatrix[i][j]][1] = movementOfAPiece[1];
                    allPieceMovement[puzzleMatrix[i][j]][2] = movementOfAPiece[2];
                    allPieceMovement[puzzleMatrix[i][j]][3] = movementOfAPiece[3];

                    movementOfAPiece[0] = 0;
                    movementOfAPiece[1] = 0;
                    movementOfAPiece[2] = 0;
                    movementOfAPiece[3] = 0;

                    sRow = -1;
                    sCol = -1;
                    eRow = -1;
                    eCol = -1;

                    movementSuccess[puzzleMatrix[i][j]] = 1;
                }
            }
        }
        //printPuzzleMatrix(puzzleMatrix);
        // cout << "\n";
        // printUnionOfAllMoves();
    }

    //Function to change the state of the matrix after applying move to a piece
    vector<vector<int> > applyMove(vector<vector<int> > puzzleMatrix, int piece, string movement)
    {
        int move;
        if (movement == "left")
        {
            move = 0;
        }
        else if (movement == "right")
        {
            move = 1;
        }
        else if (movement == "up")
        {
            move = 2;
        }
        else if (movement == "down")
        {
            move = 3;
        }

        if (allPieceMovement[piece][move] == 1)
        {
            //cout << "movement is possible\n";

            int sRow = sizeOfAllPieces[piece][0];
            int sCol = sizeOfAllPieces[piece][1];
            int eRow = sizeOfAllPieces[piece][2];
            int eCol = sizeOfAllPieces[piece][3];

            if (eRow == -1)
            {
                //1x1
                if (eCol == -1)
                {
                    puzzleMatrix[sRow][sCol] = 0;
                    if (move == 0)
                    {
                        puzzleMatrix[sRow][sCol - 1] = piece;
                    }
                    if (move == 1)
                    {
                        puzzleMatrix[sRow][sCol + 1] = piece;
                    }
                    if (move == 2)
                    {
                        puzzleMatrix[sRow - 1][sCol] = piece;
                    }
                    if (move == 3)
                    {
                        puzzleMatrix[sRow + 1][sCol] = piece;
                    }
                }
                //1x2
                else
                {
                    if (move == 0)
                    {
                        puzzleMatrix[sRow][eCol] = 0;
                        puzzleMatrix[sRow][sCol - 1] = piece;
                    }
                    if (move == 1)
                    {
                        puzzleMatrix[sRow][sCol] = 0;
                        puzzleMatrix[sRow][eCol + 1] = piece;
                    }
                    if (move == 2)
                    {
                        for (int i = sCol; i <= eCol; i++)
                        {
                            puzzleMatrix[sRow - 1][i] = piece;
                            puzzleMatrix[sRow][i] = 0;

                        }
                    }
                    if (move == 3)
                    {
                        for (int i = sCol; i <= eCol; i++)
                        {
                            puzzleMatrix[sRow + 1][i] = piece;
                            puzzleMatrix[sRow][i] = 0;

                        }
                    }
                }
            }
            else
            {//2x1
                if (eCol == -1)
                {
                    if (move == 0)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol - 1] = piece;
                            puzzleMatrix[i][sCol] = 0;
                        }
                    }
                    if (move == 1)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol + 1] = piece;
                            puzzleMatrix[i][sCol] = 0;
                        }
                    }
                    if (move == 2)
                    {
                        puzzleMatrix[sRow - 1][sCol] = piece;
                        puzzleMatrix[eRow][sCol] = 0;
                    }
                    if (move == 3)
                    {
                        puzzleMatrix[eRow + 1][sCol] = piece;
                        puzzleMatrix[sRow][sCol] = 0;
                    }
                }
                //>=2x2
                else
                {

                    if (move == 0)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol - 1] = piece;
                            puzzleMatrix[i][eCol] = 0;
                        }
                    }
                    if (move == 1)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][eCol + 1] = piece;
                            puzzleMatrix[i][sCol - 1] = 0;
                        }
                    }
                    if (move == 2)
                    {
                        for (int j = sCol; j <= eCol; j++)
                        {
                            puzzleMatrix[sRow - 1][j] = piece;
                            puzzleMatrix[eRow][j] = 0;
                        }
                    }
                    if (move == 3)
                    {
                        for (int j = sCol; j <= eCol; j++)
                        {
                            puzzleMatrix[eRow + 1][j] = piece;
                            puzzleMatrix[sRow][j] = 0;
                        }
                    }

                }
            }

            //printPuzzleMatrix(puzzleMatrix);
        }
        else
        {
            // cout << "movement is not possible\n";
        }
        return puzzleMatrix;
    }

    //Function to transform the matrix into a normal form (1E)
    vector<vector<int>> normalization(vector<vector<int>> normPuzzleMatrix)
    {
        int startPieceNo = 3;

        for (int i = 0; i < normPuzzleMatrix.size(); i++)
        {
            for (int j = 0; j < normPuzzleMatrix[i].size(); j++)
            {
                if (normPuzzleMatrix[i][j] == startPieceNo)
                {
                    startPieceNo++;
                }
                else if (normPuzzleMatrix[i][j] > startPieceNo)
                {
                    normPuzzleMatrix = swapPieces(startPieceNo, normPuzzleMatrix[i][j], normPuzzleMatrix);
                    startPieceNo++;
                }
            }
        }

        return normPuzzleMatrix;

    }

    //Function to apply move to a piece and return changed state of matrix
    vector<vector<int> > applyMoveCloning(vector<vector<int> > puzzleMatrix, int piece, string movement)
    {
        int move;
        //cout << "Applying movement:" << movement.c_str() << " on " << piece << "\n";
        //printPuzzleMatrix(puzzleMatrix);
        if (movement == "left")
        {
            move = 0;
        }
        else if (movement == "right")
        {
            move = 1;
        }
        else if (movement == "up")
        {
            move = 2;
        }
        else if (movement == "down")
        {
            move = 3;
        }
        // cout << "movement-->(" << piece << "," << movement.c_str() << ")\n";

        if (allPieceMovement[piece][move] == 1)
        {
            //cout << "movement is possible\n";

            int sRow = sizeOfAllPieces[piece][0];
            int sCol = sizeOfAllPieces[piece][1];
            int eRow = sizeOfAllPieces[piece][2];
            int eCol = sizeOfAllPieces[piece][3];

            if (eRow == -1)
            {
                //1x1
                if (eCol == -1)
                {
                    puzzleMatrix[sRow][sCol] = 0;
                    if (move == 0)
                    {
                        puzzleMatrix[sRow][sCol - 1] = piece;
                    }
                    if (move == 1)
                    {
                        puzzleMatrix[sRow][sCol + 1] = piece;
                    }
                    if (move == 2)
                    {
                        puzzleMatrix[sRow - 1][sCol] = piece;
                    }
                    if (move == 3)
                    {
                        puzzleMatrix[sRow + 1][sCol] = piece;
                    }
                }
                //1x2
                else
                {
                    if (move == 0)
                    {
                        puzzleMatrix[sRow][eCol] = 0;
                        puzzleMatrix[sRow][sCol - 1] = piece;
                    }
                    if (move == 1)
                    {
                        puzzleMatrix[sRow][sCol] = 0;
                        puzzleMatrix[sRow][eCol + 1] = piece;
                    }
                    if (move == 2)
                    {
                        for (int i = sCol; i <= eCol; i++)
                        {
                            puzzleMatrix[sRow - 1][i] = piece;
                            puzzleMatrix[sRow][i] = 0;

                        }
                    }
                    if (move == 3)
                    {
                        for (int i = sCol; i <= eCol; i++)
                        {
                            puzzleMatrix[sRow + 1][i] = piece;
                            puzzleMatrix[sRow][i] = 0;

                        }
                    }
                }
            }
            else
            {//2x1
                if (eCol == -1)
                {
                    if (move == 0)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol - 1] = piece;
                            puzzleMatrix[i][sCol] = 0;
                        }
                    }
                    if (move == 1)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol + 1] = piece;
                            puzzleMatrix[i][sCol] = 0;
                        }
                    }
                    if (move == 2)
                    {
                        puzzleMatrix[sRow - 1][sCol] = piece;
                        puzzleMatrix[eRow][sCol] = 0;
                    }
                    if (move == 3)
                    {
                        puzzleMatrix[eRow + 1][sCol] = piece;
                        puzzleMatrix[sRow][sCol] = 0;
                    }
                }
                //>=2x2
                else
                {

                    if (move == 0)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][sCol - 1] = piece;
                            puzzleMatrix[i][eCol] = 0;
                        }
                    }
                    if (move == 1)
                    {
                        for (int i = sRow; i <= eRow; i++)
                        {
                            puzzleMatrix[i][eCol + 1] = piece;
                            puzzleMatrix[i][sCol - 1] = 0;
                        }
                    }
                    if (move == 2)
                    {
                        for (int j = sCol; j <= eCol; j++)
                        {
                            puzzleMatrix[sRow - 1][j] = piece;
                            puzzleMatrix[eRow][j] = 0;
                        }
                    }
                    if (move == 3)
                    {
                        for (int j = sCol; j <= eCol; j++)
                        {
                            puzzleMatrix[eRow + 1][j] = piece;
                            puzzleMatrix[sRow][j] = 0;
                        }
                    }

                }
            }
            //printPuzzleMatrix(puzzleMatrix);
        }
        else
        {
            // cout << "movement is not possible\n";
        }
        return puzzleMatrix;
    }

    //Function to apply a random move to a piece, change state of matrix and normalize the matrix
    void randowWalk(vector<vector<int>> puzzleMatrixState, int n)
    {
        bool isSolved = 0;
        string direction;
        cout << "\n";
        printPuzzleMatrix(puzzleMatrixState);

        for (int i = 0; i < n; i++)
        {
            int randomPieceIndex = rand() % allPieceMovement.size();
            int randomDirection = rand() % 4;

            if (randomDirection == 0)
                direction = "left";
            else if (randomDirection == 1)
                direction = "right";
            else if (randomDirection == 2)
                direction = "up";
            else if (randomDirection == 3)
                direction = "down";

            auto it = allPieceMovement.begin();

            for (int i = 0; i < randomPieceIndex && it != allPieceMovement.end(); ++i)
                ++it;

            int randomPiece = it->first;
            if (randomPiece == 1)
            {
            }
            else{
                puzzleMatrixState = applyMoveCloning(puzzleMatrixState, randomPiece, direction);

                puzzleMatrixState = normalization(puzzleMatrixState);

                isSolved = isPuzzleSolved(puzzleMatrixState);
                cout << "\n";
                cout << "(" << randomPiece << "," << direction.c_str() << ")\n\n";
                printPuzzleMatrix(puzzleMatrixState);
                if (isSolved)
                    break;
            }
        }

        if (isSolved)
        {
            cout << "Puzzle is solved in randomw walks";
            printPuzzleMatrix(puzzleMatrixState);
        }

    }

    //Function to implement Breadth First Search
    void BFS(vector<vector<int>> puzzleMatrix)
    {
        high_resolution_clock::time_point start = high_resolution_clock::now();
        queue<Node> bfsQueue;

        map<vector<vector<int>>,int> visitedMap;

        Node rootNode;

        int numberOfNodesExplored = 0;

        rootNode.state = puzzleMatrix;
        rootNode.parent = nullptr;

        rootNode.move.direction = 0;
        rootNode.move.piece = 0;

        bfsQueue.push(rootNode);

        visitedMap[rootNode.state] = 1;

        while (!bfsQueue.empty())
        {
            Node *first = new(Node);
            *first = bfsQueue.front();
            vector<PieceMovement> moveList;
            bfsQueue.pop();

            bool isSolved = isPuzzleSolved(first->state);
            //printPuzzleMatrix(first->state);

            if (isSolved)
            {
                printPuzzleMatrix(puzzleMatrix);

                Node finalnode = *first;
                Node temp = *first;

                vector<Node> stateList;

                bool compareState;
                string direction;

                do {
                    stateList.push_back(temp);
                    if (temp.parent == NULL)
                        break;
                    temp = *(temp.parent);
                    compareState = compareStates(temp.state, rootNode.state);
                } while (&temp != NULL);

                for (int i = stateList.size() - 1; i >= 0; i--)
                {
                    if (stateList[i].move.piece != 0)
                    {
                        if (stateList[i].move.direction == 0)
                            direction = "left";
                        else if (stateList[i].move.direction == 1)
                            direction = "right";
                        else if (stateList[i].move.direction == 2)
                            direction = "up";
                        else
                            direction = "down";
                        cout << "\n(" << stateList[i].move.piece << "," << direction.c_str() << ")" << "\n\n";
                        printPuzzleMatrix(stateList[i].state);
                    }
                }
                cout << "\n";

                // cout << "Solved puzzle state:";
                //  printPuzzleMatrix(finalnode.state);

                high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
                microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

                float microToSeconds = 1000000;

                cout << numberOfNodesExplored << " " << double(duration.count() / microToSeconds) << " " << stateList.size() << "\n\n";
                return;
            }
            else
            {
                visitedMap[first->state] = 1;
                moveList = allMovesList(first->state);

                for (int i = 0; i < moveList.size(); i++) {
                    Node child;

                    int countNodes = 0;
                    string direction;
                    if (moveList[i].direction == 0)
                        direction = "left";
                    else if (moveList[i].direction == 1)
                        direction = "right";
                    else if (moveList[i].direction == 2)
                        direction = "up";
                    else direction = "down";

                    child.state = applyMoveCloning(first->state, moveList[i].piece, direction);

                    child.parent = first;
                    child.move.direction = moveList[i].direction;
                    child.move.piece = moveList[i].piece;

                    numberOfNodesExplored = numberOfNodesExplored + 1;

                    if(visitedMap[child.state]!=1)
                    {
                        visitedMap[child.state] = 1;
                        bfsQueue.push(child);
                    }
                }
            }
        }
        // cout << "Puzzle cannot be solved\n";
    }

    //Function to implement Depth First Search
    void DFS(vector<vector<int>> puzzleMatrix)
    {
        stack<Node> dfsStack;
        high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        Node rootNode;

        int i = 0;
        int numberOfNodesExplored = 0;
        rootNode.state = puzzleMatrix;
        rootNode.parent = nullptr;

        rootNode.move.piece = 0;
        rootNode.move.direction = 0;

        dfsStack.push(rootNode);

        map<vector<vector<int>>,int> visitedMap;

        visitedMap[rootNode.state] = 1;

        while (!dfsStack.empty()) {

            Node *first = new(Node);
            *first = dfsStack.top();
            vector<PieceMovement> moveList;
            dfsStack.pop();

            bool isSolved = isPuzzleSolved(first->state);
            if (isSolved)
            {
                // cout << "Puzzle Solved";
                printPuzzleMatrix(puzzleMatrix);
                Node finalnode = *first;
                Node temp = *first;
                vector<Node> stateList;

                bool compareState;
                string direction;

                do {
                    stateList.push_back(temp);
                    if (temp.parent == NULL)
                        break;
                    temp = *(temp.parent);
                    compareState = compareStates(temp.state, rootNode.state);
                } while (&temp != NULL);

                for (int i = stateList.size() - 1; i >= 0; i--)
                {
                    if (stateList[i].move.piece != 0)
                    {
                        if (stateList[i].move.direction == 0)
                            direction = "left";
                        else if (stateList[i].move.direction == 1)
                            direction = "right";
                        else if (stateList[i].move.direction == 2)
                            direction = "up";
                        else
                            direction = "down";
                        cout << "\n(" << stateList[i].move.piece << "," << direction.c_str() << ")\n\n";
                        printPuzzleMatrix(stateList[i].state);
                    }
                }
                cout << "\n";
                high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
                microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

                float microToSeconds = 1000000;

                cout << numberOfNodesExplored << " " << double(duration.count() / microToSeconds) << " " << stateList.size() << "\n";
                //cout << "Solved puzzle state:\n";
                //printPuzzleMatrix(finalnode.state);
                return;
            }
            else
            {
                visitedMap[first->state] = 1;

                moveList = allMovesList(first->state);

                for (i = 0; i < moveList.size(); i++) {
                    Node child;

                    int countNodes = 0;
                    string direction;
                    if (moveList[i].direction == 0)
                        direction = "left";
                    else if (moveList[i].direction == 1)
                        direction = "right";
                    else if (moveList[i].direction == 2)
                        direction = "up";
                    else
                        direction = "down";

                    child.state = applyMoveCloning(first->state, moveList[i].piece, direction);

                    child.parent = first;
                    child.move.direction = moveList[i].direction;
                    child.move.piece = moveList[i].piece;


                    numberOfNodesExplored = numberOfNodesExplored + 1;

                    if (visitedMap[child.state] != 1) {
                        dfsStack.push(child);
                        visitedMap[child.state] = 1;
                    }
                }
            }
        }
        cout << "Puzzle cannot be solved\n";
    }

    //Function to implement Iterative Deepening Search
	void IDS(vector<vector<int>> puzzleMatrix)
    {
        high_resolution_clock::time_point start = high_resolution_clock::now();
        queue<Node> idsQueue;

        map<vector<vector<int>>,int> visitedMap;

        Node rootNode;

        int numberOfNodesExplored = 0;

        rootNode.state = puzzleMatrix;
        rootNode.parent = nullptr;

        rootNode.move.direction = 0;
        rootNode.move.piece = 0;

        idsQueue.push(rootNode);

        visitedMap[rootNode.state] = 1;

        while (!idsQueue.empty())
        {
            Node *first = new(Node);
            *first = idsQueue.front();
            vector<PieceMovement> moveList;
            idsQueue.pop();

            bool isSolved = isPuzzleSolved(first->state);
            //printPuzzleMatrix(first->state);

            if (isSolved)
            {
                printPuzzleMatrix(puzzleMatrix);

                Node finalnode = *first;
                Node temp = *first;

                vector<Node> stateList;

                bool compareState;
                string direction;

                do {
                    stateList.push_back(temp);
                    if (temp.parent == NULL)
                        break;
                    temp = *(temp.parent);
                    compareState = compareStates(temp.state, rootNode.state);
                } while (&temp != NULL);

                for (int i = stateList.size() - 1; i >= 0; i--)
                {
                    if (stateList[i].move.piece != 0)
                    {
                        if (stateList[i].move.direction == 0)
                            direction = "left";
                        else if (stateList[i].move.direction == 1)
                            direction = "right";
                        else if (stateList[i].move.direction == 2)
                            direction = "up";
                        else
                            direction = "down";
                        cout << "\n(" << stateList[i].move.piece << "," << direction.c_str() << ")" << "\n\n";
                        printPuzzleMatrix(stateList[i].state);
                    }
                }
                cout << "\n";

                // cout << "Solved puzzle state:";
                //  printPuzzleMatrix(finalnode.state);

                high_resolution_clock::time_point stop = std::chrono::high_resolution_clock::now();
                microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

                float microToSeconds = 1000000;

                cout << numberOfNodesExplored << " " << double(duration.count() / microToSeconds) << " " << stateList.size() << "\n\n";
                return;
            }
            else
            {
                visitedMap[first->state] = 1;
                moveList = allMovesList(first->state);

                for (int i = 0; i < moveList.size(); i++) {
                    Node child;

                    int countNodes = 0;
                    string direction;
                    if (moveList[i].direction == 0)
                        direction = "left";
                    else if (moveList[i].direction == 1)
                        direction = "right";
                    else if (moveList[i].direction == 2)
                        direction = "up";
                    else direction = "down";

                    child.state = applyMoveCloning(first->state, moveList[i].piece, direction);

                    child.parent = first;
                    child.move.direction = moveList[i].direction;
                    child.move.piece = moveList[i].piece;

                    numberOfNodesExplored = numberOfNodesExplored + 1;

                    if(visitedMap[child.state]!=1)
                    {
                        visitedMap[child.state] = 1;
                        idsQueue.push(child);
                    }
                }
            }
        }
        // cout << "Puzzle cannot be solved\n";
    }
};

//main() function
int main()
{
    class SlidingBrick Sb;

    Sb.readInputFile("SBP-level0.txt");

    class PieceMove pieceMovement;

    pieceMovement.printPuzzleMatrix(Sb.puzzleMatrix);

    pieceMovement.printAllPossibleMoves(Sb.puzzleMatrix);

    cout << "\nRandom Walk\n";
    pieceMovement.randowWalk(Sb.puzzleMatrix, 3);


    cout << "\n";
    Sb.readInputFile("SBP-level1.txt");
    pieceMovement;

    pieceMovement.printPuzzleMatrix(Sb.puzzleMatrix);

    pieceMovement.printAllPossibleMoves(Sb.puzzleMatrix);

    cout << "\nBFS starting\n";

    pieceMovement.BFS(Sb.puzzleMatrix);

    cout << "\nDFS starting\n";

    pieceMovement.DFS(Sb.puzzleMatrix);

    cout << "\nIDS starting\n";

    pieceMovement.IDS(Sb.puzzleMatrix);
    return 0;
}
