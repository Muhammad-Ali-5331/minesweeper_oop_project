#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

struct Cell
{
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int adjacentMines;
};

struct Account
{
    string userName;
    string passwordHash;
    int highScore;
};

struct DynamicArray
{
    Account *data;
    int size;
    int capacity;

    DynamicArray()
    {
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    ~DynamicArray()
    {
        delete[] data;
    }

    void resize(int newCapacity)
    {
        Account *newData = new Account[newCapacity];
        for (int i = 0; i < size; ++i)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }

    void addRecord(const Account &acc)
    {
        if (size == capacity)
            resize(capacity == 0 ? 1 : capacity * 2);
        data[size++] = acc;
    }

    Account &operator[](int index) { return data[index]; }
    const Account &operator[](int index) const { return data[index]; }
};

int rows, cols, mines;
Cell **board;
int revealedCount = 0;
string currentUser;
int userBestScore = -1;
DynamicArray accounts;

void printHeader();
void authenticateUser();
void loadAccounts();
void saveAccounts();
bool signup();
bool login();
string hashPassword(const string &password);
int getChoice();
void initializeBoard();
void placeMines();
void calculateAdjacency();
void displayBoard(bool revealAll);
bool isValid(int r, int c);
void revealCell(int r, int c);
void toggleFlag(int r, int c);
bool checkWin();
void updateHighScore(int moves);
void displayHighScores();
void freeBoard();
void playGame();

// Main
int main()
{
    srand(time(0));
    printHeader();
    authenticateUser();
    while (true)
    {
        playGame();
        cout << "\nDo you want to play again? (1 = Yes, 2 = No): ";
        int ch = getChoice();
        if (ch == 2)
        {
            cout << "Thanks for playing!\n";
            break;
        }
        cout << "\nStarting a new game...\n";
    }
}

void printHeader()
{
    cout << "===============================================\n";
    cout << "              MINESWEEPER GAME                 \n";
    cout << "-----------------------------------------------\n";
    cout << "# = Hidden  F = Flag  * = Mine\n";
    cout << "Numbers show adjacent mines\n";
    cout << "===============================================\n\n";
}

void authenticateUser()
{
    loadAccounts();
    bool authenticated = false;
    while (!authenticated)
    {
        cout << "1. Login\n2. Signup\nChoice: ";
        int choice = getChoice();
        if (choice == 1)
            authenticated = login();
        else
            authenticated = signup();
    }
}

void loadAccounts()
{
    ifstream inFile("accounts.txt");
    if (!inFile)
        return;
    string userName, passwordHash;
    int highScore;
    while (inFile >> userName >> passwordHash >> highScore)
    {
        if (highScore >= -1 && userName.length() > 0)
            accounts.addRecord({userName, passwordHash, highScore});
    }
    inFile.close();
}

void saveAccounts()
{
    ofstream outFile("accounts.txt");
    if (!outFile)
    {
        cout << "Error: Unable to save accounts. Check file permissions.\n";
        return;
    }
    for (int i = 0; i < accounts.size; ++i)
    {
        outFile << accounts[i].userName << " " << accounts[i].passwordHash << " " << accounts[i].highScore << endl;
    }
    outFile.close();
}

bool signup()
{
    string userName, password;
    cout << "Enter user name (3-20 characters): ";
    cin >> userName;
    if (userName.length() < 3 || userName.length() > 20)
    {
        cout << "Username must be 3-20 characters..! Try again...\n\n";
        return false;
    }
    for (int i = 0; i < accounts.size; ++i)
        if (accounts[i].userName == userName)
        {
            cout << "User name already exists..! Try logging in...\n\n";
            return false;
        }
    cout << "Enter password (3-20 characters): ";
    cin >> password;
    if (password.length() < 3 || password.length() > 20)
    {
        cout << "Password must be 3-20 characters..! Try again...\n\n";
        return false;
    }
    string hashed = hashPassword(password);
    accounts.addRecord({userName, hashed, -1});
    saveAccounts();
    currentUser = userName;
    userBestScore = -1;
    cout << "Account created successfully..! \nWelcome, " << userName << "..!\n\n";
    return true;
}

bool login()
{
    string userName, password;
    cout << "Enter user name: ";
    cin >> userName;
    for (int i = 0; i < accounts.size; ++i)
    {
        if (accounts[i].userName == userName)
        {
            cout << "Enter password: ";
            cin >> password;
            string hashed = hashPassword(password);
            if (accounts[i].passwordHash != hashed)
            {
                cout << "Incorrect password..! Try again...\n\n";
                return false;
            }
            currentUser = userName;
            userBestScore = accounts[i].highScore;
            if (userBestScore == -1)
                cout << "Login successful..! Welcome back, " << userName << "..! No games played yet.\n\n";
            else
                cout << "Login successful..! Welcome back, " << userName << "..! Your highest score: " << userBestScore << " moves.\n\n";
            return true;
        }
    }
    cout << "User name not found..! Try signing up...\n\n";
    return false;
}

string hashPassword(const string &password)
{
    int hash = 0;
    for (char c : password)
    {
        hash += (c * 3);
    }
    return to_string(hash);
}

void playGame()
{
    revealedCount = 0;
    cout << "Enter board size (rows cols, 2-50): ";
    while (!(cin >> rows >> cols) || rows < 2 || cols < 2 || rows > 50 || cols > 50)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input..! Enter rows and cols (2-50): ";
    }
    cout << "Enter number of mines (0 for random, max " << (rows * cols - 1) << "): ";
    while (!(cin >> mines) || mines < 0)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input..! Enter non-negative number: ";
    }
    if (mines == 0)
    {
        mines = (rows * cols) / 6;
        if (mines < 1)
            mines = 1;
    }
    if (mines >= rows * cols)
    {
        cout << "Too many mines..! Setting to maximum: " << (rows * cols - 1) << endl;
        mines = rows * cols - 1;
    }
    initializeBoard();
    placeMines();
    calculateAdjacency();

    int moves = 0;
    bool validMove = false;

    while (true)
    {
        displayBoard(false);
        cout << "\nMenu:\n1. Reveal Cell\n2. Flag/Unflag Cell\nChoice: ";
        int choice = getChoice();
        int r, c;
        cout << "Enter row and column (0-" << rows - 1 << " 0-" << cols - 1 << "): ";
        while (!(cin >> r >> c))
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input..! Enter row and column: ";
        }
        if (!isValid(r, c))
        {
            cout << "Invalid coordinates..! Try again...\n";
            continue;
        }

        validMove = false;

        if (choice == 1)
        {
            if (board[r][c].isFlagged)
            {
                cout << "Cell is flagged..! Unflag first...\n";
                continue;
            }
            if (board[r][c].isRevealed)
            {
                cout << "Cell already revealed..! Try another cell...\n";
                continue;
            }
            if (board[r][c].isMine)
            {
                displayBoard(true);
                cout << "\nBoom..! You hit a mine. Game Over..!\n";
                break;
            }
            revealCell(r, c);
            validMove = true;
        }
        else if (choice == 2)
        {
            if (board[r][c].isRevealed)
            {
                cout << "Cannot flag revealed cell..!\n";
                continue;
            }
            toggleFlag(r, c);
            validMove = true;
        }

        if (validMove)
            moves++;

        if (checkWin())
        {
            displayBoard(true);
            cout << "\nCongratulations..! You cleared the board in " << moves << " moves.\n";
            updateHighScore(moves);
            cout << "\n----- Current Player Summary -----\n";
            cout << "Player: " << currentUser << "\n";
            cout << "Score this game: " << moves << "\n";
            if (userBestScore == -1)
                cout << "Highest score: No previous games\n";
            else
                cout << "Highest score: " << userBestScore << "\n";
            cout << "----------------------------------\n";
            displayHighScores();
            break;
        }
    }

    freeBoard();
}

