#include "versionParsing.h"
#include <functional>
#include <regex>

#include <iostream>

Operator ParseOperand(std::string str) {
    static const std::unordered_map<std::string, Operator> opMap {
        {">=", Operator::EQ_OR_GREATER_THAN},
        {">", Operator::GREATER_THAN},
        {"=", Operator::EQUAL},
        {"<", Operator::LESS_THAN},
        {"<=", Operator::EQ_OR_LESS_THAN},
    };

    if (!opMap.contains(str)) {
        return Operator::EQ_OR_GREATER_THAN;
    } else {
        return opMap.at(str);
    }
}
std::string ToString(Operator op) {
    static const std::unordered_map<Operator, std::string> opMap {
        {Operator::EQ_OR_GREATER_THAN, ">="},
        {Operator::GREATER_THAN, ">"},
        {Operator::EQUAL, "="},
        {Operator::LESS_THAN, "<"},
        {Operator::EQ_OR_LESS_THAN, "<="},
    };

    return opMap.at(op);
}
SemanticVersion ParseSemanticVersion(std::string s) {
    SemanticVersion output;

    std::stringstream ss(s);
    std::string token;
    char delim = '.';
    unsigned int tokens[3] = {0, 0, 0};

    for (int i = 0; i < 3; i++) {
        std::getline(ss, token, delim);
        try {
            tokens[i] = std::stoi(token);
        } catch (const std::invalid_argument& e) {
            break;
        }
        if (ss.eof()) {
            break;
        }
    }

    return SemanticVersion {tokens[0], tokens[1], tokens[2]};
}
std::string ToString(SemanticVersion ver) {
    std::stringstream ss;
    ss << ver.major << "." << ver.minor << "." << ver.patchNum;
    return ss.str();
}
using VersionComparator = std::function<bool(SemanticVersion, SemanticVersion)>;
VersionComparator GetComparatorLambda(Operator op) {
    static const std::unordered_map<Operator, VersionComparator> _lambdaMap {
        {Operator::EQ_OR_GREATER_THAN, [](SemanticVersion a, SemanticVersion b) { return a >= b; }},
        {Operator::GREATER_THAN,  [](SemanticVersion a, SemanticVersion b) { return a > b; }},
        {Operator::EQUAL,  [](SemanticVersion a, SemanticVersion b) { return a == b; }},
        {Operator::LESS_THAN,  [](SemanticVersion a, SemanticVersion b) { return a < b; }},
        {Operator::EQ_OR_LESS_THAN, [](SemanticVersion a, SemanticVersion b) { return a <= b; }},
    };

    if (!_lambdaMap.contains(op)) {
        return _lambdaMap.at(Operator::EQ_OR_GREATER_THAN);
    } else {
        return _lambdaMap.at(op);
    }
}
bool CompareVersions(SemanticVersion a, Operator op, SemanticVersion b) {
    return GetComparatorLambda(op)(a, b);
}


// selects '<=', '>=', '=', '<', and '>'
const std::regex operationRegex("^[<>]=|^[=<>]");
const std::regex semanticVersionRegex(R"(\d+(.\d+){0,2})");

bool ParseVersionQualifier(std::string s, Operator& ouOp, SemanticVersion& outVer) {
    SemanticVersion specifiedVersion;
    std::string versionOpStr = "";

    auto regexIter =
        std::sregex_iterator(s.begin(), s.end(), operationRegex);
    std::smatch match = *regexIter;

    if (!match.empty()) {
        int substringOffset = match.position() + match.length();
        outVer = ParseSemanticVersion(s.substr(substringOffset));

        ouOp = ParseOperand(match.str());
    } else {
        outVer = ParseSemanticVersion(s);
    }

    return true;
}

bool Evaluate(const char* versionConstraint, SemanticVersion version) {
    const std::string constraintStr(versionConstraint);

    Operator verOp;
    SemanticVersion specifiedVersion;
    if (!ParseVersionQualifier(constraintStr, verOp, specifiedVersion)) return false;

    return CompareVersions(version, verOp, specifiedVersion);
}
