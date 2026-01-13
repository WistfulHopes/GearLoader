#pragma once

#include "gearLoaderApi/gearLoader_p.h"
#include <filesystem>


ModManifest ParseConfig(std::filesystem::path path);
