# Minesweeper Game 💣

A classic Minesweeper game implemented in C++, featuring user authentication, high score tracking, and a dynamic array for managing user accounts. This project provides a console-based Minesweeper experience with account management and persistent high score storage.

## 🚀 Features

- **User Authentication:** Securely create accounts and log in to track your high scores. Passwords are hashed for enhanced security. 🔐
- **Dynamic Board Generation:**  Initializes the Minesweeper board with customizable dimensions (rows and columns) and mine count. ⚙️
- **Random Mine Placement:** Randomly distributes mines across the board to create a unique game experience each time. 🎲
- **Adjacency Calculation:** Calculates the number of adjacent mines for each cell, providing the core Minesweeper logic. 🔢
- **Interactive Gameplay:** Reveal cells, flag potential mines, and navigate the board to uncover safe cells. 🕹️
- **Win/Lose Detection:**  Accurately determines when the player has won by revealing all non-mine cells or lost by revealing a mine. ✅/❌
- **High Score Tracking:**  Records and updates the user's high score, persisting it across game sessions. 🏆
- **Account Management:** Uses a dynamic array to efficiently store and manage user accounts. 👥
- **Persistent Data:** Loads and saves user account data to a file, ensuring that accounts and high scores are preserved. 💾

## 🛠️ Tech Stack

- **Language:** C++
- **Data Structures:** `struct` (Cell, Account), Dynamic Array
- **Hashing:** Password hashing for secure authentication
- **Input/Output:** `<iostream>`, `<fstream>`
- **String Manipulation:** `<string>`
- **Formatting:** `<iomanip>`
- **Random Number Generation:** `<ctime>`, `<cstdlib>`
- **Build Tool:** (Assumed) g++ or similar C++ compiler

## 📦 Installation

### Prerequisites

- A C++ compiler (e.g., g++, clang)
- Standard C++ libraries

### Installation

1.  Clone the repository:

    ```bash
    git clone <repository_url>
    cd <repository_directory>
    ```

2.  Compile the code:

    ```bash
    g++ main.cpp -o minesweeper
    ```

### Running Locally

1.  Execute the compiled binary:

    ```bash
    ./minesweeper
    ```

## 💻 Usage

1.  Run the executable.
2.  Choose to sign up or log in.
3.  Follow the on-screen prompts to play the game.
4.  Reveal cells and flag potential mines to uncover the board.
5.  Try to reveal all non-mine cells to win!

## 📂 Project Structure

```
├── main.cpp          # Main source code file
├── README.md         # Project documentation
├── accounts.dat      # (Potentially) File to store user account data
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests with bug fixes, new features, or improvements to the existing code.

## 📬 Contact

If you have any questions or suggestions, please feel free to contact me at [malitariq5324@gmail.com](mailto:malitariq5324@gmail.com).

## 💖 Thanks Message

Thank you for checking out this Minesweeper game! I hope you enjoy playing it and find the code helpful.
