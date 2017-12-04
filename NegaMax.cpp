/*
This function is a recursive negaMax algorithm
It executes every move that can be played by the AI to a certain depth and remembers the highest scoring column
Stops when the game is won or until it hits a depth of 0

Returns the highest score that can be played by the AI
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
