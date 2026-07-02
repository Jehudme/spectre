#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace spectre::utilities {

    /**
     * @brief Hashes a virtual path string into a stable uint64_t handle.
     *        This is the canonical handle generation function used across all
     *        modules that need to index resources by path.
     *
     * @param path  The virtual path (e.g. "app://textures/hero.png")
     * @return      A stable uint64_t hash of the path.
     */
    inline uint64_t hash_path(const std::string& path) {
        return std::hash<std::string>{}(path);
    }

} // namespace spectre::utilities
