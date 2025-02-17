#define _CRT_SECURE_NO_WARNINGS // Higher versions of VS consider scanf, fopen, etc., unsafe by default. Adding this statement allows their use.
#include <iostream>
#include <graphics.h>
#include <windows.h>
#include <cstdio>
using namespace std;

#define GRID_NUM 11         // Number of rows on the board, where 1-9 are playable coordinates, 0 and 10 are boundaries
#define GRID_COUNT 81       // Total number of playable positions
#define BORDER 3            // Boundary
#define NOSTONE 0           // Empty position with no stone
#define STEPCHANGE 12       // AI switches to greedy algorithm after 12 moves

typedef struct // (Zhang Kaiyue)
{
    int i;
    int j;
    int di; // The current state when searching for a stone's path, 0 means impassable, 1 means passable
} Road;

typedef struct // (Zhang Kaiyue)
{
    Road data[GRID_COUNT];
    int top;
} stType;

int winWidth = 700; // Window width
int winHeight = 520; // Window height
int board[GRID_NUM][GRID_NUM]; // Board information: 0 means playable, 1 means black stone, 2 means white stone, 3 means boundary
double mark[GRID_NUM][GRID_NUM];
int step = 0; // Tracks the number of moves made by the AI
int chess_color = 1; // Stone color, 1 for black, 2 for white
int mode = 1; // Single-player mode is 1, two-player mode is 2
bool ai_turn; // 1 means AI's turn, 0 means player's turn
bool legal = 0; // Validity of the move (whether the position is already occupied)
bool ai_black = 0; // Whether the AI is playing black, used for determining the winner
int x;
int y;
double endTime;
double runTime;
double remainTime; // Countdown timer

mouse_msg msg = {0};
MUSIC music;
DWORD status = music.GetPlayStatus();
PIMAGE pimg = newimage(); // Image pointer
PIMAGE pimg_black = newimage(); // Black stone image
PIMAGE pimg_white = newimage(); // White stone image
PIMAGE pimg_prompt = newimage(); // Hint stone image

void print_firstPage(); // (84-97),
void print_secondPage(); // (99-115)
void print_thirdPage(); // (117-133)
void print_background();
void erase(int left, int top, int right, int bottom);
// These five functions are used for rendering the graphical interface (Zhou Zixuan)

int play_firstpage();
void play_thirdpage();
// Some mode selection and feature implementations (Zhou Zixuan)

int ai_play();
int human_play();
int judge_winner(int x);
// Chess interaction (Zhou Zixuan)

void restart(); // Restart the game
void save();
int read();
void prompt(); // Hint
void music_play();
void music_judge();
// These functions are used for menu functionality (Zhou Zixuan)

bool Breath(int xi, int yi); // Determines whether a single stone has liberties
bool Rule_Suicide(int xi, int yi, int color); // Self-elimination check
bool Rules(int xi, int yi, int color); // Trapped stone check
int markpoints(int color);
bool put_OK(int xi, int yi, int color);
void move(int color);
int greedy(int color); // Greedy algorithm
// These functions are used for AI algorithms (Zhang Kaiyue)

