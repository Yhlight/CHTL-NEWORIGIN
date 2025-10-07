#include "ModuleLoader.h"
#include "../../Util/StringUtil/StringUtil.h"
#include <fstream>
#include <sstream>

namespace CHTL {
namespace CMOD {

ModuleLoader::ModuleLoader() {
    officialModulePath_ = "./module";
}

void ModuleLoader::addSearchPath(const String& path) {
    searchPaths_.push_back(path);
}

void ModuleLoader::setOfficialModulePath(const String& path) {
    officialModulePath_ = path;
}

void ModuleLoader::setUserModulePath(const String& path) {
    userModulePath_ = path;
}

Optional<ModuleData> ModuleLoader::loadModule(const String& moduleName) {
    auto modulePath = findModule(moduleName);
    if (!modulePath.has_value()) {
        return std::nullopt;
    }
    
    return loadModuleFromPath(modulePath.value());
}

Optional<ModuleData> ModuleLoader::loadModuleFromPath(const String& path) {
    namespace fs = std::filesystem;
    
    if (Util::StringUtil::endsWith(path, ".cmod")) {
        return loadCMODFile(path);
    }
    
    if (Util::StringUtil::endsWith(path, ".cjmod")) {
        return loadCJMODFile(path);
    }
    
    if (fs::is_directory(path)) {
        return loadFromDirectory(path);
    }
    
    if (Util::StringUtil::endsWith(path, ".chtl")) {
        ModuleData data;
        std::ifstream file(path);
        if (!file) {
            return std::nullopt;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        data.info.name = fs::path(path).stem().string();
        data.info.version = "1.0.0";
        data.info.type = ModuleType::CMOD;
        data.sourceFiles[fs::path(path).filename().string()] = buffer.str();
        
        return data;
    }
    
    return std::nullopt;
}

Optional<ModuleData> ModuleLoader::loadCMODFile(const String&) {
    // TODO: Implement .cmod unpacking
    return std::nullopt;
}

Optional<ModuleData> ModuleLoader::loadCJMODFile(const String&) {
    // TODO: Implement .cjmod loading
    return std::nullopt;
}

Optional<ModuleInfo> ModuleLoader::parseInfoFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    String content = buffer.str();
    
    return parseInfoBlock(content);
}

Optional<ModuleExports> ModuleLoader::parseExportFile(const String& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    String content = buffer.str();
    
    return parseExportBlock(content);
}

ModuleInfo ModuleLoader::parseInfoBlock(const String& content) {
    ModuleInfo info;
    
    size_t infoStart = content.find("[Info]");
    if (infoStart == String::npos) {
        return info;
    }
    
    size_t blockStart = content.find("{", infoStart);
    size_t blockEnd = content.find("}", blockStart);
    if (blockStart == String::npos || blockEnd == String::npos) {
        return info;
    }
    
    String block = content.substr(blockStart + 1, blockEnd - blockStart - 1);
    
    auto lines = Util::StringUtil::split(block, '\n');
    for (const auto& line : lines) {
        auto trimmed = Util::StringUtil::trim(line);
        if (trimmed.empty() || Util::StringUtil::startsWith(trimmed, "//")) {
            continue;
        }
        
        auto pos = trimmed.find('=');
        if (pos == String::npos) {
            continue;
        }
        
        String key = Util::StringUtil::trim(trimmed.substr(0, pos));
        String value = Util::StringUtil::trim(trimmed.substr(pos + 1));
        
        if (Util::StringUtil::endsWith(value, ";")) {
            value = value.substr(0, value.length() - 1);
        }
        value = Util::StringUtil::trim(value);
        if ((Util::StringUtil::startsWith(value, "\"") && Util::StringUtil::endsWith(value, "\"")) ||
            (Util::StringUtil::startsWith(value, "'") && Util::StringUtil::endsWith(value, "'"))) {
            value = value.substr(1, value.length() - 2);
        }
        
        if (key == "name") {
            info.name = value;
        } else if (key == "version") {
            info.version = value;
        } else if (key == "description") {
            info.description = value;
        } else if (key == "author") {
            info.author = value;
        } else if (key == "license") {
            info.license = value;
        } else if (key == "category") {
            info.category = value;
        } else if (key == "minCHTLVersion") {
            info.minCHTLVersion = value;
        } else if (key == "maxCHTLVersion") {
            info.maxCHTLVersion = value;
        } else if (key == "dependencies") {
            if (!value.empty()) {
                info.dependencies = Util::StringUtil::split(value, ',');
                for (auto& dep : info.dependencies) {
                    dep = Util::StringUtil::trim(dep);
                }
            }
        }
    }
    
    return info;
}

ModuleExports ModuleLoader::parseExportBlock(const String& content) {
    ModuleExports exports;
    
    size_t exportStart = content.find("[Export]");
    if (exportStart == String::npos) {
        return exports;
    }
    
    size_t blockStart = content.find("{", exportStart);
    size_t blockEnd = content.find("}", blockStart);
    if (blockStart == String::npos || blockEnd == String::npos) {
        return exports;
    }
    
    String block = content.substr(blockStart + 1, blockEnd - blockStart - 1);
    
    // 解析每一行
    auto lines = Util::StringUtil::split(block, '\n');
    
    for (const auto& line : lines) {
        auto trimmed = Util::StringUtil::trim(line);
        if (trimmed.empty() || Util::StringUtil::startsWith(trimmed, "//")) {
            continue;
        }
        
        // 查找类型标记
        ExportEntry::Type entryType;
        size_t nameStart = 0;
        
        if (Util::StringUtil::contains(trimmed, "[Custom] @Style")) {
            entryType = ExportEntry::Type::CustomStyle;
            nameStart = trimmed.find("@Style") + 6;
        } else if (Util::StringUtil::contains(trimmed, "[Custom] @Element")) {
            entryType = ExportEntry::Type::CustomElement;
            nameStart = trimmed.find("@Element") + 8;
        } else if (Util::StringUtil::contains(trimmed, "[Custom] @Var")) {
            entryType = ExportEntry::Type::CustomVar;
            nameStart = trimmed.find("@Var") + 4;
        } else if (Util::StringUtil::contains(trimmed, "[Template] @Style")) {
            entryType = ExportEntry::Type::TemplateStyle;
            nameStart = trimmed.find("@Style") + 6;
        } else if (Util::StringUtil::contains(trimmed, "[Template] @Element")) {
            entryType = ExportEntry::Type::TemplateElement;
            nameStart = trimmed.find("@Element") + 8;
        } else if (Util::StringUtil::contains(trimmed, "[Template] @Var")) {
            entryType = ExportEntry::Type::TemplateVar;
            nameStart = trimmed.find("@Var") + 4;
        } else if (Util::StringUtil::contains(trimmed, "[Origin] @Html")) {
            entryType = ExportEntry::Type::OriginHtml;
            nameStart = trimmed.find("@Html") + 5;
        } else if (Util::StringUtil::contains(trimmed, "[Origin] @Style")) {
            entryType = ExportEntry::Type::OriginStyle;
            nameStart = trimmed.find("@Style") + 6;
        } else if (Util::StringUtil::contains(trimmed, "[Origin] @JavaScript")) {
            entryType = ExportEntry::Type::OriginJavaScript;
            nameStart = trimmed.find("@JavaScript") + 11;
        } else if (Util::StringUtil::contains(trimmed, "[Configuration] @Config")) {
            entryType = ExportEntry::Type::Configuration;
            nameStart = trimmed.find("@Config") + 7;
        } else {
            continue;
        }
        
        // 提取名称列表
        if (nameStart < trimmed.length()) {
            String namesStr = Util::StringUtil::trim(trimmed.substr(nameStart));
            
            if (Util::StringUtil::endsWith(namesStr, ";")) {
                namesStr = namesStr.substr(0, namesStr.length() - 1);
            }
            
            auto names = Util::StringUtil::split(namesStr, ',');
            for (auto& name : names) {
                name = Util::StringUtil::trim(name);
                if (!name.empty()) {
                    exports.addExport(entryType, name);
                }
            }
        }
    }
    
    return exports;
}

Optional<String> ModuleLoader::findModule(const String& moduleName) {
    namespace fs = std::filesystem;
    
    if (!officialModulePath_.empty()) {
        auto found = findInPath(officialModulePath_, moduleName, true, true, true);
        if (found.has_value()) {
            return found;
        }
    }
    
    if (!userModulePath_.empty()) {
        auto found = findInPath(userModulePath_, moduleName, true, true, true);
        if (found.has_value()) {
            return found;
        }
    }
    
    auto found = findInPath(".", moduleName, true, true, true);
    if (found.has_value()) {
        return found;
    }
    
    for (const auto& path : searchPaths_) {
        auto found = findInPath(path, moduleName, true, true, true);
        if (found.has_value()) {
            return found;
        }
    }
    
    return std::nullopt;
}

Optional<String> ModuleLoader::findInPath(const String& path, const String& moduleName,
                                           bool searchCMOD, bool searchCHTL, bool searchCJMOD) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(path)) {
        return std::nullopt;
    }
    
