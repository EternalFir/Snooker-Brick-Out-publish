#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

#include "game.h"

int main() {
    auto mapfile = std::ifstream("./map.in");
    Game *game = new Game(mapfile);
    //                    ^^^^^^^
    // 提交时将上一行改为
    // Game *game = new Game(std::cin);

    std::vector<Game::Save *> saves;

    while (true) {
        int choice;
        std::cin >> choice;
        int id;
        bool result;
        int reward;
        switch (choice) {
            case 1: {
                char op;
                std::cin >> op;
                reward = game->play(op);
                std::cout << reward << std::endl;
                break;
            }
            case 2:
                std::cout << saves.size() << std::endl;
                saves.push_back(game->save());
                break;
            case 3:
                std::cin >> id;
                if (id >= saves.size() || saves[id] == nullptr) {
                    std::cout << "No such save" << std::endl;
                } else {
                    game->load(saves[id]);
                }
                break;
            case 4:
                std::cin >> id;
                game->erase(saves[id]);
                saves[id] = nullptr;
                break;
            default:
                std::cout << "wrong op" << std::endl;
        }
    }
    return 0;
}