void initializeBoard()
{
    board = new Cell *[rows];
    for (int i = 0; i < rows; i++)
    {
        board[i] = new Cell[cols];
        for (int j = 0; j < cols; j++)
        {
            board[i][j].isMine = false;
            board[i][j].isRevealed = false;
            board[i][j].isFlagged = false;
            board[i][j].adjacentMines = 0;
        }
    }
}

void placeMines()
{
    int placed = 0;
    while (placed < mines)
    {
        int r = rand() % rows;
        int c = rand() % cols;
        if (!board[r][c].isMine)
        {
            board[r][c].isMine = true;
            placed++;
        }
    }
}

void calculateAdjacency()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (board[i][j].isMine)
                continue;
            int count = 0;
            for (int dr = -1; dr <= 1; dr++)
            {
                for (int dc = -1; dc <= 1; dc++)
                {
                    if (dr == 0 && dc == 0)
                        continue;
                    if (isValid(i + dr, j + dc) && board[i + dr][j + dc].isMine)
                        count++;
                }
            }
            board[i][j].adjacentMines = count;
        }
    }
}

void displayBoard(bool revealAll)
{
    cout << "\n    ";
    for (int c = 0; c < cols; c++)
        cout << c << " ";
    cout << "\n   ";
    for (int c = 0; c < cols; c++)
        cout << "--";
    cout << endl;

    for (int r = 0; r < rows; r++)
    {
        cout << r << " | ";
        for (int c = 0; c < cols; c++)
        {
            if (revealAll)
            {
                if (board[r][c].isMine)
                    cout << "* ";
                else
                    cout << board[r][c].adjacentMines << " ";
            }
            else
            {
                if (board[r][c].isRevealed)
                    cout << board[r][c].adjacentMines << " ";
                else if (board[r][c].isFlagged)
                    cout << "F ";
                else
                    cout << "# ";
            }
        }
        cout << endl;
    }
}

