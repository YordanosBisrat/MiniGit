#include "minigit.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <vector>
namespace fs = std::filesystem;

std::unordered_map<std::string, std::string> stagingArea;

void initMiniGit() {
    if (fs::exists(".minigit")) {
        std::cout << "MiniGit repository already exists.\n";
        return;
    }

    fs::create_directory(".minigit");
    fs::create_directory(".minigit/objects");
    fs::create_directory(".minigit/commits");
    fs::create_directory(".minigit/refs");
    fs::create_directory(".minigit/refs/heads");

    std::ofstream head(".minigit/HEAD");
    head << "refs/heads/main";
    head.close();

    std::ofstream mainBranch(".minigit/refs/heads/main");
    mainBranch << "";
    mainBranch.close();

    std::cout << "Initialized empty MiniGit repository!\n";
}

void addFile(const std::string& filename) {
    if (!fs::exists(filename)) {
        std::cout << "File does not exist: " << filename << "\n";
        return;
    }

    std::ifstream inFile(filename);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();

    std::hash<std::string> hasher;
    size_t hashvalue = hasher(content);
    std::string hashStr = std::to_string(hashvalue);

    std::string blobpath = ".minigit/objects/" + hashStr;
    std::ofstream blob(blobpath);
    blob << content;
    blob.close();

    stagingArea[filename] = hashStr;
    std::cout << "Staged " << filename << " -> " << hashStr << "\n";
}

void commit(const std::string& message) {
    if (stagingArea.empty()) {
        std::cout << "Nothing to commit.\n";
        return;
    }

    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    std::string commitID = std::to_string(now_time);

    std::ifstream headFile(".minigit/HEAD");
    std::string headRef;
    std::getline(headFile, headRef);
    headFile.close();

    std::ifstream branchFile(".minigit/" + headRef);
    std::string parentCommit;
    std::getline(branchFile, parentCommit);
    branchFile.close();

    std::ofstream commitFile(".minigit/commits/" + commitID);
    commitFile << "Message: " << message << "\n";
    commitFile << "Time: " << std::ctime(&now_time);
    commitFile << "Parent: " << parentCommit << "\n";
    commitFile << "Files:\n";
    for (const auto& [file, hash] : stagingArea) {
        commitFile << file << " " << hash << "\n";
    }
    commitFile.close();

    std::ofstream updateBranch(".minigit/" + headRef);
    updateBranch << commitID;
    updateBranch.close();

    std::cout << "Committed with ID: " << commitID << "\n";
    stagingArea.clear();
}

std::string getLatestCommit(const std::string& branchName) {
    std::string path = ".minigit/refs/heads/" + branchName;
    if (!fs::exists(path)) return "";
    std::ifstream in(path);
    std::string commitID;
    std::getline(in, commitID);
    return commitID;
}

void logHistory() {
    std::ifstream headFile(".minigit/HEAD");
    std::string headRef;
    std::getline(headFile, headRef);
    headFile.close();

    std::ifstream branchFile(".minigit/" + headRef);
    std::string currentCommit;
    std::getline(branchFile, currentCommit);
    branchFile.close();

    while (!currentCommit.empty()) {
        std::ifstream commitFile(".minigit/commits/" + currentCommit);
        std::string line, message, time, parent;

        while (std::getline(commitFile, line)) {
            if (line.rfind("Message: ", 0) == 0) message = line.substr(9);
            else if (line.rfind("Time: ", 0) == 0) time = line.substr(6);
            else if (line.rfind("Parent: ", 0) == 0) {
                parent = line.substr(8);
                break;
            }
        }

        std::cout << "Commit: " << currentCommit << "\n";
        std::cout << "Message: " << message << "\n";
        std::cout << "Time: " << time;
        std::cout << "--------------------------\n";

        currentCommit = parent;
    }
}
void createBranch(const std::string& branchName) {
    std::ifstream headFile(".minigit/HEAD");
    std::string headRef;
    std::getline(headFile, headRef);
    headFile.close();

    std::ifstream currentBranch(".minigit/" + headRef);
    std::string currentCommit;
    std::getline(currentBranch, currentCommit);
    currentBranch.close();

    std::string newBranchPath = ".minigit/refs/heads/" + branchName;
    if (fs::exists(newBranchPath)) {
        std::cout << "Branch already exists: " << branchName << "\n";
        return;
    }

    std::ofstream newBranchFile(newBranchPath);
    newBranchFile << currentCommit;
    newBranchFile.close();

    std::cout << "Created new branch: " << branchName << "\n";
}

void loadAndWriteFiles(const std::string& commitID) {
    std::ifstream commitFile(".minigit/commits/" + commitID);
    std::string line;
    bool readingFiles = false;

    while (std::getline(commitFile, line)) {
        if (line == "Files:") {
            readingFiles = true;
            continue;
        }

        if (readingFiles && !line.empty()) {
            std::istringstream iss(line);
            std::string filename, hash;
            iss >> filename >> hash;

            std::ifstream blob(".minigit/objects/" + hash);
            if (!blob) {
                std::cout << "Missing blob: " << hash << "\n";
                continue;
            }

            std::ofstream outFile(filename);
            outFile << blob.rdbuf();
        }
    }
}

