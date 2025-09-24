#include "ZipUtil.h"
#include "miniz.h"
#include "FileSystem.h"
#include <fstream>
#include <sstream>

namespace CHTL::Util {

bool ZipUtil::createArchive(const std::string& archivePath, const std::vector<ZipEntry>& entries) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_writer_init_file(&zipArchive, archivePath.c_str(), 0)) {
        return false;
    }
    
    bool success = true;
    for (const auto& entry : entries) {
        if (!mz_zip_writer_add_mem(&zipArchive, entry.name.c_str(), 
                                   entry.content.c_str(), entry.content.length(), MZ_DEFAULT_COMPRESSION)) {
            success = false;
            break;
        }
    }
    
    mz_zip_writer_finalize_archive(&zipArchive);
    mz_zip_writer_end(&zipArchive);
    
    return success;
}

bool ZipUtil::extractArchive(const std::string& archivePath, const std::string& extractPath) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_reader_init_file(&zipArchive, archivePath.c_str(), 0)) {
        return false;
    }
    
    bool success = true;
    int numFiles = mz_zip_reader_get_num_files(&zipArchive);
    
    for (int i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat fileStat;
        if (!mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            success = false;
            break;
        }
        
        std::string fileName = fileStat.m_filename;
        std::string fullPath = FileSystem::joinPath(extractPath, fileName);
        
        if (mz_zip_reader_is_file_a_directory(&zipArchive, i)) {
            FileSystem::createDirectory(fullPath);
        } else {
            size_t uncompSize = fileStat.m_uncomp_size;
            std::vector<char> buffer(uncompSize);
            
            if (!mz_zip_reader_extract_to_mem(&zipArchive, i, buffer.data(), uncompSize, 0)) {
                success = false;
                break;
            }
            
            FileSystem::writeFile(fullPath, std::string(buffer.data(), uncompSize));
        }
    }
    
    mz_zip_reader_end(&zipArchive);
    return success;
}

std::vector<ZipEntry> ZipUtil::listArchive(const std::string& archivePath) {
    std::vector<ZipEntry> entries;
    
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_reader_init_file(&zipArchive, archivePath.c_str(), 0)) {
        return entries;
    }
    
    int numFiles = mz_zip_reader_get_num_files(&zipArchive);
    
    for (int i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat fileStat;
        if (mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            ZipEntry entry;
            entry.name = fileStat.m_filename;
            entry.uncompressedSize = fileStat.m_uncomp_size;
            entry.compressedSize = fileStat.m_comp_size;
            entry.isDirectory = mz_zip_reader_is_file_a_directory(&zipArchive, i);
            
            if (!entry.isDirectory) {
                size_t uncompSize = fileStat.m_uncomp_size;
                std::vector<char> buffer(uncompSize);
                
                if (mz_zip_reader_extract_to_mem(&zipArchive, i, buffer.data(), uncompSize, 0)) {
                    entry.content = std::string(buffer.data(), uncompSize);
                }
            }
            
            entries.push_back(entry);
        }
    }
    
    mz_zip_reader_end(&zipArchive);
    return entries;
}

ZipEntry ZipUtil::readEntry(const std::string& archivePath, const std::string& entryName) {
    ZipEntry entry;
    
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_reader_init_file(&zipArchive, archivePath.c_str(), 0)) {
        return entry;
    }
    
    int fileIndex = mz_zip_reader_locate_file(&zipArchive, entryName.c_str(), nullptr, 0);
    if (fileIndex < 0) {
        mz_zip_reader_end(&zipArchive);
        return entry;
    }
    
    mz_zip_archive_file_stat fileStat;
    if (mz_zip_reader_file_stat(&zipArchive, fileIndex, &fileStat)) {
        entry.name = fileStat.m_filename;
        entry.uncompressedSize = fileStat.m_uncomp_size;
        entry.compressedSize = fileStat.m_comp_size;
        entry.isDirectory = mz_zip_reader_is_file_a_directory(&zipArchive, fileIndex);
        
        if (!entry.isDirectory) {
            size_t uncompSize = fileStat.m_uncomp_size;
            std::vector<char> buffer(uncompSize);
            
            if (mz_zip_reader_extract_to_mem(&zipArchive, fileIndex, buffer.data(), uncompSize, 0)) {
                entry.content = std::string(buffer.data(), uncompSize);
            }
        }
    }
    
    mz_zip_reader_end(&zipArchive);
    return entry;
}

