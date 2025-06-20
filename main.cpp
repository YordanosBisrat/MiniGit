#include <iostream>
#include <string>
#include "minigit.hpp"

int main(int argc, char* argv[]) {
    std::string command;

    std::cout << "welcom to MiniGit!\n";

    while (true) 
    {
        std::cout << "\nEnter command: ";
        std::cin >> command;
        if (command == "init") {
            initMiniGit();
        }
        else if (command == "add") {
            std::string filename;
            std::cin >> filename;
            addFile(filename);
        }
        else if (command == "commit") {
            std::string dash, msg;
            std::cin >> dash;
            std::getline(std::cin, msg);
            if (dash == "-m" && !msg.empty()) {
                if (msg[0] == ' ') msg = msg.substr(1);
                commit(msg);
            }
            else {
                std::cout << "Usage: commit -m\"your message\"\n";
            }
        }
        else if (command == "log") {
            logHistory();
        }
        else if (command == "branch") {
        std::string branchName;
        std::cin >> branchName;
        createBranch(branchName);
        }
        else if (command == "checkout") {
        std::string name;
        std::cin >> name;
        checkout(name);
        }
        else if (command == "merge") {
        std::string targetBranch;
        std::cin >> targetBranch;
        mergeBranch(targetBranch);
        }
        else if (command == "diff") {
        std::string id1, id2;
        std::cin >> id1 >> id2;
        diffCommits(id1, id2);
        }
        else if (command == "exit") {
            std::cout << "Exiting MiniGit...\n";
            break;
        }
        else {
            std::cout << "Unknown command. Try 'init' or 'exit'.\n"; 
        }
    }
    return 0;
}


//This is just to fix my.. (WinMain)
#ifdef _WIN32
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdshow) {
    return main(__argc, __argv);
}
#endif