#include <iostream>
#include "src/CHTL/CMODSystem/ModuleLoader.h"
#include "src/CHTL/CMODSystem/ModuleInfo.h"
#include "src/CHTL/CMODSystem/ModulePacker.h"

using namespace CHTL;
using namespace CHTL::CMOD;

int main() {
    std::cout << "=== CHTL v2.5.0 Module System Test ===" << std::endl << std::endl;
    
    // 创建模块加载器
    ModuleLoader loader;
    loader.setOfficialModulePath("./Module");
    
    // 测试1: 加载珂朵莉模块
    std::cout << "Test 1: Loading Chtholly module..." << std::endl;
    String chthollyPath = "./Module/CMOD/Chtholly";
    auto chthollyData = loader.loadModuleFromPath(chthollyPath);
    
    if (chthollyData.has_value()) {
        std::cout << "  ✅ Chtholly module loaded successfully!" << std::endl;
        std::cout << "  - Name: " << chthollyData->info.name << std::endl;
        std::cout << "  - Version: " << chthollyData->info.version << std::endl;
        std::cout << "  - Description: " << chthollyData->info.description << std::endl;
        std::cout << "  - Author: " << chthollyData->info.author << std::endl;
        std::cout << "  - Type: " << (chthollyData->info.type == ModuleType::Hybrid ? "Hybrid (CMOD + CJMOD)" : "CMOD") << std::endl;
        
        // 测试导出表
        std::cout << "  - Exports:" << std::endl;
        auto customStyles = chthollyData->exports.getExportsByType(ExportEntry::Type::CustomStyle);
        if (!customStyles.empty()) {
            std::cout << "    - Custom Styles: " << customStyles.size() << " items" << std::endl;
            for (const auto& style : customStyles) {
                std::cout << "      * " << style << std::endl;
            }
        }
        
        auto customElements = chthollyData->exports.getExportsByType(ExportEntry::Type::CustomElement);
        if (!customElements.empty()) {
            std::cout << "    - Custom Elements: " << customElements.size() << " items" << std::endl;
            for (const auto& elem : customElements) {
                std::cout << "      * " << elem << std::endl;
            }
        }
        
        auto customVars = chthollyData->exports.getExportsByType(ExportEntry::Type::CustomVar);
        if (!customVars.empty()) {
            std::cout << "    - Custom Vars: " << customVars.size() << " items" << std::endl;
            for (const auto& var : customVars) {
                std::cout << "      * " << var << std::endl;
            }
        }
    } else {
        std::cout << "  ❌ Failed to load Chtholly module" << std::endl;
    }
    std::cout << std::endl;
    
    // 测试2: 加载由比滨结衣模块
    std::cout << "Test 2: Loading Yuigahama module..." << std::endl;
    String yuigahamaPath = "./Module/CMOD/Yuigahama";
    auto yuigahamaData = loader.loadModuleFromPath(yuigahamaPath);
    
    if (yuigahamaData.has_value()) {
        std::cout << "  ✅ Yuigahama module loaded successfully!" << std::endl;
        std::cout << "  - Name: " << yuigahamaData->info.name << std::endl;
        std::cout << "  - Version: " << yuigahamaData->info.version << std::endl;
        std::cout << "  - Description: " << yuigahamaData->info.description << std::endl;
        std::cout << "  - Author: " << yuigahamaData->info.author << std::endl;
        
        // 测试导出表
        std::cout << "  - Exports:" << std::endl;
        auto customStyles = yuigahamaData->exports.getExportsByType(ExportEntry::Type::CustomStyle);
        if (!customStyles.empty()) {
            std::cout << "    - Custom Styles: " << customStyles.size() << " items" << std::endl;
        }
        
        auto customElements = yuigahamaData->exports.getExportsByType(ExportEntry::Type::CustomElement);
        if (!customElements.empty()) {
            std::cout << "    - Custom Elements: " << customElements.size() << " items" << std::endl;
        }
    } else {
        std::cout << "  ❌ Failed to load Yuigahama module" << std::endl;
    }
    std::cout << std::endl;
    
    // 测试3: 测试导入语句解析
    std::cout << "Test 3: Testing import statement parsing..." << std::endl;
    ModuleResolver resolver;
    
    String importStmt1 = "[Import] @Chtl from chtl::Chtholly";
    auto spec1 = resolver.parseImport(importStmt1);
    
    if (spec1.has_value()) {
        std::cout << "  ✅ Import statement parsed successfully!" << std::endl;
        std::cout << "  - Type: @Chtl" << std::endl;
        std::cout << "  - Path: " << spec1->path << std::endl;
        std::cout << "  - Official module: " << (resolver.isOfficialModule(spec1->path) ? "Yes" : "No") << std::endl;
    }
    std::cout << std::endl;
    
    // 测试4: 测试模块搜索
    std::cout << "Test 4: Testing module search..." << std::endl;
    auto foundPath = loader.loadModule("Chtholly");
    if (foundPath.has_value()) {
        std::cout << "  ✅ Module 'Chtholly' found!" << std::endl;
    } else {
        std::cout << "  ℹ️  Module 'Chtholly' not found in search paths" << std::endl;
    }
    std::cout << std::endl;
    
    // 测试5: 测试JSON清单生成
    std::cout << "Test 5: Testing JSON manifest generation..." << std::endl;
    if (chthollyData.has_value()) {
        ModulePacker packer;
        String manifest = packer.generateJSONManifest(chthollyData.value());
        std::cout << "  ✅ JSON manifest generated:" << std::endl;
        std::cout << manifest << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "=== All tests completed! ===" << std::endl;
    
    return 0;
}