bool isValid(int r, int c)
{
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

void revealCell(int r, int c)
{
    if (!isValid(r, c) || board[r][c].isRevealed || board[r][c].isFlagged)
        return;
    board[r][c].isRevealed = true;
    revealedCount++;
    if (board[r][c].adjacentMines == 0)
    {
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++)
                if (dr != 0 || dc != 0)
                    revealCell(r + dr, c + dc);
    }
}

void toggleFlag(int r, int c)
{
    if (!isValid(r, c) || board[r][c].isRevealed)
        return;

    int currentFlags = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (board[i][j].isFlagged)
                currentFlags++;

    if (!board[r][c].isFlagged && currentFlags == mines)
    {
        cout << "You already used all " << mines << " flags!\n";
        return;
    }

    board[r][c].isFlagged = !board[r][c].isFlagged;
}

bool checkWin()
{
    int correctFlags = 0;
    int totalFlags = 0;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
        {
            if (board[r][c].isFlagged)
            {
                totalFlags++;
                if (board[r][c].isMine)
                    correctFlags++;
            }
        }

    return (correctFlags == mines && totalFlags == mines && revealedCount == (rows * cols - mines));
}

void updateHighScore(int moves)
{
    if (userBestScore == -1 || moves < userBestScore)
    {
        cout << "New High Score..! Updating your record...\n";
        userBestScore = moves;
        for (int i = 0; i < accounts.size; ++i)
        {
            if (accounts[i].userName == currentUser)
            {
                accounts[i].highScore = moves;
                break;
            }
        }
        saveAccounts();
    }
    else
    {
        cout << "Your best score remains: " << userBestScore << " moves.\n";
    }
}

void displayHighScores()
{
    cout << "\n----- High Scores -----\n";
    for (int i = 0; i < accounts.size; ++i)
    {
        cout << setw(10) << left << accounts[i].userName << " - ";
        if (accounts[i].highScore == -1)
            cout << "No games played\n";
        else
            cout << accounts[i].highScore << " moves\n";
    }
    cout << "------------------------\n";
}

void freeBoard()
{
    for (int i = 0; i < rows; i++)
        delete[] board[i];
    delete[] board;
    board = nullptr;
}

int getChoice()
{
    int choice;
    while (true)
    {
        if (cin >> choice)
        {
            if (choice == 1 || choice == 2)
                return choice;
            cout << "Invalid choice..! Enter 1 or 2: ";
        }
        else
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input..! Enter 1 or 2: ";
        }
    }
}