bool ZipUtil::addFileToArchive(const std::string& archivePath, const std::string& filePath, const std::string& archiveEntryPath) {
    if (!FileSystem::exists(filePath) || !FileSystem::isFile(filePath)) {
        return false;
    }
    
    std::string content = FileSystem::readFile(filePath);
    if (content.empty() && FileSystem::readFile(filePath).empty()) {
        return false; // Failed to read file
    }
    
    ZipEntry entry;
    entry.name = archiveEntryPath.empty() ? FileSystem::getFileName(filePath) : archiveEntryPath;
    entry.content = content;
    entry.uncompressedSize = content.length();
    entry.isDirectory = false;
    
    std::vector<ZipEntry> entries = {entry};
    return createArchive(archivePath, entries);
}

bool ZipUtil::addDirectoryToArchive(const std::string& archivePath, const std::string& dirPath, const std::string& archiveEntryPath) {
    if (!FileSystem::exists(dirPath) || !FileSystem::isDirectory(dirPath)) {
        return false;
    }
    
    std::vector<ZipEntry> entries;
    auto files = FileSystem::listAll(dirPath, true);
    
    for (const auto& file : files) {
        if (FileSystem::isFile(file)) {
            std::string relativePath = file.substr(dirPath.length());
            if (relativePath.front() == '/' || relativePath.front() == '\\') {
                relativePath = relativePath.substr(1);
            }
            
            std::string finalPath = archiveEntryPath.empty() ? relativePath : 
                                   FileSystem::joinPath(archiveEntryPath, relativePath);
            
            ZipEntry entry;
            entry.name = normalizePath(finalPath);
            entry.content = FileSystem::readFile(file);
            entry.uncompressedSize = entry.content.length();
            entry.isDirectory = false;
            
            entries.push_back(entry);
        }
    }
    
    return createArchive(archivePath, entries);
}

bool ZipUtil::extractFileFromArchive(const std::string& archivePath, const std::string& entryName, const std::string& outputPath) {
    ZipEntry entry = readEntry(archivePath, entryName);
    if (entry.name.empty()) {
        return false;
    }
    
    return FileSystem::writeFile(outputPath, entry.content);
}

std::vector<uint8_t> ZipUtil::createArchiveInMemory(const std::vector<ZipEntry>& entries) {
    std::vector<uint8_t> data;
    
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_writer_init_heap(&zipArchive, 0, 1024 * 1024)) {
        return data;
    }
    
    for (const auto& entry : entries) {
        if (!mz_zip_writer_add_mem(&zipArchive, entry.name.c_str(), 
                                   entry.content.c_str(), entry.content.length(), MZ_DEFAULT_COMPRESSION)) {
            mz_zip_writer_end(&zipArchive);
            return data;
        }
    }
    
    void* pBuf;
    size_t pSize;
    if (mz_zip_writer_finalize_heap_archive(&zipArchive, &pBuf, &pSize)) {
        data.assign(static_cast<uint8_t*>(pBuf), static_cast<uint8_t*>(pBuf) + pSize);
    }
    
    mz_zip_writer_end(&zipArchive);
    return data;
}

std::vector<ZipEntry> ZipUtil::extractArchiveFromMemory(const std::vector<uint8_t>& data) {
    std::vector<ZipEntry> entries;
    
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_reader_init_mem(&zipArchive, data.data(), data.size(), 0)) {
        return entries;
    }
    
    int numFiles = mz_zip_reader_get_num_files(&zipArchive);
    
    for (int i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat fileStat;
        if (mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            ZipEntry entry;
            entry.name = fileStat.m_filename;
            entry.uncompressedSize = fileStat.m_uncomp_size;
            entry.compressedSize = fileStat.m_comp_size;
            entry.isDirectory = mz_zip_reader_is_file_a_directory(&zipArchive, i);
            
            if (!entry.isDirectory) {
                size_t uncompSize = fileStat.m_uncomp_size;
                std::vector<char> buffer(uncompSize);
                
                if (mz_zip_reader_extract_to_mem(&zipArchive, i, buffer.data(), uncompSize, 0)) {
                    entry.content = std::string(buffer.data(), uncompSize);
                }
            }
            
            entries.push_back(entry);
        }
    }
    
    mz_zip_reader_end(&zipArchive);
    return entries;
}

