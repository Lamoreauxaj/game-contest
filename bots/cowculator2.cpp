#include "cowculator2.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

void Cowculator2::init(int Nrows, int Ncols, int Npaddles_per_player, int paddle_size, bool am_i_left) {
    this->gameInfo.numRows = Nrows;
    this->gameInfo.numCols = Ncols;
    this->gameInfo.numPaddles = Npaddles_per_player;
    this->gameInfo.paddleSize = paddle_size;
    this->gameInfo.isLeft = am_i_left;
    this->gameInfo.paddleCols = this->gameInfo.numCols / (4 * this->gameInfo.numPaddles - 1);
    bot_name = "COWCULATOR_2";
}

pair<int, int> Cowculator2::move(MovingObject me, MovingObject opponent, vector<MovingObject> balls, int timer_offset) {
    GameState state;
    state.me.row = me.row;
    state.me.col = me.col;
    state.me.row_vel = me.row_vel;
    state.me.col_vel = me.col_vel;
    state.opponent.row = opponent.row;
    state.opponent.col = opponent.col;
    state.opponent.row_vel = opponent.row_vel;
    state.opponent.col_vel = opponent.col_vel;
    for (const MovingObject& ball : balls) {
        state.balls.push_back(Ball());
        state.balls.back().row = ball.row;
        state.balls.back().col = ball.col;
        state.balls.back().row_vel = ball.row_vel;
        state.balls.back().col_vel = ball.col_vel;
    }
    return move(state, timer_offset);
}

pair<int, int> Cowculator2::move(const GameState& state, int timerOffset) {
    pair<int, int> currentMove = findBestMove(state);
    return currentMove;
}

pair<int, int> Cowculator2::findBestMove(const GameState& state) {
    vector<pair<int, int>> balls;
    for (int ballNum = 0; ballNum < state.balls.size(); ballNum++) {
        int time = timeToHit(state.balls[ballNum], state.me);
        if (time >= 3000) continue;
        vector<int> cols;
        for (int i = 0; i < gameInfo.numPaddles; i++)
            cols.push_back(state.me.col + 2 * (i + !gameInfo.isLeft) * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1)));
        int ahead = 0;
        int k = 0;
        while (k < cols.size() && cols[k] <= state.balls[ballNum].col)
            k++;
        if (!gameInfo.isLeft)
            ahead = gameInfo.numPaddles - ahead;
        int weight = (ahead == 1 ? 1000 : 1) * 100; // * (4000 - time) / 2000;
        cout << ahead << "\n";
        balls.push_back({weight, ballNum});
    }
    sort(balls.begin(), balls.end());
    int colVel = 100 * (gameInfo.isLeft ? -1 : 1);
    if (balls.size() == 0) return {0, colVel};
    Ball ballToHit = state.balls[balls[balls.size() - 1].second];
    int time = timeToHit(ballToHit, state.me);
    if (time < 100)
        colVel = 100 * (gameInfo.isLeft ? 1 : -1);
    auto pos = calculatePosition(ballToHit, time);
    // return make_pair((state.me.row < ballToHit.row ? 100 : -100), 0);
    return make_pair((state.me.row + gameInfo.paddleSize / 2 < calculatePosition(ballToHit, time).first ? 100 : -100), colVel);
}

pair<int, int> Cowculator2::calculatePosition(const Ball& ball, int time) {
    pair<int, int> pos = make_pair(ball.row + ball.row_vel * time / 1000, ball.col + ball.col_vel * time / 1000);
    int bounces = abs(pos.first / gameInfo.numRows);
    if (pos.first < 0) bounces++;
    pos.first = (pos.first % gameInfo.numRows + gameInfo.numRows) % gameInfo.numRows;
    if (bounces % 2 != 0)
        pos.first = gameInfo.numRows - 1 - pos.first;
    return pos;
}

int Cowculator2::timeToHit(const Ball& ball, const Paddle& me) {
    vector<int> cols;
    for (int i = 0; i < gameInfo.numPaddles; i++)
        cols.push_back(me.col + 2 * (i + !gameInfo.isLeft) * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1)));
    if (ball.col_vel == 0) return 1000000;
    if (ball.col < cols[0] && gameInfo.isLeft ||
        ball.col > cols[cols.size() - 1] && !gameInfo.isLeft) return 1000000;
    int k = 0;
    while (k < cols.size() && ball.col >= cols[k] + 5) k++;
    if (k == 0 && ball.col_vel < 0) return 1000000;
    if (k == cols.size() && ball.col_vel > 0) return 1000000;
    if (ball.col_vel > 0 ^ !gameInfo.isLeft) return 1000000;
    if (ball.col_vel < 0) return abs(ball.col - cols[k - 1]) * 1000 / abs(ball.col_vel);
    return abs(ball.col - cols[k]) * 1000 / abs(ball.col_vel);
}
