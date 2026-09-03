#pragma once

#include <cstdint>
#include <memory>
#include <vector>

struct FileDeleter {
    void operator()(std::FILE *fp) const {
        if (fp) {
            std::fclose(fp);
        }
    }
};

using UniqueFile = std::unique_ptr<std::FILE, FileDeleter>;

UniqueFile open_file(const char *filename, const char *mode) {
    return UniqueFile(std::fopen(filename, mode));
}

bool read_binary_file(const char *filename, std::vector<char> &buffer) {
    const UniqueFile fp = open_file(filename, "rb");
    if (!fp) return false;

    std::fseek(fp.get(), 0, SEEK_END);
    const uint32_t size = std::ftell(fp.get());
    std::fseek(fp.get(), 0, SEEK_SET);

    if (size > 0) {
        buffer.resize(size);
        if (const std::size_t read_bytes = std::fread(buffer.data(), 1, size, fp.get());
            read_bytes != static_cast<std::size_t>(size)) {
            return false;
        }
    }

    return true;
}
