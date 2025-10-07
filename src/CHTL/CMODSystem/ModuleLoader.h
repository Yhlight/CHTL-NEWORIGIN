#ifndef CHTL_MODULE_LOADER_H
#define CHTL_MODULE_LOADER_H

#include "../../Common.h"
#include "ModuleInfo.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <filesystem>

namespace CHTL {
namespace CMOD {

// 模块加载器
class ModuleLoader {
public:
    ModuleLoader();
    
    // 设置模块搜索路径
    void addSearchPath(const String& path);
    void setOfficialModulePath(const String& path);
    void setUserModulePath(const String& path);
    
    // 加载模块
    Optional<ModuleData> loadModule(const String& moduleName);
    Optional<ModuleData> loadModuleFromPath(const String& path);
    
    // 加载.cmod文件（打包的模块）
    Optional<ModuleData> loadCMODFile(const String& filePath);
    
    // 加载.cjmod文件
    Optional<ModuleData> loadCJMODFile(const String& filePath);
    
    // 解析info文件
    Optional<ModuleInfo> parseInfoFile(const String& filePath);
    Optional<ModuleExports> parseExportFile(const String& filePath);
    
    // 获取当前CHTL版本
    static String getCHTLVersion() { return "2.5.0"; }
    
private:
    Vector<String> searchPaths_;
    String officialModulePath_;
    String userModulePath_;
    
    // 辅助方法
    Optional<String> findModule(const String& moduleName);
    Optional<String> findInPath(const String& path, const String& moduleName, 
                                 bool searchCMOD, bool searchCHTL, bool searchCJMOD);
    
    bool isOrderedStructure(const String& path) const;
    ModuleData loadFromDirectory(const String& dirPath);
    ModuleData loadFromCMODFile(const String& filePath);
    
    // 解析[Info]块
    ModuleInfo parseInfoBlock(const String& content);
    // 解析[Export]块
    ModuleExports parseExportBlock(const String& content);
    
    // 读取目录中的所有子模块
    Vector<SubModuleInfo> loadSubModules(const String& srcPath);
};

// 模块解析器 - 处理导入路径
class ModuleResolver {
public:
    enum class ImportType {
        Chtl,           // @Chtl
        CJmod,          // @CJmod
        Html,           // @Html
        Style,          // @Style
        JavaScript,     // @JavaScript
        Custom,         // 自定义类型
        CustomElement,  // [Custom] @Element
        CustomStyle,    // [Custom] @Style
        CustomVar,      // [Custom] @Var
        TemplateElement,// [Template] @Element
        TemplateStyle,  // [Template] @Style
        TemplateVar,    // [Template] @Var
        Origin,         // [Origin]
        Configuration,  // @Config
    };
    
    struct ImportSpec {
        ImportType type;
        String targetName;     // 要导入的具体项名称（精确导入）
        String path;           // 路径
        String alias;          // 别名
        bool isWildcard;       // 是否是通配符导入
        String customTypeName; // 自定义类型名称
        
        ImportSpec() : type(ImportType::Chtl), isWildcard(false) {}
    };
    
    ModuleResolver();
    
    // 解析导入语句
    Optional<ImportSpec> parseImport(const String& importStatement);
    
    // 解析路径
    String resolvePath(const String& basePath, const String& importPath);
    
    // 判断是否是官方模块路径
    bool isOfficialModule(const String& path) const;
    
    // 提取模块名称
    static String extractModuleName(const String& path);
    
private:
    String officialPrefix_;  // "chtl::"
};

} // namespace CMOD
} // namespace CHTL

#endif // CHTL_MODULE_LOADER_H
