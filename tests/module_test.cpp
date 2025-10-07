#include "catch.hpp"
#include "../src/CHTL/CMODSystem/ModuleLoader.h"
#include "../src/CHTL/CMODSystem/ModulePacker.h"
#include "../src/CHTL-JS/CJMODSystem/CJMODApi.h"
#include <filesystem>

using namespace CHTL;
using namespace CHTL::CMOD;
using namespace CHTL::CJMOD;

TEST_CASE("CMOD System Tests", "[cmod][modules]") {
    
    SECTION("Module Info Parsing") {
        ModuleLoader loader;
        
        // 测试珂朵莉模块信息解析
        String infoPath = "./Module/CMOD/Chtholly/info/Chtholly.chtl";
        if (std::filesystem::exists(infoPath)) {
            auto info = loader.parseInfoFile(infoPath);
            REQUIRE(info.has_value());
            REQUIRE(info->name == "chtholly");
            REQUIRE(info->version == "1.0.0");
            REQUIRE(info->author == "CHTL Team");
            REQUIRE(info->license == "MIT");
        }
    }
    
    SECTION("Module Structure Validation") {
        ModulePacker packer;
        
        // 测试珂朵莉模块结构验证
        String modulePath = "./Module/CMOD/Chtholly";
        if (std::filesystem::exists(modulePath)) {
            bool isValid = packer.validateModuleStructure(modulePath);
            REQUIRE(isValid);
        }
        
        // 测试由比滨模块结构验证
        String yuiPath = "./Module/CMOD/Yuigahama";
        if (std::filesystem::exists(yuiPath)) {
            bool isValid = packer.validateModuleStructure(yuiPath);
            REQUIRE(isValid);
        }
    }
    
    SECTION("Module Loading") {
        ModuleLoader loader;
        loader.setOfficialModulePath("./Module");
        
        // 测试加载珂朵莉模块
        String chthollyPath = "./Module/CMOD/Chtholly";
        if (std::filesystem::exists(chthollyPath)) {
            auto moduleData = loader.loadModuleFromPath(chthollyPath);
            REQUIRE(moduleData.has_value());
            REQUIRE(moduleData->info.name == "chtholly");
        }
    }
    
    SECTION("Export Parsing") {
        ModuleLoader loader;
        
        String infoPath = "./Module/CMOD/Chtholly/info/Chtholly.chtl";
        if (std::filesystem::exists(infoPath)) {
            auto exports = loader.parseExportFile(infoPath);
            // Export解析测试
            // 注意：当前parseExportBlock还未完全实现
        }
    }
}

TEST_CASE("CJMOD API Tests", "[cjmod][api]") {
    
    SECTION("Syntax Analysis") {
        // 测试占位符解析
        Arg arg = Syntax::analyze("$ ** $");
        REQUIRE(arg.size() == 3);
        
        // 测试复杂模式
        Arg complex = Syntax::analyze("PrintMylove { url: $!_, mode: $?_ }");
        REQUIRE(complex.size() > 0);
    }
    
    SECTION("Type Detection") {
        REQUIRE(Syntax::isObject("{a: 1, b: 2}") == true);
        REQUIRE(Syntax::isFunction("function test() {}") == true);
        REQUIRE(Syntax::isArray("[1, 2, 3]") == true);
        REQUIRE(Syntax::isCHTLJSFunction("Listen { click: () => {} }") == true);
    }
    
    SECTION("Arg Binding and Matching") {
        Arg arg = Syntax::analyze("$ + $");
        
        // 绑定值获取函数
        arg.bind("$", [](const String& value) {
            return value;
        });
        
        // 填充值
        arg.fillValue({"3", "+", "4"});
        
        // 匹配并转换
        String left = arg.match("$", [](const String& v) { return v; });
        String right = arg.match("$", [](const String& v) { return v; });
        
        REQUIRE(left == "3");
        REQUIRE(right == "4");
    }
    
    SECTION("Transform and Export") {
        Arg arg = Syntax::analyze("$ ** $");
        arg.fillValue({"3", "**", "4"});
        
        // 转换
        arg.transform("Math.pow(" + arg[0].getValue() + ", " + arg[2].getValue() + ")");
        
        String result = CJMODGenerator::exportResult(arg);
        REQUIRE(result.find("Math.pow(3, 4)") != String::npos);
    }
    
    SECTION("Virtual Object Support") {
        // 测试虚对象绑定
        CHTLJSFunction::bindVirtualObject("PrintMylove");
        REQUIRE(CHTLJSFunction::hasVirtualObject("PrintMylove") == true);
    }
    
    SECTION("CJMOD Registry") {
        // 注册语法
        CJMODRegistry::SyntaxInfo info;
        info.pattern = "TestSyntax { key: $!_ }";
        info.description = "Test syntax for CJMOD";
        info.examples = {"TestSyntax { key: \"value\" }"};
        
        CJMODRegistry::registerSyntax("TestSyntax", info);
        
        auto retrievedInfo = CJMODRegistry::getSyntaxInfo("TestSyntax");
        REQUIRE(retrievedInfo.has_value());
        REQUIRE(retrievedInfo->pattern == "TestSyntax { key: $!_ }");
    }
}

