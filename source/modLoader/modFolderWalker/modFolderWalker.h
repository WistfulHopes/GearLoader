#pragma once

#include <filesystem>
#include "logger/logger.h"


typedef void (*WalkModFolderCallback)(std::filesystem::directory_entry modFolder, std::filesystem::directory_entry file);

void WalkModFolder(std::filesystem::path modFolder, WalkModFolderCallback callback, Logger& logger) {
    if (!std::filesystem::exists(modFolder) || !std::filesystem::is_directory(modFolder)) {
        logger.log(WARN, "No mod folder was found, creating one now");
        std::error_code errCode;
        bool success = std::filesystem::create_directory(modFolder, errCode);
        if (!success || errCode.value() > 0) {
            logger.log(ERR, "Failed to create mod folder. Error code 0x%x (%d): %s",
                errCode.value(),
                errCode.value(),
                errCode.message());
        }
        return;
    }
    logger.log(VERBOSE, "Mod folder found: %s", modFolder.string().c_str());

    logger.log(VERBOSE, "Walking mod folder:");
    for (const auto& entry : std::filesystem::directory_iterator(modFolder)) {
        logger.log(VERBOSE, "\t%s", entry.path().string().c_str());
        if (std::filesystem::is_directory(entry)) {
            for (const auto& subEntry : std::filesystem::directory_iterator(entry)) {
                logger.log(VERBOSE, "\t\t%s", subEntry.path().string().c_str());
                callback(entry, subEntry);
            }
        }
    }
}
