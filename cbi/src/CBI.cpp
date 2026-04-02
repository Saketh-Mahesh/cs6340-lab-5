/*
 * Copyright Â© 2021 Georgia Institute of Technology (Georgia Tech). All Rights
 * Reserved. Template code for CS 6340 Software Analysis Instructors: Mayur Naik
 * and Chris Poch Head TAs: Kelly Parks and Joel Cooper
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
#include <cstdlib>
#include <sstream>
#include <string>
#include <unistd.h>

#include "Utils.h"

State getStateForValue(const std::string &Type, int Value) {
    if (Type == "branch") {
        return (Value == 1) ? State::BranchTrue : State::BranchFalse;
    } else if (Type == "return") {
        if (Value < 0) {
            return State::ReturnNeg;
        } else if (Value == 0) {
            return State::ReturnZero;
        } else {
            return State::ReturnPos;
        }
    }
    return State::BranchFalse;
}

void addBranchPredicates(
    std::set<std::tuple<int, int, State>> &truePredicates,
    std::set<std::tuple<int, int, State>> &observedPredicates, int line,
    int col, int val) {
    State S = getStateForValue("branch", val);
    truePredicates.insert(std::make_tuple(line, col, S));
    observedPredicates.insert(std::make_tuple(line, col, State::BranchFalse));
    observedPredicates.insert(std::make_tuple(line, col, State::BranchTrue));
}

void addReturnPredicates(
    std::set<std::tuple<int, int, State>> &truePredicates,
    std::set<std::tuple<int, int, State>> &observedPredicates, int line,
    int col, int val) {
    observedPredicates.insert(std::make_tuple(line, col, State::ReturnNeg));
    observedPredicates.insert(std::make_tuple(line, col, State::ReturnZero));
    observedPredicates.insert(std::make_tuple(line, col, State::ReturnPos));

    State S = getStateForValue("return", val);
    truePredicates.insert(std::make_tuple(line, col, S));
}

void computeScores() {
    
}

void constructPredicateSets(std::ifstream &File, bool isSuccess) {
    std::string Line;
    std::set<std::tuple<int, int, State>> truePredicates;
    std::set<std::tuple<int, int, State>> observedPredicates;

    while (std::getline(File, Line)) {
        std::istringstream SS(Line);
        std::string Type, LineNo, ColNo, Value;

        std::getline(SS, Type, ',');
        std::getline(SS, LineNo, ',');
        std::getline(SS, ColNo, ',');
        std::getline(SS, Value, ',');

        int line = std::stoi(LineNo);
        int col = std::stoi(ColNo);
        int val = std::stoi(Value);

        if (Type == "branch") {
            addBranchPredicates(truePredicates, observedPredicates, line, col,
                                val);
        } else if (Type == "return") {
            addReturnPredicates(truePredicates, observedPredicates, line, col,
                                val);
        }
    }

    if (isSuccess) {
        for (const auto &element : truePredicates) {
            S[element] += 1.0;
        }
        for (const auto &element : observedPredicates) {
            SObs[element] += 1.0;
        }
    } else {
        for (const auto &element : truePredicates) {
            F[element] += 1.0;
        }
        for (const auto &element : observedPredicates) {
            FObs[element] += 1.0;
        }
    }
}

void generateReport() {
    for (const auto &element : FailureLogs) {
        std::ifstream File(element);
        constructPredicateSets(File, false);
    }
    for (const auto &element : SuccessLogs) {
        std::ifstream File(element);
        constructPredicateSets(File, true);
    }
}

// ./CBI [exe file] [fuzzer output dir]
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Invalid usage\n");
        return 1;
    }

    struct stat Buffer;
    if (stat(argv[1], &Buffer)) {
        fprintf(stderr, "%s not found\n", argv[1]);
        return 1;
    }

    if (stat(argv[2], &Buffer)) {
        fprintf(stderr, "%s not found\n", argv[2]);
        return 1;
    }

    std::string Target(argv[1]);
    std::string OutDir(argv[2]);

    generateLogFiles(Target, OutDir);
    generateReport();
    printReport();
    return 0;
}