bool ZipUtil::createCMOD(const std::string& modulePath, const std::string& outputPath) {
    if (!validateCMODStructure(modulePath)) {
        return false;
    }
    
    std::vector<ZipEntry> entries = collectCMODFiles(modulePath);
    return createArchive(outputPath, entries);
}

bool ZipUtil::extractCMOD(const std::string& cmodPath, const std::string& extractPath) {
    return extractArchive(cmodPath, extractPath);
}

bool ZipUtil::createCJMOD(const std::string& modulePath, const std::string& outputPath) {
    if (!validateCJMODStructure(modulePath)) {
        return false;
    }
    
    std::vector<ZipEntry> entries = collectCJMODFiles(modulePath);
    return createArchive(outputPath, entries);
}

bool ZipUtil::extractCJMOD(const std::string& cjmodPath, const std::string& extractPath) {
    return extractArchive(cjmodPath, extractPath);
}

bool ZipUtil::isValidArchive(const std::string& archivePath) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    bool isValid = mz_zip_reader_init_file(&zipArchive, archivePath.c_str(), 0);
    if (isValid) {
        mz_zip_reader_end(&zipArchive);
    }
    
    return isValid;
}

bool ZipUtil::isCMOD(const std::string& filePath) {
    if (!endsWith(filePath, ".cmod")) {
        return false;
    }
    
    return isValidArchive(filePath);
}

bool ZipUtil::isCJMOD(const std::string& filePath) {
    if (!endsWith(filePath, ".cjmod")) {
        return false;
    }
    
    return isValidArchive(filePath);
}

std::string ZipUtil::getArchiveInfo(const std::string& archivePath) {
    std::ostringstream info;
    
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));
    
    if (!mz_zip_reader_init_file(&zipArchive, archivePath.c_str(), 0)) {
        return "Invalid archive";
    }
    
    int numFiles = mz_zip_reader_get_num_files(&zipArchive);
    info << "Archive: " << archivePath << "\n";
    info << "Files: " << numFiles << "\n";
    
    size_t totalUncompressed = 0;
    size_t totalCompressed = 0;
    
    for (int i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat fileStat;
        if (mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            totalUncompressed += fileStat.m_uncomp_size;
            totalCompressed += fileStat.m_comp_size;
        }
    }
    
    info << "Total uncompressed size: " << totalUncompressed << " bytes\n";
    info << "Total compressed size: " << totalCompressed << " bytes\n";
    info << "Compression ratio: " << (totalCompressed * 100.0 / totalUncompressed) << "%\n";
    
    mz_zip_reader_end(&zipArchive);
    return info.str();
}

size_t ZipUtil::getArchiveSize(const std::string& archivePath) {
    return FileSystem::exists(archivePath) ? FileSystem::readFile(archivePath).length() : 0;
}

std::vector<std::string> ZipUtil::getArchiveFileList(const std::string& archivePath) {
    std::vector<std::string> fileList;
    auto entries = listArchive(archivePath);
    
    for (const auto& entry : entries) {
        fileList.push_back(entry.name);
    }
    
    return fileList;
}

bool ZipUtil::validateCMODStructure(const std::string& modulePath) {
    // CMOD structure validation: src/ and info/ directories required
    std::string srcPath = FileSystem::joinPath(modulePath, "src");
    std::string infoPath = FileSystem::joinPath(modulePath, "info");
    
    return FileSystem::exists(srcPath) && FileSystem::isDirectory(srcPath) &&
           FileSystem::exists(infoPath) && FileSystem::isDirectory(infoPath);
}

