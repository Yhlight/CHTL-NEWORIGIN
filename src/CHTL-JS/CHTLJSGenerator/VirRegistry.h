#ifndef VIR_REGISTRY_H
#define VIR_REGISTRY_H

#include "../../Common.h"
#include <mutex>

namespace CHTL {
namespace JS {

// Vir 虚对象信息
struct VirObjectInfo {
    String name;
    String type;  // Listen, Delegate, Animate, Router, Custom
    String definition;
    Vector<String> virtualKeys;
    bool hasVirtualObject;
    
    VirObjectInfo() : hasVirtualObject(false) {}
    
    VirObjectInfo(const String& n, const String& t, const String& def)
        : name(n), type(t), definition(def), hasVirtualObject(false) {}
};

// Vir 全局注册表（单例）
class VirRegistry {
public:
    static VirRegistry& getInstance();
    
    // 注册 Vir 对象
    void registerVir(const String& name, const VirObjectInfo& info);
    
    // 查询 Vir 对象
    bool hasVir(const String& name) const;
    Optional<VirObjectInfo> getVir(const String& name) const;
    
    // 获取所有 Vir 对象
    Vector<String> getAllVirNames() const;
    Vector<VirObjectInfo> getAllVirs() const;
    
    // 清空注册表
    void clear();
    
    // 生成 JSON 清单
    String generateJSONManifest() const;
    
private:
    VirRegistry() = default;
    ~VirRegistry() = default;
    VirRegistry(const VirRegistry&) = delete;
    VirRegistry& operator=(const VirRegistry&) = delete;
    
    mutable std::mutex mutex_;
    HashMap<String, VirObjectInfo> registry_;
};

} // namespace JS
} // namespace CHTL

#endif // VIR_REGISTRY_H
