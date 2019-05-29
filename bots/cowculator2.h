#ifndef COWCULATOR2
#define COWCULATOR2

#include "bot.h"
using namespace std;

const int TIME_DELTA = 200;

struct Ball : public MovingObject {
};

struct Paddle : public MovingObject {
};

struct GameInfo {
    int numRows, numCols, numPaddles, paddleSize;
    int paddleCols;
    bool isLeft;
};

struct GameState {
    Paddle me, opponent;
    vector<Ball> balls;
};

class Cowculator2 : public Bot
{
private:

    GameInfo gameInfo;

    pair<int, int> move(const GameState& state, int timerOffset);
    pair<int, int> findBestMove(const GameState& state);

    pair<int, int> calculatePosition(const Ball& ball, int time);
    int timeToHit(const Ball& ball, const Paddle& me);

public:
    void init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left);
    pair<int, int> move(MovingObject me, MovingObject opponent, vector<MovingObject> balls, int timer_offset);
};

#endif