bool ZipUtil::validateCJMODStructure(const std::string& modulePath) {
    // CJMOD structure validation: src/ and info/ directories required
    std::string srcPath = FileSystem::joinPath(modulePath, "src");
    std::string infoPath = FileSystem::joinPath(modulePath, "info");
    
    return FileSystem::exists(srcPath) && FileSystem::isDirectory(srcPath) &&
           FileSystem::exists(infoPath) && FileSystem::isDirectory(infoPath);
}

std::vector<ZipEntry> ZipUtil::collectCMODFiles(const std::string& modulePath) {
    std::vector<ZipEntry> entries;
    
    // Collect files from src/ directory
    std::string srcPath = FileSystem::joinPath(modulePath, "src");
    auto srcFiles = FileSystem::listAll(srcPath, true);
    
    for (const auto& file : srcFiles) {
        if (FileSystem::isFile(file)) {
            std::string relativePath = file.substr(srcPath.length());
            if (relativePath.front() == '/' || relativePath.front() == '\\') {
                relativePath = relativePath.substr(1);
            }
            
            ZipEntry entry;
            entry.name = "src/" + normalizePath(relativePath);
            entry.content = FileSystem::readFile(file);
            entry.uncompressedSize = entry.content.length();
            entry.isDirectory = false;
            
            entries.push_back(entry);
        }
    }
    
    // Collect files from info/ directory
    std::string infoPath = FileSystem::joinPath(modulePath, "info");
    auto infoFiles = FileSystem::listAll(infoPath, true);
    
    for (const auto& file : infoFiles) {
        if (FileSystem::isFile(file)) {
            std::string relativePath = file.substr(infoPath.length());
            if (relativePath.front() == '/' || relativePath.front() == '\\') {
                relativePath = relativePath.substr(1);
            }
            
            ZipEntry entry;
            entry.name = "info/" + normalizePath(relativePath);
            entry.content = FileSystem::readFile(file);
            entry.uncompressedSize = entry.content.length();
            entry.isDirectory = false;
            
            entries.push_back(entry);
        }
    }
    
    return entries;
}

std::vector<ZipEntry> ZipUtil::collectCJMODFiles(const std::string& modulePath) {
    std::vector<ZipEntry> entries;
    
    // Collect files from src/ directory
    std::string srcPath = FileSystem::joinPath(modulePath, "src");
    auto srcFiles = FileSystem::listAll(srcPath, true);
    
    for (const auto& file : srcFiles) {
        if (FileSystem::isFile(file)) {
            std::string relativePath = file.substr(srcPath.length());
            if (relativePath.front() == '/' || relativePath.front() == '\\') {
                relativePath = relativePath.substr(1);
            }
            
            ZipEntry entry;
            entry.name = "src/" + normalizePath(relativePath);
            entry.content = FileSystem::readFile(file);
            entry.uncompressedSize = entry.content.length();
            entry.isDirectory = false;
            
            entries.push_back(entry);
        }
    }
    
    // Collect files from info/ directory
    std::string infoPath = FileSystem::joinPath(modulePath, "info");
    auto infoFiles = FileSystem::listAll(infoPath, true);
    
    for (const auto& file : infoFiles) {
        if (FileSystem::isFile(file)) {
            std::string relativePath = file.substr(infoPath.length());
            if (relativePath.front() == '/' || relativePath.front() == '\\') {
                relativePath = relativePath.substr(1);
            }
            
            ZipEntry entry;
            entry.name = "info/" + normalizePath(relativePath);
            entry.content = FileSystem::readFile(file);
            entry.uncompressedSize = entry.content.length();
            entry.isDirectory = false;
            
            entries.push_back(entry);
        }
    }
    
    return entries;
}

std::string ZipUtil::normalizePath(const std::string& path) {
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    // Remove double slashes
    size_t pos = 0;
    while ((pos = normalized.find("//", pos)) != std::string::npos) {
        normalized.replace(pos, 2, "/");
        pos += 1;
    }
    
    return normalized;
}

} // namespace CHTL::Util