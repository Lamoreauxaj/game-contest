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

int Cowculator2::calculateMove(const vector<pair<pair<int, int>, int>>& balls, int startingRow) {
    if (balls.size() == 0)
        return 0;
    int dp[balls.size()][gameInfo.numRows];
    int parent[balls.size()][gameInfo.numRows];
    for (int ball = balls.size() - 1; ball >= 0; ball--) {
        cout << balls[ball].first.first << "," << balls[ball].first.second << " " << balls[ball].second << "\n";
        for (int row = 0; row <= gameInfo.numRows - gameInfo.paddleSize; row++) {
            parent[ball][row] = -1;
            int bonus = 0;
            if (row + 2 <= balls[ball].first.second && row + gameInfo.paddleSize - 2 >= balls[ball].first.second) {
                int curr = balls[ball].second; // * (gameInfo.paddleSize - abs(row + gameInfo.paddleSize / 2 - balls[ball].first.second)) / gameInfo.paddleSize * 2;
                if (balls[ball].first.second >= row + gameInfo.paddleSize / 5 && balls[ball].first.second <= row + gameInfo.paddleSize - gameInfo.paddleSize / 5)
                    curr *= 2;
                bonus += curr;
            }
            dp[ball][row] = bonus;
            if (ball == balls.size() - 1) continue;
            for (int vel = 0; vel <= 100; vel += 10) {
                for (int i = -1; i <= 1; i += 2) {
                    vel *= i;
                    int timeDelta = balls[ball + 1].first.first - balls[ball].first.first;
                    int nextRow = row + timeDelta * vel / 1000;
                    if (nextRow < 0) nextRow = 0;
                    if (nextRow + gameInfo.paddleSize >= gameInfo.numRows)
                        nextRow = gameInfo.numRows - 1 - gameInfo.paddleSize;
                    int ans = bonus + dp[ball + 1][nextRow];
                    if (ans > dp[ball][row]) {
                        dp[ball][row] = ans;
                        parent[ball][row] = vel;
                    }
                    vel *= i;
                }
                // dp[ball][row] = max(dp[ball][row], bonus + dp[ball + 1][nextRow]);
                /*
            */
            }
        }
    }
    int bestVel = 0, bestAns = 0;
    for (int vel = 0; vel <= 100; vel += 10) {
        for (int i = -1; i <= 1; i += 2) {
            vel *= i;
            int timeDelta = balls[0].first.first;
            int nextRow = startingRow + timeDelta * vel / 1000;
            if (nextRow < 0) nextRow = 0;
            if (nextRow + gameInfo.paddleSize >= gameInfo.numRows)
                nextRow = gameInfo.numRows - 1 - gameInfo.paddleSize;
            int ans = dp[0][nextRow];
            // cout << vel << " " << ans << " " << startingRow << " " << nextRow << " " << timeDelta << "\n";
            if (ans > bestAns) {
                bestAns = ans;
                bestVel = vel;
            }
            vel *= i;
        }
    }
    // cout << bestAns << "\n";
    // cout << balls.size() << "\n";
    // cout << bestVel << "\n";
    return bestVel;
}

pair<int, int> Cowculator2::findBestMove(const GameState& state) {
    vector<pair<pair<int, int>, int>> balls;
    for (int ballNum = 0; ballNum < state.balls.size(); ballNum++) {
        int time = timeToHit(state.balls[ballNum], state.me);
        if (time >= 2000) continue;
        Ball ball = state.balls[ballNum];
        int timeToReach = (state.me.row - ball.row) / 100;
        if (state.me.row <= ball.row && state.me.row + gameInfo.paddleSize >= ball.row)
            timeToReach = 0;
        else
            timeToReach = (state.me.row + gameInfo.paddleSize - ball.row) / 100;
        if (timeToReach > time) continue;
        int weight = 1; (time - timeToReach + 1000) / 200;
        // balls.push_back({time, calculatePosition(state.balls[ballNum], time).first});
        vector<int> cols;
        for (int i = 0; i < gameInfo.numPaddles * 2; i += 2)
            cols.push_back(state.me.col + 2 * (i + !gameInfo.isLeft) * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1)));
        int ahead = 0;
        int k = 0;
        while (k < cols.size() && cols[k] <= state.balls[ballNum].col)
            k++;
        ahead = k;
        if (!gameInfo.isLeft)
            ahead = gameInfo.numPaddles - ahead;
        // cout << ahead << "\n";
        if (ahead == 3)
            weight *= PADDLE_WEIGHTS[0];
        if (ahead == 2)
            weight *= PADDLE_WEIGHTS[1];
        if (ahead == 1)
            weight *= PADDLE_WEIGHTS[2];
        // cout << ahead << " " << weight << "\n";
        balls.push_back({{time, calculatePosition(state.balls[ballNum], time).first}, weight});
    }
    sort(balls.begin(), balls.end());
    int colVel = 100 * (gameInfo.isLeft ? -1 : 1);
    if (balls.size() > 0) {
        int time = balls[0].first.first;
        int row = balls[0].first.second;
        if (time < 100 && row >= state.me.row && row <= state.me.row + gameInfo.paddleSize)
            colVel *= -STRIKE_PERCENTAGE;
    }
    // return {-100, 0};
    return {calculateMove(balls, state.me.row), STRIKE ? colVel : 0};
    /* 
    int colVel = 100 * (gameInfo.isLeft ? -1 : 1);
    if (balls.size() == 0) return {0, colVel};
    int time = balls[balls.size() - 1].second.first;
    int rowToHit = balls[balls.size() - 1].second.second;
    // int time = timeToHit(ballToHit, state.me);
    if (time < 100)
        colVel = 100 * (gameInfo.isLeft ? 1 : -1);
    // auto pos = calculatePosition(ballToHit, time);
    // return make_pair((state.me.row < ballToHit.row ? 100 : -100), 0);
    return make_pair((state.me.row + gameInfo.paddleSize / 2 < rowToHit ? 100 : -100), colVel);
    */
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
    /*
    vector<int> cols;
    for (int i = 0; i < gameInfo.numPaddles; i++)
        cols.push_back(me.col + 2 * (i + !gameInfo.isLeft) * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1)));
        */
    vector<int> cols;
    for (int i = 0; i < gameInfo.numPaddles * 2; i += 2)
        cols.push_back(me.col + 2 * (i + !gameInfo.isLeft) * (gameInfo.numCols / (4 * gameInfo.numPaddles - 1)));
    if (ball.col_vel == 0) return 1000000;
    if (ball.col < cols[0] && gameInfo.isLeft ||
        ball.col > cols[cols.size() - 1] && !gameInfo.isLeft) return 1000000;
    int k = 0;
    while (k < cols.size() && ball.col >= cols[k]) k++;
    if (k == 0 && ball.col_vel < 0) return 1000000;
    if (k == cols.size() && ball.col_vel > 0) return 1000000;
    if (ball.col_vel > 0 ^ !gameInfo.isLeft) return 1000000;
    if (ball.col_vel < 0) return abs(ball.col - cols[k - 1]) * 1000 / abs(ball.col_vel);
    return abs(ball.col - cols[k]) * 1000 / abs(ball.col_vel);
}