void print_firstPage() // Render the first screen (Zhou Zixuan)
{
    setfont(40, 0, "Huaguang Amber_CNKI"); // setfont(x, y, "xx")  x: font height  y: font width (0 for auto-fit) "xx": font
    xyprintf(0.422 * winWidth, 0.2 * winHeight, "No-Go Chess");
    setfont(20, 0, "KaiTi");
    xyprintf(0.447 * winWidth, 0.4 * winHeight, "New Game");
    xyprintf(0.46 * winWidth, 0.6 * winHeight, "Exit");
    setfont(15, 0, "KaiTi");
    xyprintf(0.74 * winWidth, 0.955 * winHeight, "Developers: Zhang Kaiyue, Zhou Zixuan");
    pimg = newimage();
    getimage(pimg, "board/pku.png"); // Retrieve the Peking University logo
    putimage(20, 410, 290, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
}

void print_secondPage() // Render the second screen, mainly for explaining the game rules (Zhou Zixuan)
{
    setfont(30, 0, "Huaguang Amber_CNKI");
    xyprintf(0.1 * winWidth, 0.1 * winHeight, "Please read the game instructions first~");
    setfont(20, 0, "KaiTi");
    xyprintf(0.067 * winWidth, 0.25 * winHeight, "No-Go game, as the name suggests, is about avoiding capturing the opponent's stones (trapping).");
    xyprintf(0.07 * winWidth, 0.3 * winHeight, "Try to make your stones captured (trapped) by the opponent.");
    xyprintf(0.067 * winWidth, 0.35 * winHeight, "Black moves first, players take turns, and each has 10 seconds to think per move, no regrets.");
    xyprintf(0.07 * winWidth, 0.40 * winHeight, "If a move captures the opponent’s stones, the player who made the move loses; self-elimination also leads to a loss.");
    xyprintf(0.07 * winWidth, 0.45 * winHeight, "If you're unsure, click the 'Hint' button, and the AI will help you!");
    xyprintf(0.07 * winWidth, 0.6 * winHeight, "Are you ready? Good luck!");
    setcolor(BLUE);
    xyprintf(0.07 * winWidth, 0.7 * winHeight, "Press the spacebar to continue after reading");
    pimg = newimage();
    getimage(pimg, "board/pku.png"); // Retrieve the logo again
    putimage(20, 410, 290, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
}

void print_thirdPage() // Render the third screen, mainly for selecting game mode (Zhou Zixuan)
{
    setcolor(BLACK);
    setfont(30, 0, "Huaguang Amber_CNKI");
    xyprintf(0.1 * winWidth, 0.1 * winHeight, "Please select a game mode~");
    setfont(20, 0, "KaiTi");
    xyprintf(0.07 * winWidth, 0.25 * winHeight, "If you want to challenge the AI, please select");
    pimg = newimage();
    getimage(pimg, "board/single.png");
    putimage(410, 110, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    xyprintf(0.07 * winWidth, 0.35 * winHeight, "If you have a companion or want to play for fun, please select");
    pimg = newimage();
    getimage(pimg, "board/couple.png");
    putimage(410, 170, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
}

void print_background() // Render the background image (Zhou Zixuan)
{
    pimg = newimage();
    getimage(pimg, "board/board.png"); // Retrieve the board image
    putimage(100, 40, pimg);
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/eecs call.png");
    putimage(540, 110, 120, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/music.png");
    putimage(540, 40, 120, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/save.png");
    putimage(540, 260, 70, 35, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/read.png");
    putimage(540, 300, 70, 35, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/prompt.png");
    putimage(540, 220, 100, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/end.png");
    putimage(590, 460, 80, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = newimage();
    getimage(pimg, "board/restart.png");
    putimage(590, 405, 100, 50, pimg, 0, 0, getwidth(pimg), getheight(pimg));
    // Render the menu images
}

void erase(int left, int top, int right, int bottom) // Cover with a white rectangle to erase previously printed parts (Zhou Zixuan)
{
    setfillcolor(WHITE);
    bar(left, top, right, bottom);
    setfillcolor(BLACK);
}

int play_firstpage() // Return value indicates whether to exit the game (Zhou Zixuan)
{
    for (; is_run(); delay_fps(60))
    {     // is_run() function checks if the EGE window still exists, delay_fps(60) listens for user mouse clicks every 1/60 second. 
          // If the interval is too long, the program may become unresponsive; if too short, rapid clicks may crash the program.
        while (mousemsg()) // getmouse retrieves mouse messages
        {
            msg = getmouse();
            if (msg.x <= 390 && msg.x > 300 && msg.y > 200 && msg.y < 230 && msg.is_down())
                return 1;
            if (msg.x <= 360 && msg.x > 310 && msg.y > 320 && msg.y < 350 && msg.is_down())
                return 0; // Exit game
        }
    }
    return 1;
}


void play_thirdpage() // Interaction for selecting game mode (Zhou Zixuan)
{
    setcolor(BLUE);
    bool b1 = 0;
    bool b2 = 0; // Used to determine if selection is complete; exits the loop once done
    for (; is_run() && b1 == 0; delay_fps(60))
    {
        while (mousemsg()) // getmouse retrieves mouse messages
        {
            msg = getmouse();
            if (msg.x <= 580 && msg.x > 410 && msg.y > 110 && msg.y < 170 && msg.is_down())
            {
                mode = 1;
                b1 = 1;
                setfont(18, 0, "Huaguang Amber_CNKI");
                xyprintf(0.77 * winWidth, 0.3 * winHeight, "Single-player mode selected successfully");
            }
            if (msg.x <= 580 && msg.x > 410 && msg.y > 170 && msg.y < 230 && msg.is_down() && b1 == 0)
            {
                mode = 2;
                ai_turn = 0;
                b1 = 1;
                setfont(18, 0, "Huaguang Amber_CNKI");
                xyprintf(0.77 * winWidth, 0.3 * winHeight, "Two-player mode selected successfully");
            }
        }
    }
    if (mode == 1) // Single-player mode requires further selection
    {
        xyprintf(0.07 * winWidth, 0.45 * winHeight, "Do you choose to play first or second?");
        pimg = newimage();
        getimage(pimg, "board/first.png");
        putimage(100, 270, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
        delimage(pimg);
        pimg = newimage();
        getimage(pimg, "board/follow.png");
        putimage(300, 270, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
        delimage(pimg);
        for (; is_run() && b2 == 0; delay_fps(60))
        {
            while (mousemsg() && b2 == 0) // getmouse retrieves mouse messages
            {
                msg = getmouse();
                if (msg.x <= 220 && msg.x > 100 && msg.y > 270 && msg.y < 330 && msg.is_down()) // Player chooses to play first
                {
                    ai_turn = 0;
                    ai_black = 0;
                    b2 = 1;
                }
                if (msg.x > 300 && msg.x <= 570 && msg.y > 270 && msg.y < 330 && msg.is_down() && b2 == 0) // Player chooses to play second
                {
                    ai_turn = 1;
                    ai_black = 1;
                    b2 = 1;
                }
            }
        }
    }
    if (ai_turn == 0) // If player moves first, display reminder
    {
        setfont(18, 0, "Huaguang Amber_CNKI");
        xyprintf(0.17 * winWidth, 0.80 * winHeight, "Note: The first move should not be placed in the center of the board");
    }
    endTime = fclock() + 5;
    for (; is_run(); delay_fps(60))
    {
        runTime = fclock();
        remainTime = endTime - runTime; // Current time equals the difference between the two times
        erase(19, 460, 319, 480);
        xyprintf(19, 460, "%d seconds until the game screen appears", (int)ceil(remainTime));
        if (remainTime <= 0)
            break;
    }
}

int ai_play() // Call AI algorithm for making a move (Zhou Zixuan)
{
    getimage(pimg_black, "board/black1.png"); // Retrieve black stone image
    getimage(pimg_white, "board/white1.png"); // Retrieve white stone image
    step++; // Increment AI move counter
    if (step <= STEPCHANGE)
    {
        move(chess_color); // Use positioning algorithm for the first 12 moves, ensuring AI does not lose early
        if (chess_color == 1)
            putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
        else if (chess_color == 2)
            putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
        board[x][y] = chess_color;
        chess_color = chess_color % 2 + 1; // Toggle color
        ai_turn = 0;
    }
    else if (step > STEPCHANGE)
        if (greedy(chess_color)) // Use greedy algorithm after 12 moves; AI may lose if no valid moves are available
        {
            if (chess_color == 1)
                putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
            else if (chess_color == 2)
                putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
            board[x][y] = chess_color;
            chess_color = chess_color % 2 + 1;
            ai_turn = 0;
        }
        else return judge_winner(2); // If greedy algorithm returns 0, AI has no valid moves and loses
    return 2;
}

int human_play()
{
    getimage(pimg_black, "board/black1.png"); // Retrieve black stone image
    getimage(pimg_white, "board/white1.png"); // Retrieve white stone image
    x = (msg.x - 118) / 40 + 1; // Corresponding grid x-coordinate
    y = (msg.y - 59) / 40 + 1; // Corresponding grid y-coordinate
    if (board[x][y] == 0) // Position is unoccupied
    {
        legal = 1;
        int vic = -1;
        if (Rule_Suicide(x, y, chess_color)) // Self-elimination, determine the winner immediately
        {
            if (mode == 1)
                vic = judge_winner(1);
            else if (mode == 2)
                vic = judge_winner(5 - chess_color);
        }
        else
            vic = judge_winner(0); // No self-elimination, but trapped stone condition may apply
        if (vic == 0)
            return 0;
        if (vic == 1) // Restart
            return 1;
        // vic == 2, continue playing
        if (chess_color == 1)
        {
            putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
            board[x][y] = 1;
        }
        else if (chess_color == 2)
        {
            putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
            board[x][y] = 2;
        }
        chess_color = chess_color % 2 + 1;
        if (mode == 1)
            ai_turn = 1;
    }
    else legal = 0;
    return 2;
}

int judge_winner(int x) // Determine the winner (Zhou Zixuan)
{
    int winner = x; // If x is not 0, the winner is already determined, proceed to result display
    if (winner == 0) // If not lost due to self-elimination, check for trapped stones
    {
        for (int i = 1; i <= 9; i++)
            for (int j = 1; j <= 9; j++)
            {
                if (Rules(i, j, 2)) // Black stones are trapped, black wins
                {
                    if (mode == 1)
                        winner = 2 - ai_black;
                    else if (mode == 2)
                        winner = 3;
                    break;
                }
                if (Rules(i, j, 1)) // White stones are trapped, white wins
                {
                    if (mode == 1)
                        winner = 1 + ai_black;
                    else if (mode == 2)
                        winner = 4;
                    break;
                }
            }
    }
    if (winner) // A winner has been determined
    {
        cleardevice();
        erase(0, 0, 700, 520);
        pimg = newimage();
        if (winner == 1) // AI wins
        {
            getimage(pimg, "board/winner1.png");
            putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
            delimage(pimg);
        }
        else if (winner == 2) // Player wins
        {
            getimage(pimg, "board/winner2.png");
            putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
            delimage(pimg);
        }
        else if (winner == 3) // Black wins
        {
            getimage(pimg, "board/winner3.png");
            putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
            delimage(pimg);
        }
        else if (winner == 4) // White wins
        {
            getimage(pimg, "board/winner4.png");
            putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
            delimage(pimg);
        }
        endTime = fclock() + 10;
        for (; is_run(); delay_fps(60))
        {
            runTime = fclock();
            remainTime = endTime - runTime;
            erase(13, 447, 130, 481);
            xyprintf(19, 460, "Countdown: %d", (int)ceil(remainTime));
            while (mousemsg())
            {
                msg = getmouse();
                if (msg.x > 220 && msg.x <= 450 && msg.y > 225 && msg.y < 272 && msg.is_down())
                {
                    restart(); // Restart the game
                    return 1;
                }
            }
            if (remainTime <= 0)
                return 0;
        }
    }
    return 2;
}

void restart() // Restart the game (Zhou Zixuan)
{
    cleardevice();
    print_thirdPage();
    play_thirdpage();
    cleardevice();
    setcolor(BLACK);
    setfont(20, 0, "KaiTi");
    erase(80, 20, 260, 40);
    erase(165, 475, 570, 520);
    print_background();

    endTime = fclock() + 10;
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= 9; j++)
        {
            board[i][j] = 0;
            mark[i][j] = 0;
        }
    chess_color = 1;
    step = 0;
    legal = 0;
    if (ai_turn == 1)
        ai_play();
}

void save() // Save game state (Zhou Zixuan)
{ 
    FILE* fp1; // File pointer
    FILE* fp2; // File pointer
    fp1 = fopen("data1.txt", "w");
    fp2 = fopen("mark.txt", "w");
    // "w": Open file for writing. If the file exists, clear its content (effectively deleting and recreating it).
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= 9; j++)
        {
            fprintf_s(fp1, "%d\n", board[i][j]);
            fprintf_s(fp2, "%lf\n", mark[i][j]);
        }
    fclose(fp1); // Close file
    FILE* fp; // File pointer
    fp = fopen("turn.txt", "w");
    fprintf_s(fp, "%d\n", chess_color);
    fclose(fp); // Close file
}

int read() // Load game state (Zhou Zixuan)
{
    getimage(pimg_black, "board/black1.png"); // Retrieve black stone image
    getimage(pimg_white, "board/white1.png"); // Retrieve white stone image
    int i;
    int j;
    FILE* fp1; // File pointer
    fp1 = fopen("data1.txt", "r"); // Read chess information, "r" means read-only mode, used to initialize the board
    FILE* fp2;
    fp2 = fopen("mark.txt", "r");
    if (fp1 == NULL)
        return 0;
    print_background(); // Print empty board
    for (i = 1; i <= 9; i++)
        for (j = 1; j <= 9; j++)
        {
            fscanf_s(fp1, "%d", &board[i][j]);
            if (board[i][j] == 1)
                putimage(114 + (i - 1) * 40, 53 + (j - 1) * 40, pimg_black);
            if (board[i][j] == 2)
                putimage(114 + (i - 1) * 40, 53 + (j - 1) * 40, pimg_white);
            fscanf_s(fp2, "%lf", &mark[i][j]);
        }
    fclose(fp1); // Close file
    fclose(fp2);
    FILE* fp;
    fp = fopen("turn.txt", "r"); // Read turn
    if (fp == NULL)
        return 2;
    fscanf_s(fp, "%d", &chess_color);
    fclose(fp); // Close file
    return 1;
}

void prompt() // Display hint using AI algorithm (Zhou Zixuan)
{
    ai_play();
    chess_color = chess_color % 2 + 1;
    step--;
    board[x][y] = 0;
    getimage(pimg_prompt, "board/temp.png");
    putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_prompt);
}

void music_play() // Play music (Zhou Zixuan)
{
    music.OpenFile("Ludovico Einaudi - Nuvole bianche .mp3");
    music.Play();
    status = music.GetPlayStatus();
}

void music_judge() // Determine whether to pause or play based on music status (Zhou Zixuan)
{
    if (status == MUSIC_MODE_PLAY)
    {
        music.Pause(); // Pause
        status = music.GetPlayStatus();
    }
    else if (status == MUSIC_MODE_PAUSE)
    {
        music.Play();
        status = music.GetPlayStatus();
    }
    // If music has finished playing, restart it
    if (music.GetPlayStatus() == MUSIC_MODE_STOP)
        music.Play(0); // Restart from the beginning
}

bool Breath(int xi, int yi) // Check if a stone has liberties (Zhang Kaiyue)
{
    if (board[xi][yi - 1] == 0 || board[xi - 1][yi] == 0 ||
        board[xi + 1][yi] == 0 || board[xi][yi + 1] == 0)
        return true; // Has liberties
    return false; // No liberties
}


bool Rule_Suicide(int xi, int yi, int color) // Self-elimination check, where 'color' is the placed stone's color (Zhang Kaiyue)
{
    int i = 0;
    int j = 0;
    int di = 0;
    int find = 0;
    int k = color; // Save the current stone's color
    stType st;
    st.top = 0;
    // Current stone position
    st.data[st.top].i = xi;
    st.data[st.top].j = yi;
    // Search path state
    st.data[st.top].di = -1;
    // Mark current stone as -2 (indicating search in progress)
    board[xi][yi] = -2;

    while (st.top > -1)
    {
        i = st.data[st.top].i;
        j = st.data[st.top].j;
        di = st.data[st.top].di;
        if (Breath(i, j) == true) // Simple liberty check
        {
            for (i = 1; i < 10; i++)
                for (j = 1; j < 10; j++)
                    if (board[i][j] == -2)
                        board[i][j] = k;
            return false;
        }
        find = 0;
        while (di < 4 && find == 0) // Search for the next possible move
        {
            di++;
            switch (di)
            {
            case 0: i = st.data[st.top].i - 1; j = st.data[st.top].j; break; // Up
            case 1: i = st.data[st.top].i; j = st.data[st.top].j + 1; break; // Right
            case 2: i = st.data[st.top].i + 1; j = st.data[st.top].j; break; // Down
            case 3: i = st.data[st.top].i; j = st.data[st.top].j - 1; break; // Left
            }
            if (board[i][j] == k)
                find = 1;
        }
        if (find == 1)
        {
            st.data[st.top].di = di;
            st.top++;
            st.data[st.top].i = i;
            st.data[st.top].j = j;
            st.data[st.top].di = -1;
            board[i][j] = -2;
        }
        else
        {
            board[st.data[st.top].i][st.data[st.top].j] = k;
            st.top--;
        }
    }
    for (i = 1; i < 10; i++)
        for (j = 1; j < 10; j++)
            if (board[i][j] == -2)
                board[i][j] = k;
    return true;
}

bool Rules(int xi, int yi, int color) // Trapped stone check, color = board[xi][yi] (Zhang Kaiyue)
{
    int i = xi;
    int j = yi;
    int di = -1;
    int find = 0;

    while (di < 4)
    {
        find = 0;
        int iTemp;
        while (di < 4 && find == 0) // Search for the next possible move
        {
            di++;
            switch (di)
            {
            case 0: i = xi - 1; j = yi; break; // Up
            case 1: i = xi + 1; j = yi; break; // Down
            case 2: i = xi; j = yi + 1; break; // Right
            case 3: i = xi; j = yi - 1; break; // Left
            }
            iTemp = color;
            if (iTemp != 0)
                iTemp = iTemp % 2 + 1; // Convert to opponent's color to analyze from their perspective
            if (board[i][j] == iTemp)
                find = 1;
        }
        if (find == 1)
            if (Rule_Suicide(i, j, iTemp) == true)
                return true;
    }
    return false;
}

int markpoints(int color) // Greedy evaluation function (Zhang Kaiyue)
{
    int point = 0;
    int enemy = color % 2 + 1;
    for (int x = 1; x <= 9; x++)
    {
        for (int y = 1; y <= 9; y++)
        {
            if (put_OK(x, y, color))
                point++;
            if (put_OK(x, y, enemy))
                point--;
        }
    }
    return point;
}

bool put_OK(int xi, int yi, int color) // Determines if a move is valid for AI's color (Zhang Kaiyue)
{
    if (board[xi][yi])
        return false;
    board[xi][yi] = color;
    if (Rule_Suicide(xi, yi, color))
    {
        board[xi][yi] = 0;
        return false;
    }
    if (Rules(xi, yi, color))
    {
        board[xi][yi] = 0;
        return false;
    }
    board[xi][yi] = 0;
    return true;
}

int greedy(int color) // Greedy algorithm for AI move selection (Zhang Kaiyue)
{
    int max_value = -999;
    int best_i[81] = { 0 }, best_j[81] = { 0 }, best_num = 0;
    memset(mark, 0, sizeof(mark));
    for (int i = 1; i <= 9; i++)
    {
        for (int j = 1; j <= 9; j++)
        {
            if (put_OK(i, j, color))
            {
                board[i][j] = color;
                mark[i][j] = markpoints(color);
                if (mark[i][j] > max_value)
                    max_value = mark[i][j];
                board[i][j] = 0;
            }
            else
                mark[i][j] = -99999;
        }
    }
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= 9; j++)
            if (mark[i][j] == max_value && board[i][j] == 0)
            {
                best_i[best_num] = i;
                best_j[best_num] = j;
                best_num++;
            }
    if (best_num == 0) { judge_winner(2); return 0; }
    int random = rand() % best_num; // Randomly select from the best moves
    x = best_i[random];
    y = best_j[random];
    board[x][y] = color;
    return 1;
}




int main() // Main function (Zhou Zixuan)
{
    initgraph(winWidth, winHeigth, 0); // Initialize window, 0 removes EGE startup animation
    setcaption("nogo game"); // Set window title
    setcolor(BLACK); // Set foreground color (text) to black
    setbkcolor(WHITE); // Set background color (window) to white
    setfontbkcolor(WHITE); // Set text background to white (black text on white background)
    setfont(20, 0, "KaiTi");
    print_firstPage();
    if (play_firstpage() == 0)
        return 0; // Clicked "Exit Program"
    cleardevice();
    print_secondPage();
    getch();
    cleardevice();
    print_thirdPage();
    play_thirdpage();
    cleardevice();
    setcolor(BLACK);
    setfont(20, 0, "KaiTi");
    print_background(); // Enter the fourth screen
    for (int i = 1; i <= 9; i++)
        for (int j = 1; j <= 9; j++)
            board[i][j] = 0;
    music_play();
    for (int i = 0; i < 11; i++)
    {
        board[i][0] = 3;
        board[0][i] = 3;
        board[i][10] = 3;
        board[10][i] = 3;
    }
    if (ai_turn == 1) // AI's turn to move
        ai_play();
    endTime = fclock() + 10;
    for (; is_run(); delay_fps(60))
    {
        runTime = fclock();
        remainTime = endTime - runTime; // Current time equals the difference between the two times
        erase(13, 447, 130, 481);
        xyprintf(19, 460, "Countdown: %d", (int)ceil(remainTime));
        if (remainTime <= 0)
        {
            setcolor(RED);
            setfont(30, 0, "STXingkai");
            xyprintf(165, 475, "Time is up, please make your move!");
            setcolor(BLACK);
            setfont(20, 0, "KaiTi");
        }
        while (mousemsg()) // getmouse retrieves mouse messages
        {
            msg = getmouse();
            if (msg.x > 540 && msg.x <= 660 && msg.y > 40 && msg.y < 80 && msg.is_down())
                music_judge(); // Pause or play music
            if (msg.x <= 610 && msg.x > 540 && msg.y > 260 && msg.y < 300 && msg.is_down())
                save(); // Save game
            if (msg.x <= 610 && msg.x > 540 && msg.y > 300 && msg.y < 340 && msg.is_down())
                read(); // Load game
            if (msg.x > 540 && msg.x <= 640 && msg.y > 210 && msg.y < 260 && msg.is_down())
                prompt(); // Show hint
            if (msg.x > 590 && msg.x <= 670 && msg.y > 460 && msg.y < 500 && msg.is_down())
                return 0; // Exit game
            if (msg.x > 590 && msg.x <= 690 && msg.y > 405 && msg.y < 455 && msg.is_down())
                restart(); // Restart game
            if (msg.x > 124 && msg.x <= 459 && msg.y > 63 && msg.y < 402 && msg.is_down())
            { // If clicked inside the board, place a stone
                if (ai_turn == 0) // Player's turn
                {
                    int tem = human_play();
                    if (tem == 0)
                        return 0; // Game over, exit
                    else if (tem == 2) // Game continues
                    {
                        if (legal == 1) // Valid move
                        {
                            endTime = fclock() + 10;
                            erase(80, 20, 260, 40);
                            erase(165, 475, 570, 520);
                            legal = 0;
                        }
                        else // Invalid move
                        {
                            setcolor(RED);
                            xyprintf(80, 20, "This position is already occupied!");
                            setcolor(BLACK);
                        }
                    }
                }
                if (ai_turn == 1) // AI's turn
                    if (ai_play() == 0)
                        return 0;
            }
        }
    }
    return 0;
}
