#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <limits>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

using namespace std;

/*
Global variables
*/
const int w = 7;//columns
const int h = 6;//rows
bool gameWon;

/*
This is where the board and all the important variables get stored
*/
struct Memory{
  int board[w][h];
  int height[w];
  int player;
  int column;
  int piecesPlaced;
};

/*
Declarations of all the functions
*/
int main();
int openGame(Memory &mem);
void build(Memory &mem);
int play(Memory &mem);
int endScreen(Memory &mem);
void display(Memory &mem);
bool canPlay(Memory &mem);
void makeMove(Memory &mem);
void writeLog(Memory &mem, char functionName[]);
bool check(Memory &mem);
bool checkDiagonal(Memory &mem);
int computerTurn(Memory &mem, int depth);
int negaMax(Memory &mem, int depth, int alpha, int beta);
int heuristic(Memory &mem);
int computerDifficulty(Memory &mem);
int getMax(Memory &mem, int a, int b);
void openLog();
void closeLog();
void echo(Memory &mem);
void endGameResultsEcho(Memory &mem);
unsigned long long getTimeMicrosec();

/*
Declarations of global variables
*/
ofstream outfile;
char filename[7];//log.txt

/*
Definitions
*/
#define INT_MAX 2147483647;

/*
Main function
Returns 0 when the program is terminated.
*/
void print(Memory &mem)
{
  for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            cout << mem.board[j][i] << ' ';
        }
        cout << endl;
    }

    for(int i = 0; i < w; i++)
    {
      cout << mem.height[i] << endl;
    }
}

int main()
{
  cout << "Enter logfileName: " << endl;
  cin >> filename;
  Memory mem;

  //Begin logging
  openLog();

  openGame(mem);

  //Ends logging
  closeLog();
  return 0;
}

/*
This function opens the log file
*/
void openLog()
{
  outfile.open(filename);
  cout << "Log has been opened." << endl;
}

/*
This function closes the log file
*/
void closeLog()
{
  outfile.close();
  cout << "Log has been closed." << endl;
}

/*
This function writes to the log file

Returns 1 if it successfully writes
Returns -1 if it unsuccessfully writes
*/
void writeLog(Memory &mem, char functionName[])
{
  time_t sysTimeMS = time(nullptr);
  //long double sysTimeMS = time(0)*1000;
  //outfile << sysTimeMS << "\t" << "Board State: " << mem.player << endl;
  outfile << asctime(localtime(&sysTimeMS)) << "\t" << "Function: " << functionName << endl;
}

/*
getTime
*/
unsigned long long getTimeMicrosec()
{
  timeval NOW;
  gettimeofday(&NOW, NULL);
  return NOW.tv_sec*1000000LL + NOW.tv_usec;
}

/*
Function that allows players to play again after the first game ends
Returns 0 if an error occured in the game.

***Need to implement ability to play again.
*/
int openGame(Memory &mem)
{
  //Write logging
  char functionName[] = "openGame()";
  writeLog(mem, functionName);

  build(mem);
  int temp = play(mem);
  if(temp < 0)
  {
    cout << "Game has ended for some reason." << endl;
    return 0;
  }
}

/*
This function builds the initial board state and sets all the variables to an initial state
*/
void build(Memory &mem)
{
  //Write logging
  char functionName[] = "build()";
  writeLog(mem, functionName);

  gameWon = false;
  mem.piecesPlaced = 0;
  mem.column = 0;
  mem.player = 1;//player always goes first

  for(int r = 0; r < h; r++)
  {
    for(int c = 0; c < w; c++)
    {
      mem.board[c][r] = 0;//like a coordinate system: x,y = columns,rows
    }
  }

  for(int c = 0; c < w; c++)
  {
    mem.height[c] = 0;
  }
}

