#pragma once

#include <filesystem>
#include "logger/logger.h"


typedef void (*WalkModFolderCallback)(std::filesystem::directory_entry modFolder, std::filesystem::directory_entry file);

void WalkModFolder(std::filesystem::path modFolder, WalkModFolderCallback callback, Logger& logger);
