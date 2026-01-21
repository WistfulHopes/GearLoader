#pragma once

#include "gearloaderApi/gearLoader_p.h"


Operator ParseOperand(std::string s);
std::string ToString(Operator op);
SemanticVersion ParseSemanticVersion(std::string s);
std::string ToString(SemanticVersion ver);
bool CompareVersions(SemanticVersion a, Operator op, SemanticVersion b);
bool ParseVersionQualifier(std::string s, Operator& outOp, SemanticVersion& outVer);
bool Evaluate(const char* versionConstraint, SemanticVersion version);