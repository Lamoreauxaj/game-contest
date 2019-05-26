#include "cowculator.h"
#include <cstdlib>
#include <cmath>

std::vector<MovingObject> Cowculator::getPaddles(const MovingObject& paddle, bool isLeft) {
  std::vector<MovingObject> res;
  for (int i = 0; i < 2 * gameInfo.numPaddles; i++) {
    if (i % 2 != isLeft) {
      MovingObject obj = paddle;
      obj.col += 2 * i * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1));
      res.push_back(obj);
    }
  }
  return res;
}

void Cowculator::init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left) {
  gameInfo.numRows = Nrows;
  gameInfo.numCols = Ncols;
  gameInfo.numPaddles = Npaddles_per_player;
  gameInfo.paddleSize = paddle_size;
  gameInfo.isLeft = am_i_left;
  bot_name = "COWCULATOR";
}

std::pair<int, int> Cowculator::move(MovingObject me, MovingObject opponent, std::vector<MovingObject> balls, int timer_offset) {
  GameState state;
  state.me = me;
  state.opponent = opponent;
  state.balls = balls;
  return move(state, timer_offset);
}

std::pair<int, int> Cowculator::move(const GameState& state, int timerOffset) {
  return std::make_pair(rand() % 201 - 100, 0);
}

int Cowculator::calculateRankOfState(const GameState& state) {
  return 0;   
}

GameState Cowculator::approximateNextState(const GameState& state, int timeDelta) {
  GameState nextState = state;
  for (MovingObject& ball : nextState.balls) {
    ball = moveBall(ball, timeDelta);
  }
  nextState.me = movePaddle(nextState.me, timeDelta);
  nextState.opponent = movePaddle(nextState.opponent, timeDelta);

  std::vector<MovingObject> myPaddles = getPaddles(state.me, gameInfo.isLeft);
  std::vector<MovingObject> opponentPaddles = getPaddles(state.opponent, !gameInfo.isLeft);
  std::vector<MovingObject> paddles = myPaddles;
  for (MovingObject paddle : opponentPaddles) paddles.push_back(paddle);

  myPaddles = getPaddles(nextState.me, gameInfo.isLeft);
  opponentPaddles = getPaddles(nextState.opponent, !gameInfo.isLeft);
  std::vector<MovingObject> nextPaddles = myPaddles;
  for (MovingObject paddle : opponentPaddles) nextPaddles.push_back(paddle);

  for (int ballNum = 0; ballNum < state.balls.size(); ballNum++) {
    const MovingObject& ball = state.balls[ballNum];
    MovingObject& nextBall = nextState.balls[ballNum];
    for (int paddleNum = 0; paddleNum < paddles.size(); paddleNum++) {
      MovingObject& paddle = paddles[paddleNum];
      MovingObject& nextPaddle = nextPaddles[paddleNum];
      if ((ball.col < paddle.col) != (nextBall.col < nextPaddle.col)) {
        nextBall.col_vel *= -1;
        nextBall.col_vel += nextPaddle.col_vel;
        nextBall.row_vel += round(.25 * nextPaddle.row_vel);
        nextBall.col = nextPaddle.col - (nextBall.col - nextPaddle.col);
      }
    }
  }
  return state;
}

MovingObject Cowculator::movePaddle(const MovingObject& paddle, int timeDelta) {
  MovingObject obj = paddle;
  obj.row = round(obj.row + timeDelta / 1000.0 * obj.row_vel);
  obj.col = round(obj.col + timeDelta / 1000.0 * obj.col_vel);
  obj.row = capValue(obj.row, 0, gameInfo.numRows - 1);
  obj.col = capValue(obj.col, 0, gameInfo.numCols / (4 * gameInfo.numPaddles - 1) - 1);
  return obj;
}

MovingObject Cowculator::moveBall(const MovingObject& ball, int timeDelta) {
  MovingObject nextBall = ball;
  nextBall.row = round(nextBall.row + timeDelta / 1000.0 * nextBall.row_vel);
  nextBall.col = round(nextBall.col + timeDelta / 1000.0 * nextBall.col_vel);
  /*
  if (nextBall.col >= gameInfo.numCols)
    nextBall.col = gameInfo.numCols - (nextBall.col - gameInfo.numCols + 1);
  if (nextBall.col < 0)
    nextBall.col = -nextBall.col - 1;
  */
  if (nextBall.row >= gameInfo.numRows) {
    nextBall.row = gameInfo.numRows - (nextBall.row - gameInfo.numRows + 1);
    nextBall.row_vel *= -1;
  }
  if (nextBall.row < 0) {
    nextBall.row = -nextBall.row - 1;
    nextBall.row_vel *= -1;
  }
  return nextBall;
}

