#include "modFolderWalker.h"

void WalkModFolder(std::filesystem::path modFolder, WalkModFolderCallback callback, Logger& logger) {
	if (!std::filesystem::exists(modFolder) || !std::filesystem::is_directory(modFolder)) {
		logger.log(WARN, "No mod folder was found");
		return;
	}
	logger.log(VERBOSE, "Mod folder found: %s", modFolder.string().c_str());

	logger.log(VERBOSE, "Walking mod folder:");
	for (const auto& entry : std::filesystem::directory_iterator(modFolder)) {
		logger.log(VERBOSE, "\t%s", entry.path().string().c_str());
		if (std::filesystem::is_directory(entry)) {
			// TODO: might need to track if a dll / config is found here
			for (const auto& subEntry : std::filesystem::directory_iterator(entry)) {
				logger.log(VERBOSE, "\t\t%s", subEntry.path().string().c_str());
				callback(entry, subEntry);
			}
		}
	}
}