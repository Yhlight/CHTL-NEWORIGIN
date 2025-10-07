#ifndef CHTL_MODULE_PACKER_H
#define CHTL_MODULE_PACKER_H

#include "../../Common.h"
#include "ModuleInfo.h"

namespace CHTL {
namespace CMOD {

// CMOD打包器
class ModulePacker {
public:
    ModulePacker();
    
    // 打包模块文件夹为.cmod文件
    bool packModule(const String& modulePath, const String& outputPath);
    
    // 解包.cmod文件
    bool unpackModule(const String& cmodPath, const String& outputPath);
    
    // 验证模块结构
    bool validateModuleStructure(const String& modulePath);
    
    // 生成模块的JSON查询表（用于VSCode IDE）
    String generateJSONManifest(const ModuleData& moduleData);
    
    // 使用 libzip 压缩/解压（公开以供 ModuleLoader 使用）
    bool compressDirectory(const String& dirPath, const String& zipPath);
    bool decompressZip(const String& zipPath, const String& outputDir);
    
private:
    // 收集模块文件
    Vector<String> collectModuleFiles(const String& modulePath);
    
    // 验证子模块结构
    bool validateSubModule(const String& subModulePath);
};

// CJMOD打包器
class CJMODPacker {
public:
    CJMODPacker();
    
    // 打包CJMOD模块
    bool packCJMOD(const String& modulePath, const String& outputPath);
    
    // 解包CJMOD模块
    bool unpackCJMOD(const String& cjmodPath, const String& outputPath);
    
    // 验证CJMOD结构
    bool validateCJMODStructure(const String& modulePath);
    
    // 生成CJMOD的JSON查询表
    String generateCJMODManifest(const ModuleData& moduleData);
    
private:
    bool compressDirectory(const String& dirPath, const String& zipPath);
    bool decompressZip(const String& zipPath, const String& outputDir);
};

// 模块缓存管理器
class ModuleCache {
public:
    ModuleCache();
    
    // 设置缓存目录
    void setCacheDirectory(const String& path);
    
    // 缓存模块数据
    void cacheModule(const String& moduleName, const ModuleData& data);
    
    // 从缓存加载模块
    Optional<ModuleData> loadFromCache(const String& moduleName);
    
    // 清除缓存
    void clearCache();
    void clearModuleCache(const String& moduleName);
    
    // 检查缓存是否有效
    bool isCacheValid(const String& moduleName, const String& moduleVersion);
    
private:
    String cacheDirectory_;
    HashMap<String, ModuleData> memoryCache_;
    
    String getCacheFilePath(const String& moduleName) const;
    bool loadCacheFromDisk(const String& moduleName);
    void saveCacheToDisk(const String& moduleName, const ModuleData& data);
};

// 依赖管理器
class DependencyManager {
public:
    DependencyManager();
    
    // 解析依赖关系
    Vector<String> resolveDependencies(const ModuleData& module);
    
    // 检查循环依赖
    bool hasCircularDependency(const String& moduleName, 
                                const Vector<String>& dependencies);
    
    // 获取依赖安装顺序
    Vector<String> getDependencyInstallOrder(const Vector<String>& modules);
    
private:
    HashMap<String, Vector<String>> dependencyGraph_;
    
    void buildDependencyGraph(const String& moduleName, 
                              const Vector<String>& dependencies);
    bool detectCycle(const String& current, 
                     HashMap<String, int>& visited,
                     HashMap<String, bool>& recStack);
    void topologicalSort(const String& module, 
                        HashMap<String, bool>& visited,
                        Vector<String>& stack);
};

} // namespace CMOD
} // namespace CHTL

#endif // CHTL_MODULE_PACKER_H
