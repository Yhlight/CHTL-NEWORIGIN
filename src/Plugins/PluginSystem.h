#ifndef CHTL_PLUGIN_SYSTEM_H
#define CHTL_PLUGIN_SYSTEM_H

#include "../Common.h"
#include <functional>

namespace CHTL {
namespace Plugins {

enum class PluginStage {
    PreParse,
    PostParse,
    PreGenerate,
    PostGenerate,
    PostCompile
};

using PluginCallback = std::function<String(const String&)>;

struct Plugin {
    String name;
    String version;
    PluginStage stage;
    PluginCallback callback;
    
    Plugin(const String& n, const String& v, PluginStage s, PluginCallback cb)
        : name(n), version(v), stage(s), callback(cb) {}
};

class PluginSystem {
public:
    static PluginSystem& getInstance() {
        static PluginSystem instance;
        return instance;
    }
    
    void registerPlugin(const Plugin& plugin) {
        plugins_[plugin.stage].push_back(plugin);
    }
    
    String runPlugins(PluginStage stage, const String& input) {
        String result = input;
        
        auto it = plugins_.find(stage);
        if (it != plugins_.end()) {
            for (const auto& plugin : it->second) {
                result = plugin.callback(result);
            }
        }
        
        return result;
    }
    
    void clearPlugins() {
        plugins_.clear();
    }
    
    Vector<Plugin> getPlugins(PluginStage stage) const {
        auto it = plugins_.find(stage);
        if (it != plugins_.end()) {
            return it->second;
        }
        return {};
    }
    
private:
    PluginSystem() = default;
    HashMap<PluginStage, Vector<Plugin>> plugins_;
};

// Built-in plugins

class MinifyPlugin {
public:
    static Plugin create() {
        return Plugin("minify", "1.0.0", PluginStage::PostGenerate,
            [](const String& code) -> String {
                // Simple minification
                String result = code;
                // Remove extra whitespace
                size_t pos = 0;
                while ((pos = result.find("  ", pos)) != String::npos) {
                    result.replace(pos, 2, " ");
                }
                // Remove newlines
                pos = 0;
                while ((pos = result.find("\n ", pos)) != String::npos) {
                    result.replace(pos, 2, " ");
                }
                return result;
            });
    }
};

class PrettifyPlugin {
public:
    static Plugin create() {
        return Plugin("prettify", "1.0.0", PluginStage::PostGenerate,
            [](const String& code) -> String {
                // Simple prettification
                String result;
                int indent = 0;
                bool inTag = false;
                
                for (size_t i = 0; i < code.length(); i++) {
                    char c = code[i];
                    
                    if (c == '<') {
                        if (i + 1 < code.length() && code[i + 1] != '/') {
                            result += String(indent * 2, ' ');
                            indent++;
                        } else if (i + 1 < code.length() && code[i + 1] == '/') {
                            indent--;
                            result += String(indent * 2, ' ');
                        }
                        inTag = true;
                    }
                    
                    result += c;
                    
                    if (c == '>') {
                        inTag = false;
                        result += '\n';
                    }
                }
                
                return result;
            });
    }
};

} // namespace Plugins
} // namespace CHTL

#endif // CHTL_PLUGIN_SYSTEM_H
