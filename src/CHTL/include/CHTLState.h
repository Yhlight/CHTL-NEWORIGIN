#pragma once

#include <memory>
#include <string>

namespace CHTL {

// Forward declarations
class CHTLContext;
class CHTLNode;

// Base state interface following State pattern
class CHTLState {
public:
    virtual ~CHTLState() = default;
    
    // State transitions
    virtual std::unique_ptr<CHTLState> handleElement(CHTLContext& context, CHTLNode* node) = 0;
    virtual std::unique_ptr<CHTLState> handleTemplate(CHTLContext& context, CHTLNode* node) = 0;
    virtual std::unique_ptr<CHTLState> handleCustom(CHTLContext& context, CHTLNode* node) = 0;
    virtual std::unique_ptr<CHTLState> handleImport(CHTLContext& context, CHTLNode* node) = 0;
    virtual std::unique_ptr<CHTLState> handleNamespace(CHTLContext& context, CHTLNode* node) = 0;
    virtual std::unique_ptr<CHTLState> handleConfiguration(CHTLContext& context, CHTLNode* node) = 0;
    
    // State identification
    virtual std::string getStateName() const = 0;
    
protected:
    // Common state transition logic
    void logStateTransition(const std::string& from, const std::string& to);
};

// Concrete state implementations
class InitialState : public CHTLState {
public:
    std::unique_ptr<CHTLState> handleElement(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleTemplate(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleCustom(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleImport(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleNamespace(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleConfiguration(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStateName() const override { return "InitialState"; }
};

class ParsingState : public CHTLState {
public:
    std::unique_ptr<CHTLState> handleElement(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleTemplate(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleCustom(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleImport(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleNamespace(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleConfiguration(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStateName() const override { return "ParsingState"; }
};

class TemplateState : public CHTLState {
public:
    std::unique_ptr<CHTLState> handleElement(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleTemplate(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleCustom(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleImport(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleNamespace(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleConfiguration(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStateName() const override { return "TemplateState"; }
};

class ErrorState : public CHTLState {
public:
    std::unique_ptr<CHTLState> handleElement(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleTemplate(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleCustom(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleImport(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleNamespace(CHTLContext& context, CHTLNode* node) override;
    std::unique_ptr<CHTLState> handleConfiguration(CHTLContext& context, CHTLNode* node) override;
    
    std::string getStateName() const override { return "ErrorState"; }
};

} // namespace CHTL