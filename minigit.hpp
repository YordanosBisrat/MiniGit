#ifndef MINIGIT_HPP
#define MINIGIT_HPP

#include <string>
#include <unordered_map>

void initMiniGit();
void addFile(const std::string& filename);
void commit(const std::string& message);
void logHistory();
void createBranch(const std::string& branchName);
void checkout(const std::string& name);
void mergeBranch(const std::string& targetBranch);
void loadAndWriteFiles(const std::string& commitID); 

std::string getLatestCommit(const std::string& branchName);
std::unordered_map<std::string, std::string> loadCommitFiles(const std::string& commitID);
std::string findLCA(const std::string& commitA, const std::string& commitB);

void diffCommits(const std::string& commit1, const std::string& commit2);
#endif