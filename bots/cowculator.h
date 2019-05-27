#ifndef COWCULATOR
#define COWCULATOR
#include "bot.h"

int capValue(int value, int min, int max);

struct GameInfo {
  int numRows, numCols, numPaddles, paddleSize;
  bool isLeft;
};

struct GameState {
  MovingObject me, opponent;
  std::vector<MovingObject> balls;
};

class Cowculator : public Bot
{
private:
  GameInfo gameInfo;
  std::pair<int, int> move(const GameState& state, int timerOffset);
  int calculateRankOfState(const GameState& state);
  GameState processGameState(const GameState& state);
  GameState approximateNextState(const GameState& state, int timeDelta);
  MovingObject movePaddle(const MovingObject& obj, int timeDelta);
  MovingObject moveBall(const MovingObject& ball, int timeDelta);
  std::vector<MovingObject> getPaddles(const MovingObject& paddle, bool isLeft);
public:
  void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left);
  std::pair<int,int> move(MovingObject me, MovingObject opponent, std::vector<MovingObject> balls, int timer_offset);
};

#endif