/*
This function is the main play function where the human player plays against the ai
Returns > 0 if game ran normally
Returns -1 if game was abnormal

***Need to implement ability to have human player go second.
*/
int play(Memory &mem)
{
  //Write logging
  char functionName[] = "play()";
  writeLog(mem, functionName);

  display(mem);

  char line;
  int depth = computerDifficulty(mem);
  while(depth < 0)
  {
    depth = computerDifficulty(mem);
  }
  line = 0;
  while(!gameWon)
  {
    if(mem.player == 1)//let person play first
    {
      while(true && mem.piecesPlaced != 42)//check if all pieces on the board have been played
      {
        cout << "Where do you want to place your piece?" << endl;
        cin >> line;
        if(line >= '1' && line <= '7')//can place between columns 1-7
        {
          mem.column = int(line) - 48 - 1;//offset for array bounds
          break;
        }
        else
        {
          cout << "Between 1 and 7 only." << endl;
        }
      }
      if(!canPlay(mem))//check if that column is full
      {
        cout << "Column is full. Try again." << endl;
      }
      else//make the actual move on the board
      {
        makeMove(mem);
        echo(mem);
        //print(mem);//testing
        gameWon = check(mem);
        if(!gameWon)
        {
          mem.player = 2;
        }
        display(mem);
      }
    }
    else//AI's turn
    {
      //cout << "Thinking..." << endl;
      //AI time logging
      unsigned long long start_time = getTimeMicrosec();
      mem.column = computerTurn(mem, depth);
      makeMove(mem);
      unsigned long long end_time = getTimeMicrosec();
      outfile << "Time taken by AI:" << "\t" << (end_time - start_time)*0.0000001 << endl;
      echo(mem);
      gameWon = check(mem);
      display(mem);
      if(!gameWon)
      {
        mem.player = 1;
      }
    }
  }
  int end = endScreen(mem);
  endGameResultsEcho(mem);
  return end;
}

/*
This function sets the AI difficulty

Returns the depth level if it is within the bounds
*/
int computerDifficulty(Memory &mem)
{
  //Write logging
  char functionName[] = "computerDifficulty()";
  writeLog(mem, functionName);

  char line;
  int depth;
  cout << "What AI difficulty? Choose between 1 and 9." << endl;
  cin >> line;
  if(line >= '1' && line <= '9')
  {
    depth = int(line) - 48;
    return depth;
  }
  else
  {
    cout << "Between 1 and 9 only." << endl;
    return -1;
  }
}

/*
This function determines who played the last piece that won the game

Returns > 0 if it can determine who gameWon
Returns < 0 if something went wrong
*/
int endScreen(Memory &mem)
{
  //Write logging
  char functionName[] = "endScreen()";
  writeLog(mem, functionName);

  if(mem.piecesPlaced == 42)
  {
    cout << "Tie." << endl;
    system("pause");
    return 0;
  }
  else if(mem.player == 1)
  {
    cout << "Human won." << endl;
    system("pause");
    return 1;
  }
  else if(mem.player == 2)
  {
    cout << "AI won." << endl;
    system("pause");
    return 1;
  }
  return -1;
}

/*
This function can be used by any function to print out the current board state

***For visual purposes
*/
void display(Memory &mem)
{
  //Write logging
  char functionName[] = "display()";
  writeLog(mem, functionName);

  cout << 1 << " " << 2 << " " << 3 << " " << 4 << " " << 5 << " " << 6 << " " << 7 << endl << endl;
  for(int r = 0; r < h; r++)
  {
    for(int c = 0; c < w; c++)
    {
      cout << mem.board[c][5 - r] << " ";//inverted y-axis display
    }
    cout << endl;
  }
}