    bool ordered = isOrderedStructure(path);
    
    if (ordered) {
        Vector<String> cmodDirs = {"CMOD", "Cmod", "cmod"};
        Vector<String> cjmodDirs = {"CJMOD", "CJmod", "cjmod"};
        
        if (searchCMOD || searchCHTL) {
            for (const auto& dir : cmodDirs) {
                String cmodPath = path + "/" + dir;
                if (fs::exists(cmodPath)) {
                    for (const auto& entry : fs::directory_iterator(cmodPath)) {
                        String filename = entry.path().filename().string();
                        String stem = entry.path().stem().string();
                        
                        if (stem == moduleName) {
                            if ((searchCMOD && Util::StringUtil::endsWith(filename, ".cmod")) ||
                                (searchCHTL && Util::StringUtil::endsWith(filename, ".chtl"))) {
                                return entry.path().string();
                            }
                            if (entry.is_directory()) {
                                return entry.path().string();
                            }
                        }
                    }
                }
            }
        }
        
        if (searchCJMOD) {
            for (const auto& dir : cjmodDirs) {
                String cjmodPath = path + "/" + dir;
                if (fs::exists(cjmodPath)) {
                    for (const auto& entry : fs::directory_iterator(cjmodPath)) {
                        String filename = entry.path().filename().string();
                        String stem = entry.path().stem().string();
                        
                        if (stem == moduleName && Util::StringUtil::endsWith(filename, ".cjmod")) {
                            return entry.path().string();
                        }
                        if (entry.is_directory() && stem == moduleName) {
                            return entry.path().string();
                        }
                    }
                }
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(path)) {
            String filename = entry.path().filename().string();
            String stem = entry.path().stem().string();
            
            if (stem == moduleName) {
                if ((searchCMOD && Util::StringUtil::endsWith(filename, ".cmod")) ||
                    (searchCHTL && Util::StringUtil::endsWith(filename, ".chtl")) ||
                    (searchCJMOD && Util::StringUtil::endsWith(filename, ".cjmod"))) {
                    return entry.path().string();
                }
                if (entry.is_directory()) {
                    return entry.path().string();
                }
            }
        }
    }
    
    return std::nullopt;
}

bool ModuleLoader::isOrderedStructure(const String& path) const {
    namespace fs = std::filesystem;
    
    Vector<String> cmodDirs = {"CMOD", "Cmod", "cmod"};
    Vector<String> cjmodDirs = {"CJMOD", "CJmod", "cjmod"};
    
    for (const auto& dir : cmodDirs) {
        if (fs::exists(path + "/" + dir)) {
            return true;
        }
    }
    
    for (const auto& dir : cjmodDirs) {
        if (fs::exists(path + "/" + dir)) {
            return true;
        }
    }
    
    return false;
}

ModuleData ModuleLoader::loadFromDirectory(const String& dirPath) {
    namespace fs = std::filesystem;
    
    ModuleData data;
    
    String srcPath = dirPath + "/src";
    String infoPath = dirPath + "/info";
    
    if (!fs::exists(srcPath) || !fs::exists(infoPath)) {
        return data;
    }
    
    String moduleName = fs::path(dirPath).filename().string();
    
    String infoFile = infoPath + "/" + moduleName + ".chtl";
    if (fs::exists(infoFile)) {
        auto info = parseInfoFile(infoFile);
        if (info.has_value()) {
            data.info = info.value();
            data.info.modulePath = dirPath;
            data.info.infoFilePath = infoFile;
            data.info.srcPath = srcPath;
        }
        
        auto exports = parseExportFile(infoFile);
        if (exports.has_value()) {
            data.exports = exports.value();
        }
    }
    
    if (fs::exists(srcPath)) {
        String mainFile = srcPath + "/" + moduleName + ".chtl";
        if (fs::exists(mainFile)) {
            std::ifstream file(mainFile);
            std::stringstream buffer;
            buffer << file.rdbuf();
            data.sourceFiles[moduleName + ".chtl"] = buffer.str();
        }
        
        data.subModules = loadSubModules(srcPath);
    }
    
    return data;
}

Vector<SubModuleInfo> ModuleLoader::loadSubModules(const String& srcPath) {
    namespace fs = std::filesystem;
    
    Vector<SubModuleInfo> subModules;
    
    for (const auto& entry : fs::directory_iterator(srcPath)) {
        if (entry.is_directory()) {
            String subName = entry.path().filename().string();
            String subPath = entry.path().string();
            
            if (fs::exists(subPath + "/src") && fs::exists(subPath + "/info")) {
                SubModuleInfo subInfo(subName, subPath);
                
                auto subData = loadFromDirectory(subPath);
                subInfo.info = subData.info;
                
                subModules.push_back(subInfo);
            }
        }
    }
    
    return subModules;
}

ModuleResolver::ModuleResolver() : officialPrefix_("chtl::") {}

Optional<ModuleResolver::ImportSpec> ModuleResolver::parseImport(const String& importStatement) {
    ImportSpec spec;
    
    auto trimmed = Util::StringUtil::trim(importStatement);
    
    // 解析导入类型
    if (Util::StringUtil::contains(trimmed, "[Import] @Chtl")) {
        spec.type = ImportType::Chtl;
    } else if (Util::StringUtil::contains(trimmed, "[Import] @CJmod")) {
        spec.type = ImportType::CJmod;
    } else if (Util::StringUtil::contains(trimmed, "[Import] @Html")) {
        spec.type = ImportType::Html;
    } else if (Util::StringUtil::contains(trimmed, "[Import] @Style")) {
        spec.type = ImportType::Style;
    } else if (Util::StringUtil::contains(trimmed, "[Import] @JavaScript")) {
        spec.type = ImportType::JavaScript;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Custom] @Element")) {
        spec.type = ImportType::CustomElement;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Custom] @Style")) {
        spec.type = ImportType::CustomStyle;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Custom] @Var")) {
        spec.type = ImportType::CustomVar;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Template] @Element")) {
        spec.type = ImportType::TemplateElement;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Template] @Style")) {
        spec.type = ImportType::TemplateStyle;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Template] @Var")) {
        spec.type = ImportType::TemplateVar;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Origin]")) {
        spec.type = ImportType::Origin;
    } else if (Util::StringUtil::contains(trimmed, "[Import] [Configuration]") ||
               Util::StringUtil::contains(trimmed, "[Import] @Config")) {
        spec.type = ImportType::Configuration;
    } else {
        return std::nullopt;
    }
    
    // 查找 from 关键字
    size_t fromPos = trimmed.find(" from ");
    if (fromPos != String::npos) {
        // 提取路径
        String pathPart = Util::StringUtil::trim(trimmed.substr(fromPos + 6));
        
        // 查找 as 关键字
        size_t asPos = pathPart.find(" as ");
        if (asPos != String::npos) {
            spec.path = Util::StringUtil::trim(pathPart.substr(0, asPos));
            spec.alias = Util::StringUtil::trim(pathPart.substr(asPos + 4));
        } else {
            spec.path = pathPart;
        }
        
        // 移除末尾的分号
        if (Util::StringUtil::endsWith(spec.path, ";")) {
            spec.path = spec.path.substr(0, spec.path.length() - 1);
        }
        if (Util::StringUtil::endsWith(spec.alias, ";")) {
            spec.alias = spec.alias.substr(0, spec.alias.length() - 1);
        }
        
        spec.path = Util::StringUtil::trim(spec.path);
        spec.alias = Util::StringUtil::trim(spec.alias);
        
        // 提取目标名称（精确导入）
        if (fromPos > 0) {
            String targetPart = Util::StringUtil::trim(trimmed.substr(0, fromPos));
            size_t lastSpace = targetPart.rfind(' ');
            if (lastSpace != String::npos) {
                spec.targetName = Util::StringUtil::trim(targetPart.substr(lastSpace + 1));
            }
        }
    } else {
        spec.isWildcard = true;
    }
    
    return spec;
}

String ModuleResolver::resolvePath(const String& basePath, const String& importPath) {
    namespace fs = std::filesystem;
    
    if (fs::path(importPath).is_absolute()) {
        return importPath;
    }
    
    fs::path base(basePath);
    fs::path relative(importPath);
    fs::path resolved = base.parent_path() / relative;
    
    return resolved.string();
}

bool ModuleResolver::isOfficialModule(const String& path) const {
    return Util::StringUtil::startsWith(path, officialPrefix_);
}

String ModuleResolver::extractModuleName(const String& path) {
    namespace fs = std::filesystem;
    
    String cleanPath = path;
    if (Util::StringUtil::startsWith(cleanPath, "chtl::")) {
        cleanPath = cleanPath.substr(6);
    }
    
    fs::path p(cleanPath);
    return p.stem().string();
}

} // namespace CMOD
} // namespace CHTL
