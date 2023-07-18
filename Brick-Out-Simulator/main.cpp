#include <iostream>
#include "game.h"

int main() {
    Game *game = new Game();
    while (1) {
        int choice;
        std::cin >> choice;
        int id;
        bool result;
        int reward;
        switch (choice) {
            case 1:
                char op;
                std::cin >> op;
                reward = game->play(op);
                std::cout << reward << std::endl;
                break;
            case 2:
                id = game->save();
                std::cout << id << std::endl;
                break;
            case 3:
                std::cin >> id;
                result = game->load(id);
                std::cout << result << std::endl;
                break;
            case 4:
                std::cin >> id;
                result = game->erase(id);
                std::cout << result << std::endl;
                break;
            default:
                std::cout << "wrong op" << std::endl;
        }

    }
    delete (game);


    return 0;
}
