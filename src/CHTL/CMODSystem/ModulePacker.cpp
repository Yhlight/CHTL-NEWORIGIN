#include "ModulePacker.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace CHTL {
namespace CMOD {

// ========================================
// ModulePacker 实现
// 注：压缩功能留作后续实现，当前专注于模块加载
// ========================================

ModulePacker::ModulePacker() {}

bool ModulePacker::packModule(const String& modulePath, const String& outputPath) {
    if (!validateModuleStructure(modulePath)) {
        return false;
    }
    
    // TODO: 实现使用 miniz 的压缩功能
    // 当前返回false表示功能尚未完全实现
    (void)outputPath;  // 避免未使用参数警告
    return false;
}

bool ModulePacker::unpackModule(const String& cmodPath, const String& outputPath) {
    // TODO: 实现解压功能
    (void)cmodPath;
    (void)outputPath;
    return false;
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
    ss << "  \"type\": ";
    
    switch (moduleData.info.type) {
        case ModuleType::CMOD:
            ss << "\"cmod\"";
            break;
        case ModuleType::CJMOD:
            ss << "\"cjmod\"";
            break;
        case ModuleType::Hybrid:
            ss << "\"hybrid\"";
            break;
    }
    
    ss << ",\n";
    ss << "  \"exports\": {\n";
    
    // 导出样式组
    auto customStyles = moduleData.exports.getExportsByType(ExportEntry::Type::CustomStyle);
    if (!customStyles.empty()) {
        ss << "    \"customStyles\": [";
        for (size_t i = 0; i < customStyles.size(); i++) {
            ss << "\"" << customStyles[i] << "\"";
            if (i < customStyles.size() - 1) ss << ", ";
        }
        ss << "]\n";
    }
    
    ss << "  }\n";
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

bool ModulePacker::compressDirectory(const String&, const String&) {
    // 存根实现
    return false;
}

bool ModulePacker::decompressZip(const String&, const String&) {
    // 存根实现
    return false;
}

// ========================================
// CJMODPacker 实现
// ========================================

CJMODPacker::CJMODPacker() {}

bool CJMODPacker::packCJMOD(const String&, const String&) {
    return false;
}

bool CJMODPacker::unpackCJMOD(const String&, const String&) {
    return false;
}

bool CJMODPacker::validateCJMODStructure(const String& modulePath) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(modulePath) || !fs::is_directory(modulePath)) {
        return false;
    }
    
    String srcPath = modulePath + "/src";
    String infoPath = modulePath + "/info";
    
    if (!fs::exists(srcPath) || !fs::exists(infoPath)) {
        return false;
    }
    
    // 检查src中是否有.cpp/.h文件
    bool hasCppFiles = false;
    for (const auto& entry : fs::directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            String ext = entry.path().extension().string();
            if (ext == ".cpp" || ext == ".h") {
                hasCppFiles = true;
                break;
            }
        }
    }
    
    return hasCppFiles;
}

String CJMODPacker::generateCJMODManifest(const ModuleData& moduleData) {
    std::stringstream ss;
    
    ss << "{\n";
    ss << "  \"name\": \"" << moduleData.info.name << "\",\n";
    ss << "  \"version\": \"" << moduleData.info.version << "\",\n";
    ss << "  \"description\": \"" << moduleData.info.description << "\",\n";
    ss << "  \"type\": \"cjmod\"\n";
    ss << "}";
    
    return ss.str();
}

bool CJMODPacker::compressDirectory(const String&, const String&) {
    return false;
}

bool CJMODPacker::decompressZip(const String&, const String&) {
    return false;
}

// ========================================
// ModuleCache 实现
// ========================================

ModuleCache::ModuleCache() : cacheDirectory_(".chtl_cache/modules") {}

void ModuleCache::setCacheDirectory(const String& path) {
    cacheDirectory_ = path;
}

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

void ModuleCache::clearCache() {
    memoryCache_.clear();
}

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

bool ModuleCache::loadCacheFromDisk(const String&) {
    return false;
}

void ModuleCache::saveCacheToDisk(const String&, const ModuleData&) {
    // 存根实现
}

// ========================================
// DependencyManager 实现
// ========================================

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

bool DependencyManager::hasCircularDependency(const String& moduleName, 
                                                const Vector<String>& dependencies) {
    buildDependencyGraph(moduleName, dependencies);
    
    HashMap<String, int> visited;
    HashMap<String, bool> recStack;
    
    return detectCycle(moduleName, visited, recStack);
}

Vector<String> DependencyManager::getDependencyInstallOrder(const Vector<String>& modules) {
    Vector<String> order;
    HashMap<String, bool> visited;
    
    for (const auto& module : modules) {
        if (visited.find(module) == visited.end()) {
            topologicalSort(module, visited, order);
        }
    }
    
    std::reverse(order.begin(), order.end());
    return order;
}

void DependencyManager::buildDependencyGraph(const String& moduleName,
                                              const Vector<String>& dependencies) {
    dependencyGraph_[moduleName] = dependencies;
}

bool DependencyManager::detectCycle(const String& current,
                                     HashMap<String, int>& visited,
                                     HashMap<String, bool>& recStack) {
    visited[current] = 1;
    recStack[current] = true;
    
    auto it = dependencyGraph_.find(current);
    if (it != dependencyGraph_.end()) {
        for (const auto& dep : it->second) {
            if (visited.find(dep) == visited.end()) {
                if (detectCycle(dep, visited, recStack)) {
                    return true;
                }
            } else if (recStack[dep]) {
                return true;
            }
        }
    }
    
    recStack[current] = false;
    return false;
}

void DependencyManager::topologicalSort(const String& module,
                                         HashMap<String, bool>& visited,
                                         Vector<String>& stack) {
    visited[module] = true;
    
    auto it = dependencyGraph_.find(module);
    if (it != dependencyGraph_.end()) {
        for (const auto& dep : it->second) {
            if (visited.find(dep) == visited.end()) {
                topologicalSort(dep, visited, stack);
            }
        }
    }
    
    stack.push_back(module);
}

} // namespace CMOD
} // namespace CHTL