TEST_CASE("Module Packer Tests", "[module][packer]") {
    
    SECTION("Collect Module Files") {
        ModulePacker packer;
        
        String modulePath = "./Module/CMOD/Chtholly";
        if (std::filesystem::exists(modulePath)) {
            // 测试文件收集
            // 注意：这个测试需要模块文件存在
        }
    }
    
    SECTION("Module Packing and Unpacking") {
        // 这个测试需要实际的模块文件
        // 暂时跳过，等模块完全创建后再测试
    }
    
    SECTION("JSON Manifest Generation") {
        ModulePacker packer;
        
        // 创建测试数据
        ModuleData testData;
        testData.info.name = "test-module";
        testData.info.version = "1.0.0";
        testData.info.description = "Test module";
        testData.info.author = "Test";
        testData.info.license = "MIT";
        testData.info.type = ModuleType::CMOD;
        
        String manifest = packer.generateJSONManifest(testData);
        
        REQUIRE(manifest.find("\"name\": \"test-module\"") != String::npos);
        REQUIRE(manifest.find("\"version\": \"1.0.0\"") != String::npos);
    }
}

TEST_CASE("Dependency Manager Tests", "[module][dependency]") {
    
    SECTION("Dependency Resolution") {
        DependencyManager depMgr;
        
        ModuleData module;
        module.info.dependencies = {"dep1", "dep2", "dep3"};
        
        auto resolved = depMgr.resolveDependencies(module);
        REQUIRE(resolved.size() == 3);
        REQUIRE(resolved[0] == "dep1");
    }
    
    SECTION("Circular Dependency Detection") {
        DependencyManager depMgr;
        
        // A -> B -> C -> A (循环)
        bool hasCycle = depMgr.hasCircularDependency("A", {"B"});
        // 需要先构建完整的依赖图才能正确检测
    }
    
    SECTION("Topological Sort") {
        DependencyManager depMgr;
        
        Vector<String> modules = {"A", "B", "C"};
        auto order = depMgr.getDependencyInstallOrder(modules);
        
        // 验证拓扑排序结果
        REQUIRE(order.size() == modules.size());
    }
}

TEST_CASE("Module Cache Tests", "[module][cache]") {
    
    SECTION("Cache Module Data") {
        ModuleCache cache;
        cache.setCacheDirectory("./.test_cache");
        
        ModuleData data;
        data.info.name = "test-cache";
        data.info.version = "1.0.0";
        
        cache.cacheModule("test-cache", data);
        
        auto loaded = cache.loadFromCache("test-cache");
        REQUIRE(loaded.has_value());
        REQUIRE(loaded->info.name == "test-cache");
        
        cache.clearCache();
    }
    
    SECTION("Cache Validation") {
        ModuleCache cache;
        
        ModuleData data;
        data.info.name = "test-valid";
        data.info.version = "1.0.0";
        
        cache.cacheModule("test-valid", data);
        
        REQUIRE(cache.isCacheValid("test-valid", "1.0.0") == true);
        REQUIRE(cache.isCacheValid("test-valid", "2.0.0") == false);
        
        cache.clearCache();
    }
}

TEST_CASE("Module Resolver Tests", "[module][resolver]") {
    
    SECTION("Official Module Detection") {
        ModuleResolver resolver;
        
        REQUIRE(resolver.isOfficialModule("chtl::Chtholly") == true);
        REQUIRE(resolver.isOfficialModule("custom-module") == false);
    }
    
    SECTION("Module Name Extraction") {
        String name1 = ModuleResolver::extractModuleName("chtl::Chtholly");
        REQUIRE(name1 == "Chtholly");
        
        String name2 = ModuleResolver::extractModuleName("/path/to/module.cmod");
        REQUIRE(name2 == "module");
    }
    
    SECTION("Path Resolution") {
        ModuleResolver resolver;
        
        String resolved = resolver.resolvePath("/base/path", "./relative/path");
        // 验证路径解析
    }
}
