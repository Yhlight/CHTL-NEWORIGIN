#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declare miniz structures
struct mz_zip_archive;

namespace CHTL::Util {

struct ZipEntry {
    std::string name;
    std::string content;
    size_t uncompressedSize;
    size_t compressedSize;
    bool isDirectory;
    
    ZipEntry() : uncompressedSize(0), compressedSize(0), isDirectory(false) {}
};

class ZipUtil {
public:
    // Archive operations
    static bool createArchive(const std::string& archivePath, const std::vector<ZipEntry>& entries);
    static bool extractArchive(const std::string& archivePath, const std::string& extractPath);
    static std::vector<ZipEntry> listArchive(const std::string& archivePath);
    static ZipEntry readEntry(const std::string& archivePath, const std::string& entryName);
    
    // File operations
    static bool addFileToArchive(const std::string& archivePath, const std::string& filePath, const std::string& archivePath = "");
    static bool addDirectoryToArchive(const std::string& archivePath, const std::string& dirPath, const std::string& archivePath = "");
    static bool extractFileFromArchive(const std::string& archivePath, const std::string& entryName, const std::string& outputPath);
    
    // Memory operations
    static std::vector<uint8_t> createArchiveInMemory(const std::vector<ZipEntry>& entries);
    static std::vector<ZipEntry> extractArchiveFromMemory(const std::vector<uint8_t>& data);
    
    // CMOD/CJMOD specific operations
    static bool createCMOD(const std::string& modulePath, const std::string& outputPath);
    static bool extractCMOD(const std::string& cmodPath, const std::string& extractPath);
    static bool createCJMOD(const std::string& modulePath, const std::string& outputPath);
    static bool extractCJMOD(const std::string& cjmodPath, const std::string& extractPath);
    
    // Validation
    static bool isValidArchive(const std::string& archivePath);
    static bool isCMOD(const std::string& filePath);
    static bool isCJMOD(const std::string& filePath);
    
    // Utility functions
    static std::string getArchiveInfo(const std::string& archivePath);
    static size_t getArchiveSize(const std::string& archivePath);
    static std::vector<std::string> getArchiveFileList(const std::string& archivePath);
    
private:
    static bool validateCMODStructure(const std::string& modulePath);
    static bool validateCJMODStructure(const std::string& modulePath);
    static std::vector<ZipEntry> collectCMODFiles(const std::string& modulePath);
    static std::vector<ZipEntry> collectCJMODFiles(const std::string& modulePath);
    static std::string normalizePath(const std::string& path);
};

} // namespace CHTL::Util