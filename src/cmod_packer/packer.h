#pragma once

#include <filesystem>

// The core logic for the CMOD packer, refactored into a callable function.
// Returns 0 on success, 1 on failure.
int pack_cmod(const std::filesystem::path& source_dir, const std::filesystem::path& output_file);