std::unordered_map<std::string, std::string> loadCommitFiles(const std::string& commitID) {
    std::unordered_map<std::string, std::string> files;
    std::ifstream commitFile(".minigit/commits/" + commitID);
    std::string line;
    bool reading = false;

    while (std::getline(commitFile, line)) {
        if (line == "Files:") {
            reading = true;
            continue;
        }
        if (reading && !line.empty()) {
            std::istringstream iss(line);
            std::string filename, hash;
            iss >> filename >> hash;
            files[filename] = hash;
        }
    }
    return files;
}

std::string findLCA(const std::string& commitA, const std::string& commitB) {
    std::unordered_set<std::string> ancestors;
    std::string current = commitA;

    while (!current.empty()) {
        ancestors.insert(current);
        std::ifstream file(".minigit/commits/" + current);
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("Parent:", 0) == 0) {
                current = line.substr(8);
                break;
            }
        }
        if (file.eof()) break;
    }

    current = commitB;
    while (!current.empty()) {
        if (ancestors.count(current)) return current;
        std::ifstream file(".minigit/commits/" + current);
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("Parent:", 0) == 0) {
                current = line.substr(8);
                break;
            }
        }
        if (file.eof()) break;
    }

    return "";
}

void checkout(const std::string& name) {
    std::string branchPath = ".minigit/refs/heads/" + name;

    if (fs::exists(branchPath)) {
        std::ofstream headFile(".minigit/HEAD");
        headFile << "refs/heads/" << name;
        headFile.close();

        std::ifstream branchFile(branchPath);
        std::string commitID;
        std::getline(branchFile, commitID);
        branchFile.close();

        if (!commitID.empty()) {
            loadAndWriteFiles(commitID);
        }

        std::cout << "Switched to branch: " << name << "\n";
    } else if (fs::exists(".minigit/commits/" + name)) {
        loadAndWriteFiles(name);
        std::cout << "Checked out commit: " << name << "\n";
    } else {
        std::cout << "Branch or commit not found.\n";
    }
}

void mergeBranch(const std::string& targetBranch) {
    std::string currentBranch = "main"; 
    std::string currentCommit = getLatestCommit(currentBranch);
    std::string targetCommit = getLatestCommit(targetBranch);
    if (targetCommit.empty()) {
        std::cout << "Target branch not found.\n";
        return;
    }

    std::string lca = findLCA(currentCommit, targetCommit);
    if (lca.empty()) {
        std::cout << "No common ancestor.\n";
        return;
    }

    auto base = loadCommitFiles(lca);
    auto current = loadCommitFiles(currentCommit);
    auto target = loadCommitFiles(targetCommit);

    std::unordered_map<std::string, std::string> result = current;

    for (const auto& [file, targetHash] : target) {
        std::string baseHash = base[file];
        std::string currentHash = current[file];

        if (currentHash == targetHash || baseHash == targetHash) continue;

        if (baseHash == currentHash) {
            result[file] = targetHash;
            std::ifstream blob(".minigit/objects/" + targetHash);
            std::ofstream out(file);
            out << blob.rdbuf();
        } else {
            std::cout << "CONFLICT in " << file << "\n";
            std::ifstream cur(".minigit/objects/" + currentHash);
            std::ifstream tgt(".minigit/objects/" + targetHash);
            std::ofstream out(file);
            out << "<<<<<<< CURRENT\n" << cur.rdbuf() << "=======\n" << tgt.rdbuf() << ">>>>>>> TARGET\n";
            result[file] = "CONFLICT";
        }
    }

    for (const auto& [file, hash] : result) {
        stagingArea[file] = hash;
    }

    std::cout << "Merge completed. Resolve conflicts and commit.\n";
}


void diffCommits(const std::string& commit1, const std::string& commit2) {
    auto files1 = loadCommitFiles(commit1);
    auto files2 = loadCommitFiles(commit2);

    std::cout << "Comparing commits: " << commit1 << " vs " << commit2 << "\n";

    for (const auto& [filename, hash1] : files1) {
        if (files2.find(filename) == files2.end()) continue;
        std::string hash2 = files2[filename];

        if (hash1 == hash2) continue;

        std::ifstream file1(".minigit/objects/" + hash1);
        std::ifstream file2(".minigit/objects/" + hash2);

        std::vector<std::string> lines1, lines2;
        std::string line;

        while (std::getline(file1, line)) lines1.push_back(line);
        while (std::getline(file2, line)) lines2.push_back(line);

        std::cout << "Differences in file: " << filename << "\n";

        size_t maxLines = std::max(lines1.size(), lines2.size());
        for (size_t i = 0; i < maxLines; ++i) {
            std::string l1 = i < lines1.size() ? lines1[i] : "";
            std::string l2 = i < lines2.size() ? lines2[i] : "";

            if (l1 != l2) {
                if (!l1.empty()) std::cout << "- Line " << i + 1 << ": " << l1 << "\n";
                if (!l2.empty()) std::cout << "+ Line " << i + 1 << ": " << l2 << "\n";
            }
        }

        std::cout << "-------------------------------\n";
    }
}