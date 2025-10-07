#include "ModulePacker.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/StringUtil/StringUtil.h"
#include "../../../third-party/miniz.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace CHTL {
namespace CMOD {

ModulePacker::ModulePacker() {}

bool ModulePacker::packModule(const String& modulePath, const String& outputPath) {
    if (!validateModuleStructure(modulePath)) {
        return false;
    }
    return compressDirectory(modulePath, outputPath);
}

bool ModulePacker::unpackModule(const String& cmodPath, const String& outputPath) {
    return decompressZip(cmodPath, outputPath);
}

bool ModulePacker::validateModuleStructure(const String& modulePath) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(modulePath) || !fs::is_directory(modulePath)) {
        return false;
    }
    
    String srcPath = modulePath + "/src";
    String infoPath = modulePath + "/info";
    
    if (!fs::exists(srcPath) || !fs::exists(infoPath)) {
        return false;
    }
    
    String moduleName = fs::path(modulePath).filename().string();
    String infoFile = infoPath + "/" + moduleName + ".chtl";
    
    if (!fs::exists(infoFile)) {
        return false;
    }
    
    std::ifstream file(infoFile);
    if (!file) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    String content = buffer.str();
    
    if (content.find("[Info]") == String::npos) {
        return false;
    }
    
    return true;
}

bool ModulePacker::validateSubModule(const String& subModulePath) {
    return validateModuleStructure(subModulePath);
}

String ModulePacker::generateJSONManifest(const ModuleData& moduleData) {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"name\": \"" << moduleData.info.name << "\",\n";
    ss << "  \"version\": \"" << moduleData.info.version << "\",\n";
    ss << "  \"description\": \"" << moduleData.info.description << "\",\n";
    ss << "  \"author\": \"" << moduleData.info.author << "\",\n";
    ss << "  \"license\": \"" << moduleData.info.license << "\",\n";
    ss << "  \"category\": \"" << moduleData.info.category << "\",\n";
    ss << "  \"type\": \"cmod\"\n";
    ss << "}";
    
    return ss.str();
}

Vector<String> ModulePacker::collectModuleFiles(const String& modulePath) {
    namespace fs = std::filesystem;
    
    Vector<String> files;
    
    for (const auto& entry : fs::recursive_directory_iterator(modulePath)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    
    return files;
}

bool ModulePacker::compressDirectory(const String& dirPath, const String& zipPath) {
    namespace fs = std::filesystem;
    
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    
    if (!mz_zip_writer_init_file(&zip, zipPath.c_str(), 0)) {
        return false;
    }
    
    auto files = collectModuleFiles(dirPath);
    
    for (const auto& filePath : files) {
        fs::path relative = fs::relative(filePath, fs::path(dirPath).parent_path());
        String archiveName = relative.string();
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            continue;
        }
        
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        
        if (!mz_zip_writer_add_mem(&zip, archiveName.c_str(), 
                                    buffer.data(), buffer.size(),
                                    MZ_DEFAULT_COMPRESSION)) {
            mz_zip_writer_end(&zip);
            return false;
        }
    }
    
    if (!mz_zip_writer_finalize_archive(&zip)) {
        mz_zip_writer_end(&zip);
        return false;
    }
    
    mz_zip_writer_end(&zip);
    return true;
}

bool ModulePacker::decompressZip(const String& zipPath, const String& outputDir) {
    namespace fs = std::filesystem;
    
    fs::create_directories(outputDir);
    
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    
    if (!mz_zip_reader_init_file(&zip, zipPath.c_str(), 0)) {
        return false;
    }
    
    int fileCount = (int)mz_zip_reader_get_num_files(&zip);
    
    for (int i = 0; i < fileCount; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zip, i, &file_stat)) {
            continue;
        }
        
        String outPath = outputDir + "/" + file_stat.m_filename;
        
        fs::path p(outPath);
        fs::create_directories(p.parent_path());
        
        if (!mz_zip_reader_extract_to_file(&zip, i, outPath.c_str(), 0)) {
            mz_zip_reader_end(&zip);
            return false;
        }
    }
    
    mz_zip_reader_end(&zip);
    return true;
}

// Other classes implementations...
CJMODPacker::CJMODPacker() {}
bool CJMODPacker::packCJMOD(const String&, const String&) { return false; }
bool CJMODPacker::unpackCJMOD(const String&, const String&) { return false; }
bool CJMODPacker::validateCJMODStructure(const String&) { return false; }
String CJMODPacker::generateCJMODManifest(const ModuleData&) { return ""; }
bool CJMODPacker::compressDirectory(const String&, const String&) { return false; }
bool CJMODPacker::decompressZip(const String&, const String&) { return false; }

ModuleCache::ModuleCache() : cacheDirectory_(".chtl_cache/modules") {}
void ModuleCache::setCacheDirectory(const String& path) { cacheDirectory_ = path; }
void ModuleCache::cacheModule(const String& moduleName, const ModuleData& data) {
    memoryCache_[moduleName] = data;
}
Optional<ModuleData> ModuleCache::loadFromCache(const String& moduleName) {
    auto it = memoryCache_.find(moduleName);
    if (it != memoryCache_.end()) {
        return it->second;
    }
    return std::nullopt;
}
void ModuleCache::clearCache() { memoryCache_.clear(); }
void ModuleCache::clearModuleCache(const String& moduleName) {
    memoryCache_.erase(moduleName);
}
bool ModuleCache::isCacheValid(const String& moduleName, const String& moduleVersion) {
    auto cached = loadFromCache(moduleName);
    if (!cached.has_value()) {
        return false;
    }
    return cached->info.version == moduleVersion;
}
String ModuleCache::getCacheFilePath(const String& moduleName) const {
    return cacheDirectory_ + "/" + moduleName + ".cache";
}
bool ModuleCache::loadCacheFromDisk(const String&) { return false; }
void ModuleCache::saveCacheToDisk(const String&, const ModuleData&) {}

DependencyManager::DependencyManager() {}
Vector<String> DependencyManager::resolveDependencies(const ModuleData& module) {
    Vector<String> resolved;
    for (const auto& dep : module.info.dependencies) {
        if (!dep.empty()) {
            resolved.push_back(dep);
        }
    }
    return resolved;
}
bool DependencyManager::hasCircularDependency(const String&, const Vector<String>&) { return false; }
Vector<String> DependencyManager::getDependencyInstallOrder(const Vector<String>&) { return {}; }
void DependencyManager::buildDependencyGraph(const String&, const Vector<String>&) {}
bool DependencyManager::detectCycle(const String&, HashMap<String, int>&, HashMap<String, bool>&) { return false; }
void DependencyManager::topologicalSort(const String&, HashMap<String, bool>&, Vector<String>&) {}

} // namespace CMOD
} // namespace CHTL