/*
This function determines whether or not the current column can be played
Returns true if piece can be played at that columns
Returns false if that piece cannot be played
*/
bool canPlay(Memory &mem)
{
  //Write logging
  // char functionName[] = "canPlay()";
  // writeLog(mem, functionName);

  if(mem.height[mem.column] < h && (mem.column >= 0 && mem.column <= 6))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
This function plays the column indicated in the Memory struct and updates the rest of the Memory struct
*/
void makeMove(Memory &mem)
{
  //Write logging
  // char functionName[] = "makeMove()";
  // writeLog(mem, functionName);

  mem.board[mem.column][mem.height[mem.column]] = mem.player;
  mem.height[mem.column] = mem.height[mem.column] + 1;
  mem.piecesPlaced++;
}

/*
This function determines whether or not at the current state, the game is won by the current player
Returns true if game is Finished
Returns false if game is not Finished
*/
bool check(Memory &mem)
{
  //Write logging
  // char functionName[] = "check()";
  // writeLog(mem, functionName);

  for(int c = 0; c <= w - 4; c++)
  {
    for(int r = 0; r < h; r++)
    {
      if(mem.board[c][r] == mem.player && mem.board[c + 1][r] == mem.player && mem.board[c + 2][r] == mem.player && mem.board[c + 3][r] == mem.player)
      {
        return true;
      }
    }
  }

  int counterPlayer = 0;

  //check vertical
  counterPlayer = 0;
  for(int c = 0; c < w; c++)
  {
    if(mem.height[c] >= 4)
    {
      for(int r = 0; r < h; r++)
      {
        if(counterPlayer >= 4)
        {
          return true;
        }
        if(mem.board[c][r] == mem.player)
        {
          counterPlayer++;
        }
        else
        {
          counterPlayer = 0;
        }
      }
    }
  }

  //check diagonal
  if(checkDiagonal(mem))
  {
    return true;
  }

  return false;
}

/*
This function is a helper function for check()
Checks the diagonal win condition in the current board state
Returns true if diagonal win
Returns false if no diagonal win
*/
bool checkDiagonal(Memory &mem)
{
  //Write logging
  // char functionName[] = "checkDiagonal()";
  // writeLog(mem, functionName);

  int player = mem.player;
  int x = mem.column;
  int y = mem.height[x] - 1;

  int diagonal1[6] = {0}; //largest possible diagonal size
  int diagonal2[6] = {0};

  //fill diagonal 1
  int min = (x < y) ? x : y; //min between x and y
  int startX = x - min;
  int startY = y - min;
  int i = 0; //used for x
  int j = 0; //used for y
  int k = 0; //counter for diagonal

  while(startX+i<7 && startY+j<6)
  {
	  diagonal1[k] = mem.board[startX+i][startY+j];
	  i++;
    j++;
    k++;
  }

  //fill diagonal 2
  min = (6 - x < y) ? 6 - x : y;
  int startX2 = x+min;
  int startY2 = y-min;
  i = 0;
  j = 0;
  k = 0;

  while(startX2-i>0 && startY2+j<6){
	  diagonal2[k] = mem.board[startX2-i][startY2+j];
	  i++;
    j++;
    k++;
  }

  int count1 = 0;
  int count2 = 0;
  for(i=0; i<6; i++){
	  if(diagonal1[i]==player) count1 ++;
	  if(diagonal2[i]==player) count2 ++;
  }
  if(count1>=4||count2>=4) return true;
  return false;
}

void copyStruct(Memory &mem, Memory &mem2)
{
  //Write logging
  // char functionName[] = "copyStruct()";
  // writeLog(mem, functionName);

  //copy board state
  for(int r = 0; r < h; r++)
  {
    for(int c = 0; c < w; c++)
    {
      int temp = mem.board[c][r];
      mem2.board[c][r] = temp;
    }
  }
  //copy height state
  for(int c = 0; c < w; c++)
  {
    int temp = mem.height[c];
    mem2.height[c] = temp;
  }
  //copy rest of the variables
  int temp1 = mem.player;
  int temp2 = mem.column;
  int temp3 = mem.piecesPlaced;
  mem2.player = temp1;
  mem2.column = temp2;
  mem2.piecesPlaced = temp3;
}

/*
This function is the computer's turn
Initializes the negaMax algorithm by playing the turn's piece in every columns
Uses a seperate copy of Memory, mem2, in order to look deeper into the game
Returns the column that the AI determines to be the highest scoring column
*/
int computerTurn(Memory &mem, int depth)
{
  //Write logging
  char functionName[] = "computerTurn()";
  writeLog(mem, functionName);

  int move;
  int highScore = -1*INT_MAX;
  int currScore = -1;
  int alpha = -1*INT_MAX;
  int beta = INT_MAX;
  Memory mem2;
  copyStruct(mem, mem2);

  for(int c = 0; c < w; c++)
  {
    mem2.column = c;
    if(canPlay(mem2))
    {
      makeMove(mem2);
      mem2.player = 1;
      currScore = -1*negaMax(mem2, depth, alpha, beta);//now it's player1's turn
      if(currScore > highScore)
      {
        highScore = currScore;
        move = c;
      }
    }
  }
  cout << "Finished Thinking..." << endl;
  return move;
}

/*
This function is a recursive negaMax algorithm
It executes every move that can be played by the AI to a certain depth and remembers the highest scoring column
Stops when the game is won or until it hits a depth of 0
Returns the highest score that can be played by the AI

***Not tested
*/
int negaMax(Memory &mem, int depth, int alpha, int beta)
{
  //Write logging
  // char functionName[] = "negaMax()";
  // writeLog(mem, functionName);

  if(check(mem) && mem.piecesPlaced == 42)
  {
    return 0;//tie
  }
  else if(check(mem) && mem.player == 2)
  {
    return 10000;
  }
  else if(check(mem) && mem.player == 1)
  {
    return -10000;
  }

  if(depth == 0)
  {
    if(mem.player == 1)
    {
      return heuristic(mem);
    }
    else
    {
      return -1*heuristic(mem);
    }
  }

  int highScore = -1*w*h;
  int negaMax_val;

  Memory mem2;
  copyStruct(mem, mem2);
  for(int c = 0; c < w; c++)
  {
    mem2.column = c;
    if(canPlay(mem2))
    {
      makeMove(mem2);
      mem2.player = 3 - mem2.player;
      negaMax_val = -1*negaMax(mem2, depth - 1, -1*beta, -1*alpha);
      if(negaMax_val > highScore)
      {
        highScore = negaMax_val;
      }
      if(negaMax_val > alpha)
      {
        alpha = negaMax_val;
      }
      if(alpha >= beta)
      {
        return alpha;
      }
    }
  }
  return highScore;
}

/*
This function calculates the final heuristic of the board based on player pieces on the board
Returns the result of that heuristic

***Not tested
*/
int heuristic(Memory &mem)
{
  //Write logging
  // char functionName[] = "heuristic()";
  // writeLog(mem, functionName);

  int result = 0;
  //check horizontals
  for(int r = 0; r < h; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] != 2 && mem.board[c][r + 1] != 2 && mem.board[c][r + 2] != 2 && mem.board[c][r + 3] != 2)
      {
        result++;
      }
      if(mem.board[c][r] != 1 && mem.board[c][r + 1] != 1 && mem.board[c][r + 2] != 1 && mem.board[c][r + 3] != 1)
      {
        result--;
      }
    }
  }

  //check verticals
  for(int r = 0; r <= h - 4; r++)
  {
    for(int c = 0; c < w; c++)
    {
      if(mem.board[c][r] != 2 && mem.board[c + 1][r] != 2 && mem.board[c + 2][r] != 2 && mem.board[c + 3][r] != 2)
      {
        result++;
      }
      if(mem.board[c][r] != 1 && mem.board[c + 1][r] != 1 && mem.board[c + 2][r] != 1 && mem.board[c + 3][r] != 1)
      {
        result--;
      }
    }
  }

  //implement diagonal heuristic Checks
  //right diagonal check
  for(int r = 0; r <= h - 4; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] != 2 && mem.board[c + 1][r + 1] != 2 && mem.board[c + 2][r + 2] != 2 && mem.board[c + 3][r + 3] != 2)
      {
        result++;
      }
      if(mem.board[c][r] != 1 && mem.board[c + 1][r + 1] != 1 && mem.board[c + 2][r + 2] != 1 && mem.board[c + 3][r + 3] != 1)
      {
        result--;
      }
    }
  }

  //left diagonal check
  for(int r = h - 3; r < h; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] != 2 && mem.board[c + 1][r - 1] != 2 && mem.board[c + 2][r - 2] != 2 && mem.board[c + 3][r - 3] != 2)
      {
        result++;
      }
      if(mem.board[c][r] != 1 && mem.board[c + 1][r - 1] != 1 && mem.board[c + 2][r - 2] != 1 && mem.board[c + 3][r - 3] != 1)
      {
        result--;
      }
    }
  }

  return result;
}

