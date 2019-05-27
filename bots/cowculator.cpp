#include "cowculator.h"
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

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
  gameInfo.numRows = Nrows * 1000;
  gameInfo.numCols = Ncols * 1000;
  std::cout << gameInfo.numRows << "\n";
  gameInfo.numPaddles = Npaddles_per_player;
  gameInfo.paddleSize = paddle_size * 1000;
  gameInfo.isLeft = am_i_left;
  bot_name = "COWCULATOR";
}

GameState Cowculator::processGameState(const GameState& state) {
  GameState res = state;
  res.me.row *= 1000;
  res.me.col *= 1000;
  res.me.row_vel *= 1000;
  res.me.col_vel *= 1000;
  res.opponent.row *= 1000;
  res.opponent.col *= 1000;
  res.opponent.row_vel *= 1000;
  res.opponent.col_vel *= 1000;
  for (MovingObject& ball : res.balls) {
    ball.row *= 1000;
    ball.col *= 1000;
    ball.row_vel *= 1000;
    ball.col_vel *= 1000;
  }
  return res;
}

std::pair<int, int> Cowculator::move(MovingObject me, MovingObject opponent, std::vector<MovingObject> balls, int timer_offset) {
  GameState state;
  state.me = me;
  state.opponent = opponent;
  state.balls = balls;
  state = processGameState(state);
  return move(state, timer_offset);
}

/*
GameState lastState;

const int EPS = 1000;

void printObject(const MovingObject& obj, const std::string name) {
  std::cout << "Object: " << name << "\n";
  std::cout << "row, col: " << obj.row << ", " << obj.col << "\n";
  std::cout << "row_vel, col_vel: " << obj.row_vel << ", " << obj.col_vel << "\n";
}

void compareObject(const MovingObject& a, const MovingObject& b, const std::string name) {
  if (abs(a.row - b.row) >= EPS || abs(a.col - b.col) >= EPS || abs(a.row_vel - b.row_vel) >= EPS || abs(a.col_vel - b.col_vel) >= EPS) {
    printObject(a, name);
    printObject(b, name);
  }
}

void compareStates(const GameState& a, const GameState& b) {
  // std::cout << "Comparing states\n";
  // compareObject(a.me, b.me, "ME");
  for (int ballNum = 0; ballNum < b.balls.size() && ballNum < a.balls.size(); ballNum++)
    compareObject(a.balls[ballNum], b.balls[ballNum], "BALL " + std::to_string(ballNum));
}
*/

std::pair<int, int> Cowculator::move(const GameState& state, int timerOffset) {
  GameState nextState = approximateNextState(state, timerOffset);
  // nextState = approximateNextState(nextState, 10);
  // nextState = approximateNextState(nextState, 10);
  // compareStates(nextState, state);
  // compareStates(lastState, state);
  // lastState = state;
  return std::make_pair(rand() % 201 - 100, 0);
}

int Cowculator::calculateRankOfState(const GameState& state) {
  int score = 0;
  for (const MovingObject& ball : state.balls) {
    std::vector<MovingObject> myPaddles = getPaddles(state.me, gameInfo.isLeft);
    int section = ball.col / (gameInfo.numCols / (4 * gameInfo.numPaddles - 1));
  }
}

GameState Cowculator::approximateNextState(const GameState& state, int timeDelta) {
  GameState nextState = state;
  for (int ballNum = 0; ballNum < nextState.balls.size(); ballNum++) {
    nextState.balls[ballNum] = moveBall(state.balls[ballNum], timeDelta);
  // for (MovingObject& ball : nextState.balls) {
    // ball = moveBall(ball, timeDelta);
  }
  // std::cout << "MOVED BALLS\n";
  // compareStates(nextState, state);
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
      // compareObject(paddle, nextPaddle, "paddle compare");
      // printObject(paddle, "paddle");
      // printObject(paddle, "nextPaddle");
      if ((ball.col < paddle.col) != (nextBall.col < nextPaddle.col)) {
        // std::cout << "possible reflection off paddle\n";
        int paddleCol = (paddle.col + nextPaddle.col) / 2; // assumption
        int ballRow = (nextBall.row * abs(ball.col - paddleCol) + ball.row * abs(nextBall.col - paddleCol)) / abs(ball.col - nextBall.col);
        int paddleRow = (nextPaddle.row * abs(ball.col - paddleCol) + paddle.row * abs(nextBall.col - paddleCol)) / abs(ball.col - nextBall.col);
        // std::cout << ballRow << " " << paddleCol << "\n";
        if (ballRow >= paddleRow - gameInfo.paddleSize / 2 && ballRow <= paddleRow + gameInfo.paddleSize / 2) {
          // std::cout << "flipped ball on paddle FLIPPED FLIPPED\n";
          nextBall.col_vel *= -1;
          nextBall.col_vel += nextPaddle.col_vel;
          nextBall.row_vel += round(.25 * nextPaddle.row_vel);
          nextBall.col = nextPaddle.col - (nextBall.col - nextPaddle.col);
        }
      }
    }
  }
  return nextState;
}

MovingObject Cowculator::movePaddle(const MovingObject& paddle, int timeDelta) {
  MovingObject obj = paddle;
  obj.row = round(obj.row + timeDelta * obj.row_vel / 1000.0);
  obj.col = round(obj.col + timeDelta * obj.col_vel / 1000.0);
  obj.row = capValue(obj.row, 0, gameInfo.numRows - 1 - gameInfo.paddleSize);
  obj.col = capValue(obj.col, 0, gameInfo.numCols / (4 * gameInfo.numPaddles - 1) - 1);
  return obj;
}

MovingObject Cowculator::moveBall(const MovingObject& ball, int timeDelta) {
  MovingObject nextBall = ball;
  nextBall.row = round(nextBall.row + timeDelta * nextBall.row_vel / 1000.0);
  nextBall.col = round(nextBall.col + timeDelta * nextBall.col_vel / 1000.0);
  /*
  if (nextBall.col >= gameInfo.numCols)
    nextBall.col = gameInfo.numCols - (nextBall.col - gameInfo.numCols + 1);
  if (nextBall.col < 0)
    nextBall.col = -nextBall.col - 1;
  */
  if (nextBall.row >= gameInfo.numRows) {
    nextBall.row = round(ball.row - timeDelta * nextBall.row_vel / 1000.0);
    // nextBall.row = gameInfo.numRows - (nextBall.row - gameInfo.numRows + 1000);
    nextBall.row_vel *= -1;
    // std::cout << "ball reflects on ceiling\n";
  }
  if (nextBall.row < 0) {
    nextBall.row = round(ball.row - timeDelta * nextBall.row_vel / 1000.0);
    // nextBall.row = -nextBall.row - 1;
    nextBall.row_vel *= -1;
    // std::cout << "ball reflects on ceiling\n";
  }
  // compareObject(ball, nextBall, "COMPARED BALLS");
  return nextBall;
}

int capValue(int value, int min, int max) {
  if (value >= max) value = max;
  if (value <= min) value = min;
  return value;
}
