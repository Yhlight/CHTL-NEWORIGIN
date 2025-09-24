#pragma once

#include "StateMachine.h"
#include "Strategy.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <mutex>

namespace CHTL {

// Fragment types
enum class FragmentType {
    CHTL,
    CHTL_JS,
    CSS,
    JS,
    ORIGIN_HTML,
    ORIGIN_CSS,
    ORIGIN_JS
};

// Code fragment
struct CodeFragment {
    FragmentType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    std::string placeholderKey;
    std::map<std::string, std::string> metadata;
    
    CodeFragment(FragmentType t = FragmentType::CHTL, const std::string& c = "")
        : type(t), content(c), startLine(0), endLine(0), startColumn(0), endColumn(0) {}
};

// Placeholder registry
class PlaceholderRegistry {
public:
    static PlaceholderRegistry& getInstance();
    
    std::string generatePlaceholder(FragmentType type);
    std::string getPlaceholderPrefix(FragmentType type) const;
    FragmentType getTypeFromPlaceholder(const std::string& placeholder) const;
    bool isValidPlaceholder(const std::string& placeholder) const;
    
    void registerPlaceholder(const std::string& placeholder, const CodeFragment& fragment);
    CodeFragment getFragment(const std::string& placeholder) const;
    bool hasPlaceholder(const std::string& placeholder) const;
    
    void clear();
    size_t size() const { return placeholders_.size(); }
    
private:
    PlaceholderRegistry() = default;
    
    std::map<std::string, CodeFragment> placeholders_;
    std::map<FragmentType, std::string> typePrefixes_;
    std::atomic<size_t> counter_;
    mutable std::mutex mutex_;
    
    void initializePrefixes();
};

// Unified Scanner States
enum class ScannerState {
    INIT,
    DETECT_BLOCKS,
    REPLACE_PLACEHOLDERS,
    SLICE_CHTL_ALLOWED_IN_GLOBAL_STYLE,
    SLICE_LOCAL_SCRIPT_CHTL_CJ,
    FINALIZE,
    ERROR
};

// Unified Scanner Strategy Interface
class UnifiedScannerStrategy {
public:
    virtual ~UnifiedScannerStrategy() = default;
    virtual bool canHandle(const std::string& input) const = 0;
    virtual std::vector<CodeFragment> process(const std::string& input) = 0;
    virtual std::string getName() const = 0;
};

// Placeholder Strategy
class PlaceholderStrategy : public UnifiedScannerStrategy {
public:
    PlaceholderStrategy();
    ~PlaceholderStrategy() override = default;
    
    bool canHandle(const std::string& input) const override;
    std::vector<CodeFragment> process(const std::string& input) override;
    std::string getName() const override { return "PlaceholderStrategy"; }
    
    // Placeholder operations
    std::string replaceWithPlaceholder(const std::string& content, FragmentType type);
    std::string restoreFromPlaceholder(const std::string& content);
    
private:
    PlaceholderRegistry& registry_;
};

// Slice Strategy
class SliceStrategy : public UnifiedScannerStrategy {
public:
    SliceStrategy();
    ~SliceStrategy() override = default;
    
    bool canHandle(const std::string& input) const override;
    std::vector<CodeFragment> process(const std::string& input) override;
    std::string getName() const override { return "SliceStrategy"; }
    
    // Slice operations
    std::vector<CodeFragment> sliceGlobalStyleBlocks(const std::string& input);
    std::vector<CodeFragment> sliceLocalScriptBlocks(const std::string& input);
    std::vector<CodeFragment> sliceCHTLBlocks(const std::string& input);
    
private:
    std::vector<std::string> findBlockBoundaries(const std::string& input, const std::string& startPattern, const std::string& endPattern);
    bool isCHTLAllowedInGlobalStyle(const std::string& content);
    bool isCHTLAllowedInLocalScript(const std::string& content);
};

// Boundary Strategy
class BoundaryStrategy : public UnifiedScannerStrategy {
public:
    BoundaryStrategy();
    ~BoundaryStrategy() override = default;
    
    bool canHandle(const std::string& input) const override;
    std::vector<CodeFragment> process(const std::string& input) override;
    std::string getName() const override { return "BoundaryStrategy"; }
    
    // Boundary detection
    std::vector<std::pair<size_t, size_t>> findStyleBlocks(const std::string& input);
    std::vector<std::pair<size_t, size_t>> findScriptBlocks(const std::string& input);
    std::vector<std::pair<size_t, size_t>> findOriginBlocks(const std::string& input);
    
private:
    std::vector<std::pair<size_t, size_t>> findBlockPairs(const std::string& input, const std::string& startTag, const std::string& endTag);
    bool isValidBoundary(const std::string& input, size_t start, size_t end);
};

// Unified Scanner State Handlers
class ScannerInitHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerDetectBlocksHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerReplacePlaceholdersHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerSliceCHTLAllowedHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerSliceLocalScriptHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerFinalizeHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

class ScannerErrorHandler : public StateHandler<ScannerState> {
public:
    void onEnter(StateContext<ScannerState>& context) override;
    void onUpdate(StateContext<ScannerState>& context) override;
};

// Main Unified Scanner
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // Main processing method
    std::vector<CodeFragment> scan(const std::string& input);
    
    // Strategy management
    void registerStrategy(std::unique_ptr<UnifiedScannerStrategy> strategy);
    void setDefaultStrategy(const std::string& name);
    
    // Configuration
    void setEnablePlaceholders(bool enable) { enablePlaceholders_ = enable; }
    void setEnableSlicing(bool enable) { enableSlicing_ = enable; }
    void setEnableBoundaryDetection(bool enable) { enableBoundaryDetection_ = enable; }
    
    // Placeholder operations
    std::string generatePlaceholder(FragmentType type);
    CodeFragment getFragment(const std::string& placeholder);
    std::string restorePlaceholders(const std::string& content);
    
    // Error handling
    bool hasError() const;
    std::string getError() const;
    void clearError();
    
private:
    StateMachine<ScannerState> stateMachine_;
    std::map<std::string, std::unique_ptr<UnifiedScannerStrategy>> strategies_;
    
    // Configuration
    bool enablePlaceholders_;
    bool enableSlicing_;
    bool enableBoundaryDetection_;
    
    // Processing state
    std::string input_;
    std::vector<CodeFragment> fragments_;
    std::string error_;
    
    // Strategy instances
    std::unique_ptr<PlaceholderStrategy> placeholderStrategy_;
    std::unique_ptr<SliceStrategy> sliceStrategy_;
    std::unique_ptr<BoundaryStrategy> boundaryStrategy_;
    
    void initializeStrategies();
    void initializeStateMachine();
    bool processInput();
};

} // namespace CHTL