/*
This function computes the larger int

Returns the larger int
*/
int getMax(Memory &mem, int a, int b)
{
  //Write logging
  char functionName[] = "getMax()";
  writeLog(mem, functionName);

  int max = a;
  if(b > max)
  {
    return b;
  }
  return max;
}

void echo(Memory &mem)
{
  //echo
  int i;
  string s = "echo";
  string c = to_string(mem.column);
  string p = to_string(mem.player);
  string str = s + " \"" + c + p + "\" > /dev/ttyS1";
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());

  //cout << cstr << endl;

  if(system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
  i = system(cstr);
}

void endGameResultsEcho(Memory &mem)
{
  int r1;
  int r2;
  int c1;
  int c2;

  //check horizontals
  for(int r = 0; r < h; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] == mem.player && mem.board[c][r + 1] == mem.player && mem.board[c][r + 2] == mem.player && mem.board[c][r + 3] == mem.player)
      {
        r1 = r;
        c1 = c;
        r2 = r + 3;
        c2 = c;
      }
    }
  }

  //check verticals
  for(int r = 0; r <= h - 4; r++)
  {
    for(int c = 0; c < w; c++)
    {
      if(mem.board[c][r] == mem.player && mem.board[c + 1][r] == mem.player && mem.board[c + 2][r] == mem.player && mem.board[c + 3][r] == mem.player)
      {
        r1 = r;
        c1 = c;
        r2 = r;
        c2 = c + 3;
      }
    }
  }

  //implement diagonal heuristic Checks
  //right diagonal check
  for(int r = 0; r <= h - 4; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] == mem.player && mem.board[c + 1][r + 1] == mem.player && mem.board[c + 2][r + 2] == mem.player && mem.board[c + 3][r + 3] == mem.player)
      {
        r1 = r;
        c1 = c;
        r2 = r + 3;
        c2 = c + 3;
      }
    }
  }

  //left diagonal check
  for(int r = h - 3; r < h; r++)
  {
    for(int c = 0; c <= w - 4; c++)
    {
      if(mem.board[c][r] == mem.player && mem.board[c + 1][r - 1] == mem.player && mem.board[c + 2][r - 2] == mem.player && mem.board[c + 3][r - 3] == mem.player)
      {
        r1 = r;
        c1 = c;
        r2 = r - 3;
        c2 = c + 3;
      }
    }
  }

  //echo i > 62
  int i;
  string str = "echo \"63\" > /dev/ttyS1";
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());

  if(system(NULL)) puts ("Ok");
    else exit (EXIT_FAILURE);
  i = system(cstr);

  //echo winning set of 2 coordinates
  // int j;
  // string s = "echo";
  // string c = to_string(c1);
  // string p = to_string(r1);
  // string str_coor = s + " \"" + c + p + "\" > /dev/ttyS1";
  // char *cstr_coor = new char[str_coor.length() + 1];
  // strcpy(cstr_coor, str_coor.c_str());
  //
  // if(system(NULL)) puts ("Ok");
  //   else exit (EXIT_FAILURE);
  // j = system(cstr_coor);
  //
  // int k;
  // c = to_string(c2);
  // p = to_string(r2);
  // string str_coor2 = s + " \"" + c + p + "\" > /dev/ttyS1";
  // char *cstr_coor2 = new char[str_coor2.length() + 1];
  // strcpy(cstr_coor2, str_coor2.c_str());
  //
  // if(system(NULL)) puts ("Ok");
  //   else exit (EXIT_FAILURE);
  // k = system(cstr_coor2);
}
