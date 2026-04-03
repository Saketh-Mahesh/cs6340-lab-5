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
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h>

#include "Utils.h"

static const char *BRANCH_TYPE = "branch";
static const char *RETURN_TYPE = "return";

State getStateForValue(const std::string &Type, int Value) {
    if (Type == BRANCH_TYPE) {
        return (Value == 1) ? State::BranchTrue : State::BranchFalse;
    } else if (Type == RETURN_TYPE) {
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

void updatePredicateMaps(
    std::set<std::tuple<int, int, State>> &truePredicates,
    std::set<std::tuple<int, int, State>> &observedPredicates, bool isSuccess) {
    auto &successCountMap = isSuccess ? S : F;
    auto &successObsMap = isSuccess ? SObs : FObs;
    for (const auto &element : truePredicates) {
        successCountMap[element] += 1.0;
    }
    for (const auto &element : observedPredicates) {
        successObsMap[element] += 1.0;
    }
}

void addBranchPredicates(
    std::set<std::tuple<int, int, State>> &truePredicates,
    std::set<std::tuple<int, int, State>> &observedPredicates, int line,
    int col, int val) {
    State S = getStateForValue(BRANCH_TYPE, val);
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
    State S = getStateForValue(RETURN_TYPE, val);
    truePredicates.insert(std::make_tuple(line, col, S));
}

std::set<std::tuple<int, int, State>> collectPredicateKeys() {
    std::set<std::tuple<int, int, State>> allPredicateKeys;
    for (const auto &m : {S, SObs, F, FObs}) {
        for (const auto &[key, value] : m) {
            allPredicateKeys.insert(key);
        }
    }

    return allPredicateKeys;
}

void calculateMetrics(std::set<std::tuple<int, int, State>> &allPredicateKeys) {
    for (const auto &key : allPredicateKeys) {
        double failureDenom = F[key] + S[key];
        if (failureDenom == 0) {
            Failure[key] = 0.0;
        } else {
            Failure[key] = F[key] / failureDenom;
        }
        double contextDenom = FObs[key] + SObs[key];
        if (contextDenom == 0) {
            Context[key] = 0.0;
        } else {
            Context[key] = FObs[key] / contextDenom;
        }
        Increase[key] = Failure[key] - Context[key];
    }
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

        if (Type == BRANCH_TYPE) {
            addBranchPredicates(truePredicates, observedPredicates, line, col,
                                val);
        } else if (Type == RETURN_TYPE) {
            addReturnPredicates(truePredicates, observedPredicates, line, col,
                                val);
        }
    }

    updatePredicateMaps(truePredicates, observedPredicates, isSuccess);
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

    std::set<std::tuple<int, int, State>> allPredicateKeys =
        collectPredicateKeys();
    calculateMetrics(allPredicateKeys);
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
