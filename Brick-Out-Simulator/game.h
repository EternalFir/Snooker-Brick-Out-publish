#ifndef BRICK_OUT_GAME_H
#define BRICK_OUT_GAME_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>

class Game {
public:
    static const int k_max_n = 1e3;

    std::istream &map;


    int k, n, m, s;

    int op_cnt;
    int touch_cnt;
    int reward;

    class Map {
    public:
        int bricks[2 * k_max_n + 1][2 * k_max_n + 1];
        std::array<std::bitset<2 * k_max_n + 1>, 2 * k_max_n + 1> hit;
        int hits = 0;

        Map() {
            for (int i = 0; i < 2 * k_max_n + 1; ++i) {
                for (int j = 0; j < 2 * k_max_n + 1; ++j) {
                    bricks[i][j] = -1;
                }
            }
        }

        int get_color(const int x, const int y, const int n) {
            if (x < -2 * n || x > 2 * n || y > 3 * n || y <= 0)
                return 10;
            if (x < -n || x > n || !x)
                return -1;
            if (y <= n)
                return -1;
            if (hit[x + n][y - n]) return -1;
            return bricks[x + n][y - n];
        }

        void set_hit(const int x, const int y, const int n) {
            if (x >= -n && x <= n && y > 3 * n)
                return;
            if (x < -n || x > n || !x)
                throw std::string("set_hit: index out of range.");
            if (y <= n || y > 3 * n)
                throw std::string("set_hit: index out of range.");
            ++hits;
            hit[x + n][y - n] = true;
        }
    };


    inline const int get_x_direction(const int &v) {
        if (!v)
            return 0;
        return v < 0 ? -1 : 1;
    }

    struct Ball {
    public:
        int x, y;
        int vx, vy;
        int color_before;

        void init() {
            x = 0;
            y = 0;
            vx = 0;
            vy = -1;
            color_before = 0;
        }
    };

    struct Brick {
    public:
        int x;
        int y;
        int color = -1;
    };

    std::queue<Brick> touched_bricks;

    inline int check_op(const char &op) {
        switch (op) {
            case 'A':
                return -2;
            case 'B':
                return -1;
            case 'C':
                return 0;
            case 'D':
                return 1;
            case 'E':
                return 2;
            default:
                throw std::string("check_op: wrong option.");
        }
    }

    inline void special_reward_handler(const int &color_now) {
        if (color_now == situation_now.ball.color_before + 1)
            situation_now.ball.color_before++;
        else
            situation_now.ball.color_before = 0;
        if (situation_now.ball.color_before == 3) {
            reward += 50;
            situation_now.ball.color_before = 0;
        }
    }

    inline const int x_left(const int &x) {
        return x <= 0 ? x - 1 : x;
    }

    inline const int x_right(const int &x) {
        return x >= 0 ? x + 1 : x;
    }

