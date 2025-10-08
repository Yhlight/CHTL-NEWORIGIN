#include "ModulePacker.h"
#include "../../Util/FileSystem/FileSystem.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <filesystem>
#include <fstream>
#include <sstream>

// 条件编译：仅在libzip可用时包含
#ifdef HAVE_LIBZIP
#include <zip.h>
#endif

namespace CHTL {
namespace CMOD {

// ========================================
// ModulePacker 实现 - 使用 libzip
// ========================================

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
        ss << "],\n";
    }
    
    // 导出元素
    auto customElements = moduleData.exports.getExportsByType(ExportEntry::Type::CustomElement);
    if (!customElements.empty()) {
        ss << "    \"customElements\": [";
        for (size_t i = 0; i < customElements.size(); i++) {
            ss << "\"" << customElements[i] << "\"";
            if (i < customElements.size() - 1) ss << ", ";
        }
        ss << "],\n";
    }
    
    // 导出变量组
    auto customVars = moduleData.exports.getExportsByType(ExportEntry::Type::CustomVar);
    if (!customVars.empty()) {
        ss << "    \"customVars\": [";
        for (size_t i = 0; i < customVars.size(); i++) {
            ss << "\"" << customVars[i] << "\"";
            if (i < customVars.size() - 1) ss << ", ";
        }
        ss << "]\n";
    }
    
    ss << "  }";
    
    if (moduleData.hasSubModules()) {
        ss << ",\n  \"subModules\": [";
        for (size_t i = 0; i < moduleData.subModules.size(); i++) {
            ss << "\"" << moduleData.subModules[i].name << "\"";
            if (i < moduleData.subModules.size() - 1) ss << ", ";
        }
        ss << "]";
    }
    
    ss << "\n}";
    
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
#ifdef HAVE_LIBZIP
    namespace fs = std::filesystem;
    
    // 使用 libzip 创建 zip 文件
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!archive) {
        zip_error_t ziperror;
        zip_error_init_with_code(&ziperror, error);
        zip_error_fini(&ziperror);
        return false;
    }
    
    // 收集所有文件
    auto files = collectModuleFiles(dirPath);
    
    // 添加文件到 zip
    for (const auto& filePath : files) {
        // 计算相对路径
        fs::path relative = fs::relative(filePath, fs::path(dirPath).parent_path());
        String archiveName = relative.string();
        
        // 读取文件内容
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            continue;
        }
        
        // 获取文件大小
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // 读取文件数据
        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);
        
        // 从内存创建 zip source
        zip_source_t* source = zip_source_buffer(archive, buffer.data(), buffer.size(), 0);
        if (!source) {
            continue;
        }
        
        // 添加文件到归档
        zip_int64_t index = zip_file_add(archive, archiveName.c_str(), source, ZIP_FL_OVERWRITE);
        if (index < 0) {
            zip_source_free(source);
            continue;
        }
    }
    
    // 关闭归档
    if (zip_close(archive) < 0) {
        zip_discard(archive);
        return false;
    }
    
    return true;
#else
    // libzip 不可用时，返回false并提示
    (void)dirPath;
    (void)zipPath;
    return false;
#endif
}

bool ModulePacker::decompressZip(const String& zipPath, const String& outputDir) {
#ifdef HAVE_LIBZIP
    namespace fs = std::filesystem;
    
    // 创建输出目录
    fs::create_directories(outputDir);
    
    // 使用 libzip 打开 zip 文件
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);
    
    if (!archive) {
        return false;
    }
    
    // 获取文件数量
    zip_int64_t numEntries = zip_get_num_entries(archive, 0);
    
    // 解压每个文件
    for (zip_int64_t i = 0; i < numEntries; i++) {
        // 获取文件信息
        struct zip_stat st;
        zip_stat_init(&st);
        
        if (zip_stat_index(archive, i, 0, &st) != 0) {
            continue;
        }
        
        // 构建输出路径
        String outPath = outputDir + "/" + st.name;
        
        // 创建目录
        fs::path p(outPath);
        fs::create_directories(p.parent_path());
        
        // 打开 zip 中的文件
        zip_file_t* file = zip_fopen_index(archive, i, 0);
        if (!file) {
            continue;
        }
        
        // 读取文件内容
        std::vector<char> buffer(st.size);
        zip_int64_t bytesRead = zip_fread(file, buffer.data(), st.size);
        
        if (bytesRead != (zip_int64_t)st.size) {
            zip_fclose(file);
            continue;
        }
        
        // 写入文件
        std::ofstream outFile(outPath, std::ios::binary);
        if (outFile) {
            outFile.write(buffer.data(), st.size);
        }
        
        zip_fclose(file);
    }
    
    zip_close(archive);
    return true;
#else
    // libzip 不可用时，返回false并提示
    (void)zipPath;
    (void)outputDir;
    return false;
#endif
}

// ========================================
// CJMODPacker 实现
// ========================================

CJMODPacker::CJMODPacker() {}

bool CJMODPacker::packCJMOD(const String& modulePath, const String& outputPath) {
    if (!validateCJMODStructure(modulePath)) {
        return false;
    }
    
    // 复用 ModulePacker 的压缩功能
    return compressDirectory(modulePath, outputPath);
}

