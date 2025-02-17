
Final Project Report for Introduction to Computing A
Instructor: Wang Houfeng
Team Members: Zhang Kaiyue, Zhou Zixuan
Date: December 2022
1. Introduction
For this project, we implemented the “Anti-Go” game, where players interact with an AI, using C++.

In terms of algorithms, based on an evaluation system that avoids self-elimination and trapped stones, we used a greedy algorithm to evaluate the available moves for our side. The greedy algorithm selects the locally optimal strategy. Considering that the greedy algorithm has high randomness in the early game, we manually designed an initial strategy to prioritize advantageous edge positions for the first twelve moves. After that, the AI switches to the greedy algorithm to improve its winning rate.

For the interface design, we used EGE (Easy Graphics Engine) to print background images, capture mouse inputs, and play music. Instead of using EGE’s built-in drawing functions to render the board and pieces, we pre-designed images in other software and directly inserted them into the program. This approach reduced the code complexity while making the interface more visually appealing and intuitive.

Instead of keyboard inputs, we implemented mouse-click interactions, which prevented a large number of invalid inputs and enhanced the overall gaming experience.

2. Feature Design
1. Basic Features
Save Game: Press the "Save" button to store the current game state.
Load Game: Press the "Load" button to retrieve previously saved game states.
Restart Game: Players can restart the game in two ways:
Clicking the "Restart" button in the bottom right corner during gameplay.
Clicking "Restart" after a game has concluded.
Exit Game:
At the start menu, players can click "Exit" to leave the game.
During gameplay, clicking "Exit" in the bottom right corner will close the game.
If the game ends and the player does not click "Restart," the program will automatically exit after 10 seconds.
2. Background Music
Upon entering the game interface, background music plays automatically.
Players can toggle music on/off using the "Music Pause/Play" button.
3. Game Mode Selection
Players can choose between Single-player mode and Two-player mode:
Single-player mode: The player competes against an AI.
Two-player mode: Two players can play against each other or one person can play alone for practice. The AI does not participate in two-player mode, but if the player clicks "Hint," the AI will suggest moves.
If Single-player mode is selected, an additional selection appears:
The player must choose between playing first or playing second.
After mode selection, the game enters the board interface after a 5-second countdown.
4. Move Suggestions
Clicking the "Hint" button prompts the AI to suggest a move.
The AI suggests a move from the player’s perspective.
The suggested move is displayed as a dimmed piece (distinguishable from black and white pieces).
The player may follow or ignore the suggestion.
5. Rule Explanation
After selecting "New Game", the player is taken to a screen explaining the rules of "Anti-Go" and additional game features.
If the player chooses:
Two-player mode or Single-player first-move, a rule reminder appears:
"The first move should not be placed at the center of the board."
If Single-player second-move is selected, no reminder appears.
6. Countdown Timer
Each move has a 10-second limit.
The bottom-left corner of the screen displays the countdown.
If the time runs out, the game prompts "Time's up! Please make your move quickly!"
The message disappears once the player makes a move.
7. Illegal Move Alerts
Players make moves by clicking grid points on the board.
If a player clicks an occupied space, a message appears:
"This position is already occupied."
The message disappears once the player makes a valid move.
8. Other Interactions and Feedback
The system responds to player actions, such as displaying "Single-player mode selected successfully."
In the rule explanation interface, players press the Spacebar to proceed.
When a game concludes, a "Winner Announcement" appears:
The player can restart the game by clicking "Restart".
If no action is taken, the game exits after a 10-second countdown.
3. Code Architecture
The functions in our code are categorized into six main types:

Main function
Interface rendering functions: Includes four main UI screens and a function to erase previous graphics.
User interaction functions: Handles mouse input and triggers appropriate responses.
Move functions: Evaluates moves, checks legality, and renders pieces.
Menu functions: Implements save, load, restart, music toggle, and hint features.
Algorithm functions: Handles game logic, including capturing stones and applying the greedy strategy.
GUI Implementation Strategy
A for-loop continuously monitors mouse input inside key functions.
Based on the mouse position, the system determines the action (e.g., menu selection, move placement).
If the player clicks Exit, the main function terminates immediately.
To avoid graphical glitches, an erase function clears overlapping elements.
The human_play and ai_play functions evaluate game state after each move.
If the game concludes, the winner announcement screen appears.
Otherwise, the piece is placed, and the turn alternates (chess_color = chess_color % 2 + 1).
4. AI Algorithm
Greedy Algorithm Implementation
In "Anti-Go," self-elimination and trapping the opponent's stones result in defeat.

We define valid moves as those that do not lead to self-elimination or trapping one’s own stones.
The key evaluation metric:
The difference between the number of valid moves available to the AI vs. the opponent.
If the AI has more valid moves than the opponent, it is in an advantageous position.
The AI searches for all valid moves, simulates the new board state, and evaluates:
Valid moves for AI – Valid moves for opponent
The move that maximizes this difference is chosen.
If multiple moves have the same score, one is chosen randomly.
Overcoming Early-Game Randomness
The greedy algorithm is unreliable in the early game due to insufficient information.
To mitigate this, we manually programmed the first 12 moves to prioritize corner and edge control.
Once enough pieces are on the board, the AI switches to the greedy algorithm for optimal decision-making.
5. Experimental Results
Game Execution Results

The game ran smoothly, successfully implementing all core features.
AI played based on the greedy algorithm, and the system correctly detected AI victory and displayed the appropriate message.
BotZone Competition Results

The game successfully completed all test matches.
6. Conclusion
This project took approximately one month to complete.

We encountered several challenges, including:
Both team members getting sick at different times.
Switching the algorithm from MCTS to Greedy mid-development.
Debugging a UI issue caused by a macro definition error.
Despite these challenges, we successfully implemented all required features while incorporating our own creative ideas.

Zhang Kaiyue: Algorithm research & implementation.
Zhou Zixuan: UI design & core functionality implementation.
