#ifndef CHTL_CONFIG_LOADER_H
#define CHTL_CONFIG_LOADER_H

#include "../../Common.h"

namespace CHTL {

// 配置加载器
// 从.chtlrc文件加载配置
class ConfigLoader {
public:
    ConfigLoader();
    
    // 从文件加载配置
    bool loadFromFile(const String& filePath);
    
    // 获取配置值
    String get(const String& key, const String& defaultValue = "") const;
    
    // 设置配置值
    void set(const String& key, const String& value);
    
    // 检查配置是否存在
    bool has(const String& key) const;
    
    // 获取所有配置
    const HashMap<String, String>& getAll() const { return config_; }
    
private:
    HashMap<String, String> config_;
};

} // namespace CHTL

#endif // CHTL_CONFIG_LOADER_H
