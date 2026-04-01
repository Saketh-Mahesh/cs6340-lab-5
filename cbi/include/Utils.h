/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights Reserved.
 * Template code for CS 6340 Software Analysis
 * Instructors: Mayur Naik and Chris Poch
 * Head TAs: Kelly Parks and Joel Cooper
 *
 * Georgia Tech asserts copyright ownership of this template and all derivative
 * works, including solutions to the projects assigned in this course. Students
 * and other users of this template code are advised not to share it with others
 * or to make it available on publicly viewable websites including repositories
 * such as GitHub and GitLab. This copyright statement should not be removed
 * or edited. Removing it will be considered an academic integrity issue.
 *
 * We do grant permission to share solutions privately with non-students such
 * as potential employers as long as this header remains in full. However, 
 * sharing with other current or future students or using a medium to share
 * where the code is widely available on the internet is prohibited and 
 * subject to being investigated as a GT honor code violation.
 * Please respect the intellectual ownership of the course materials 
 * (including exam keys, project requirements, etc.) and do not distribute them 
 * to anyone not enrolled in the class. Use of any previous semester course 
 * materials, such as tests, quizzes, homework, projects, videos, and any other 
 * coursework, is prohibited in this course. */
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <sys/stat.h>

std::string readOneFile(std::string &Path) {
  std::ifstream SeedFile(Path);
  std::string Line((std::istreambuf_iterator<char>(SeedFile)),
                   std::istreambuf_iterator<char>());
  return Line;
}

int runTarget(std::string &Target, std::string &Path) {
  std::string Cmd = Target + " > /dev/null 2>&1";
  FILE *F = popen(Cmd.c_str(), "w");
  std::string Input = readOneFile(Path);
  fprintf(F, "%s", Input.c_str());
  return pclose(F);
}

enum class State { BranchTrue, BranchFalse, ReturnNeg, ReturnZero, ReturnPos };
std::set<std::string> SuccessLogs;
std::set<std::string> FailureLogs;
std::map<std::tuple<int, int, State>, double> F;
std::map<std::tuple<int, int, State>, double> S;
std::map<std::tuple<int, int, State>, double> Failure;
std::map<std::tuple<int, int, State>, double> Context;
std::map<std::tuple<int, int, State>, double> Increase;

// Might need these
std::map<std::tuple<int, int, State>, double> FObs;
std::map<std::tuple<int, int, State>, double> SObs;


std::string toString(State S) {
  switch (S) {
  case State::BranchTrue:
    return "BranchTrue";
  case State::BranchFalse:
    return "BranchFalse";
  case State::ReturnNeg:
    return "ReturnNeg";
  case State::ReturnZero:
    return "ReturnZero";
  case State::ReturnPos:
    return "ReturnPos";
  }
}

void printMap(std::map<std::tuple<int, int, State>, double> &Map) {
  for (auto &Entry : Map) {
    std::cout << "Line " << std::get<0>(Entry.first) << ",  Col "
              << std::get<1>(Entry.first) << ", "
              << toString(std::get<2>(Entry.first)) << ": " << Entry.second
              << std::endl;
  }
}

void printReport() {
  std::cout << "== S(P) ==" << std::endl;
  printMap(S);
  std::cout << "== F(P) ==" << std::endl;
  printMap(F);

  std::cout << "== Failure(P) ==" << std::endl;
  printMap(Failure);
  std::cout << "== Context(P) ==" << std::endl;
  printMap(Context);
  std::cout << "== Increase(P) ==" << std::endl;
  printMap(Increase);
}

void generateLogFiles(std::string &Target, std::string &LogDir) {
  struct dirent *Ent;
  DIR *Directory;
  std::string SuccessDir = LogDir + "/success/";
  std::string FailureDir = LogDir + "/failure/";

  std::string LogFile = Target + ".cbi";

  unlink(LogFile.c_str());

  std::cout << "Generating log files..." << std::endl;
  Directory = opendir(SuccessDir.c_str());
  if (Directory == NULL) {
    fprintf(stderr, "%s directory not found\n", SuccessDir.c_str());
    exit(1);
  }

  std::regex Reg(R"(^input[0-9]+(?:(?!\.).)+)");

  while ((Ent = readdir(Directory)) != NULL) {
    if (!(Ent->d_type == DT_REG))
      continue;
    std::string Input(Ent->d_name);
    if (std::regex_match(Input, Reg)) {
      std::string Path = SuccessDir + "/" + Input;
      runTarget(Target, Path);
      std::string Dst = Path + ".cbi";
      rename(LogFile.c_str(), Dst.c_str());
      SuccessLogs.insert(Dst);
    }
  }
  closedir(Directory);

  Directory = opendir(FailureDir.c_str());
  if (Directory == NULL) {
    fprintf(stderr, "%s directory not found\n", FailureDir.c_str());
    exit(1);
  }

  while ((Ent = readdir(Directory)) != NULL) {
    if (!(Ent->d_type == DT_REG))
      continue;
    std::string Input(Ent->d_name);
    if (std::regex_match(Input, Reg)) {
      std::string Path = FailureDir + "/" + Input;
      runTarget(Target, Path);
      std::string Dst = Path + ".cbi";
      rename(LogFile.c_str(), Dst.c_str());
      FailureLogs.insert(Dst);
    }
  }
  closedir(Directory);
}