    int move(char op) {
        bool end = false;
        while (1) {
            int next_y = situation_now.ball.y + situation_now.ball.vy;
            // check y reflection
            if (next_y < 0) { // touch the bottom
                if (!end) {
                    next_y = -next_y;
                    situation_now.ball.vy = -situation_now.ball.vy;
                    situation_now.ball.vx += check_op(op);
                    op_cnt++;
                    end = true;
                } else {
                    break;
                }
            }
            // check x reflection
            int x_move_length = abs(situation_now.ball.vx);
            int check_y = std::max(situation_now.ball.y, next_y);
            int check_x = situation_now.ball.x;
            if (situation_now.ball.vx) {
                bool first_step = true;
                while (x_move_length) {
                    {
                        if (first_step) {
                            Brick A, B, C;
                            if (situation_now.ball.vx > 0) {
                                A.color = situation_now.map.get_color(x_right(check_x), check_y, n);
                                A.x = x_right(check_x);
                                A.y = check_y;
                                B.color = situation_now.map.get_color(x_right(check_x) - 1, check_y, n);
                                B.x = x_right(check_x) - 1;
                                B.y = check_y;
                                if (situation_now.ball.vy > 0) {
                                    C.color = situation_now.map.get_color(x_right(check_x), check_y - 1, n);
                                    C.x = x_right(check_x);
                                    C.y = check_y - 1;
                                } else {
                                    C.color = situation_now.map.get_color(x_right(check_x), check_y + 1, n);
                                    C.x = x_right(check_x);
                                    C.y = check_y + 1;
                                }
                            } else {
                                A.color = situation_now.map.get_color(x_left(check_x), check_y, n);
                                A.x = x_left(check_x);
                                A.y = check_y;
                                B.color = situation_now.map.get_color(x_left(check_x) + 1, check_y, n);
                                B.x = x_left(check_x) + 1;
                                B.y = check_y;
                                if (situation_now.ball.vy > 0) {
                                    C.color = situation_now.map.get_color(x_left(check_x), check_y - 1, n);
                                    C.x = x_left(check_x);
                                    C.y = check_y - 1;
                                } else {
                                    C.color = situation_now.map.get_color(x_left(check_x), check_y + 1, n);
                                    C.x = x_left(check_x);
                                    C.y = check_y + 1;
                                }
                            }
                            if (A.color != -1) { // happens a reflection
                                if (B.color != -1) {
                                    if (C.color != -1) { // goes back
                                        if (A.color != 10) {
                                            special_reward_handler(A.color);
                                            situation_now.map.set_hit(A.x, A.y, n);
                                            reward += 1;
                                            touched_bricks.push(A);
                                            touch_cnt++;
                                        }
                                        situation_now.ball.vx = -situation_now.ball.vx;
                                        situation_now.ball.vy = -situation_now.ball.vy;
                                    } else {
                                        if (A.color != 10) {
                                            special_reward_handler(A.color);
                                            situation_now.map.set_hit(A.x, A.y, n);
                                            reward += 1;
                                            touched_bricks.push(A);
                                            touch_cnt++;
                                        }
                                        situation_now.ball.vy = -situation_now.ball.vy;
                                    }
                                } else {
                                    if (C.color != -1) {
                                        if (A.color != 10) {
                                            special_reward_handler(A.color);
                                            situation_now.map.set_hit(A.x, A.y, n);
                                            reward += 1;
                                            touched_bricks.push(A);
                                            touch_cnt++;
                                        }
                                        situation_now.ball.vx = -situation_now.ball.vx;
                                    } else {
                                        if (A.color != 10) {
                                            special_reward_handler(A.color);
                                            situation_now.map.set_hit(A.x, A.y, n);
                                            reward += 1;
                                            touched_bricks.push(A);
                                            touch_cnt++;
                                        }
                                        situation_now.ball.vx = -situation_now.ball.vx;
                                    }
                                }
                            } else {

                            }
                            check_x += get_x_direction(situation_now.ball.vx);
                            next_y = situation_now.ball.y + situation_now.ball.vy;
                            check_y = std::max(situation_now.ball.y, next_y);
                        } else { // reflect horizontal
                            Brick A;
                            if (situation_now.ball.vx > 0) {
                                A.color = situation_now.map.get_color(x_right(check_x), check_y, n);
                                A.x = x_right(check_x);
                                A.y = check_y;
                            } else {
                                A.color = situation_now.map.get_color(x_left(check_x), check_y, n);
                                A.x = x_left(check_x);
                                A.y = check_y;
                            }
                            if (A.color != -1) { // touch the brick
                                if (A.color != 10) {
                                    special_reward_handler(A.color);
                                    situation_now.map.set_hit(A.x, A.y, n);
                                    reward += 1;
                                    touched_bricks.push(A);
                                    touch_cnt++;
                                }
                                situation_now.ball.vx = -situation_now.ball.vx;
                                check_x += get_x_direction(situation_now.ball.vx);
                            } else {
                                check_x += get_x_direction(situation_now.ball.vx);
                            }
                        }
                    }
                    first_step = false;
                    x_move_length--;
                }
                situation_now.ball.x = check_x;
                situation_now.ball.y += situation_now.ball.vy;
            } else {
                check_x = x_right(situation_now.ball.x);
                Brick A;
                A.color = situation_now.map.get_color(check_x, check_y, n);
                A.x = check_x;
                A.y = check_y;
                if (A.color != -1) {
                    if (A.color != 10) {
                        special_reward_handler(A.color);
                        situation_now.map.set_hit(A.x, A.y, n);
                        reward += 1;
                        touched_bricks.push(A);
                        touch_cnt++;
                    }
                    situation_now.ball.vy = -situation_now.ball.vy;
                }
                situation_now.ball.y += situation_now.ball.vy;
            }
        }
        return reward;
    }

    struct Situation {
        Ball ball;
        Map map;

        void init() {
            ball.init();
        }

    } situation_now;
public:
    struct Save {
        Ball ball;
        std::array<std::bitset<2 * k_max_n + 1>, 2 * k_max_n + 1> hit;
        int hits;
    };

    Game(std::istream &mapfile) : map(mapfile) {
        reward = 0;
        op_cnt = 0;
        touch_cnt = 0;

        map >> k >> n >> m >> s;
        for (int y = 2 * n; y > 0; --y) {
            for (int x = 0; x < n; x++)
                map >> situation_now.map.bricks[x][y];
            for (int x = n + 1; x <= 2 * n; ++x)
                map >> situation_now.map.bricks[x][y];
        }

        situation_now.init();
    }

    int bricksTotal () {
        return 4 * n * n;
    }
    int bricksHit () {
        return situation_now.map.hits;
    }
    int bricksRemaining () {
        return bricksTotal() - bricksHit();
    }

    int play(char op) {
        reward = 0;
        touch_cnt = 0;
        while (!touched_bricks.empty())
            touched_bricks.pop();
        return move(op);
    }

    Save *save() {
        return new Save {
            .ball = situation_now.ball,
            .hit = situation_now.map.hit,
            .hits = situation_now.map.hits,
        };
    }

    void load(Save *save) {
        situation_now.ball = save->ball;
        situation_now.map.hit = save->hit;
        situation_now.map.hits = save->hits;
    }

    void erase(Save *save) {
        delete save;
    }
};

#endif //BRICK_OUT_GAME_H