bool CJMODPacker::unpackCJMOD(const String& cjmodPath, const String& outputPath) {
    return decompressZip(cjmodPath, outputPath);
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

bool CJMODPacker::compressDirectory(const String& dirPath, const String& zipPath) {
#ifdef HAVE_LIBZIP
    // 使用 libzip
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);
    
    if (!archive) {
        return false;
    }
    
    namespace fs = std::filesystem;
    
    // 收集文件
    for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file()) {
            fs::path relative = fs::relative(entry.path(), fs::path(dirPath).parent_path());
            String archiveName = relative.string();
            
            std::ifstream file(entry.path().string(), std::ios::binary);
            if (!file) continue;
            
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            
            std::vector<char> buffer(fileSize);
            file.read(buffer.data(), fileSize);
            
            zip_source_t* source = zip_source_buffer(archive, buffer.data(), buffer.size(), 0);
            if (!source) continue;
            
            if (zip_file_add(archive, archiveName.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
                zip_source_free(source);
            }
        }
    }
    
    if (zip_close(archive) < 0) {
        zip_discard(archive);
        return false;
    }
    
    return true;
#else
    // libzip 不可用时，返回false并提示
    (void)dirPath;
    (void)zipPath;
    return false;
#endif
}

bool CJMODPacker::decompressZip(const String& zipPath, const String& outputDir) {
#ifdef HAVE_LIBZIP
    namespace fs = std::filesystem;
    
    fs::create_directories(outputDir);
    
    int error = 0;
    zip_t* archive = zip_open(zipPath.c_str(), ZIP_RDONLY, &error);
    
    if (!archive) {
        return false;
    }
    
    zip_int64_t numEntries = zip_get_num_entries(archive, 0);
    
    for (zip_int64_t i = 0; i < numEntries; i++) {
        struct zip_stat st;
        zip_stat_init(&st);
        
        if (zip_stat_index(archive, i, 0, &st) != 0) {
            continue;
        }
        
        String outPath = outputDir + "/" + st.name;
        fs::path p(outPath);
        fs::create_directories(p.parent_path());
        
        zip_file_t* file = zip_fopen_index(archive, i, 0);
        if (!file) continue;
        
        std::vector<char> buffer(st.size);
        zip_int64_t bytesRead = zip_fread(file, buffer.data(), st.size);
        
        if (bytesRead == (zip_int64_t)st.size) {
            std::ofstream outFile(outPath, std::ios::binary);
            if (outFile) {
                outFile.write(buffer.data(), st.size);
            }
        }
        
        zip_fclose(file);
    }
    
    zip_close(archive);
    return true;
#else
    // libzip 不可用时，返回false并提示
    (void)zipPath;
    (void)outputDir;
    return false;
#endif
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
    saveCacheToDisk(moduleName, data);
}

Optional<ModuleData> ModuleCache::loadFromCache(const String& moduleName) {
    // 先检查内存缓存
    auto it = memoryCache_.find(moduleName);
    if (it != memoryCache_.end()) {
        return it->second;
    }
    
    // 从磁盘加载
    if (loadCacheFromDisk(moduleName)) {
        auto it2 = memoryCache_.find(moduleName);
        if (it2 != memoryCache_.end()) {
            return it2->second;
        }
    }
    
    return std::nullopt;
}

void ModuleCache::clearCache() {
    memoryCache_.clear();
    
    // 清除磁盘缓存
    namespace fs = std::filesystem;
    if (fs::exists(cacheDirectory_)) {
        fs::remove_all(cacheDirectory_);
    }
}

void ModuleCache::clearModuleCache(const String& moduleName) {
    memoryCache_.erase(moduleName);
    
    // 清除磁盘缓存文件
    namespace fs = std::filesystem;
    String cachePath = getCacheFilePath(moduleName);
    if (fs::exists(cachePath)) {
        fs::remove(cachePath);
    }
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

bool ModuleCache::loadCacheFromDisk(const String& moduleName) {
    namespace fs = std::filesystem;
    
    String cachePath = getCacheFilePath(moduleName);
    if (!fs::exists(cachePath)) {
        return false;
    }
    
    // 从磁盘反序列化模块缓存
    std::ifstream file(cachePath);
    if (!file) {
        return false;
    }
    
    ModuleData data;
    String line;
    
    // 读取版本
    if (std::getline(file, line) && line.find("version:") == 0) {
        data.info.version = line.substr(8);
    } else {
        return false;
    }
    
    // 跳过时间戳行（如果有）
    if (std::getline(file, line) && line.find("timestamp:") != 0) {
        // 不是时间戳，可能是其他字段，回退一行
        file.seekg(-(line.length() + 1), std::ios::cur);
    }
    
    // 读取依赖数量
    if (std::getline(file, line) && line.find("dependencies:") == 0) {
        size_t depCount = std::stoul(line.substr(13));
        for (size_t i = 0; i < depCount; i++) {
            if (std::getline(file, line)) {
                data.info.dependencies.push_back(line);
            }
        }
    }
    
    // 成功读取基本信息
    // 实际模块内容在需要时从模块路径加载
    return true;
}

void ModuleCache::saveCacheToDisk(const String& moduleName, const ModuleData& data) {
    namespace fs = std::filesystem;
    
    // 确保缓存目录存在
    fs::create_directories(cacheDirectory_);
    
    String cachePath = getCacheFilePath(moduleName);
    
    // 序列化模块缓存到磁盘
    std::ofstream file(cachePath);
    if (!file) {
        return;
    }
    
    // 写入模块信息
    file << "name:" << data.info.name << "\n";
    file << "version:" << data.info.version << "\n";
    file << "description:" << data.info.description << "\n";
    
    // 写入依赖
    file << "dependencies:" << data.info.dependencies.size() << "\n";
    for (const auto& dep : data.info.dependencies) {
        file << dep << "\n";
    }
    
    file.close();
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
