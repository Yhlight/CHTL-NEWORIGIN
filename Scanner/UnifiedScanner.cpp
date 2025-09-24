#include "UnifiedScanner.h"
#include "../Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {

// PlaceholderRegistry implementation
PlaceholderRegistry& PlaceholderRegistry::getInstance() {
    static PlaceholderRegistry instance;
    return instance;
}

PlaceholderRegistry::PlaceholderRegistry() : counter_(0) {
    initializePrefixes();
}

void PlaceholderRegistry::initializePrefixes() {
    typePrefixes_[FragmentType::CHTL] = "CHTL_CODE_PLACEHOLDER_";
    typePrefixes_[FragmentType::CHTL_JS] = "CHTL_JS_CODE_PLACEHOLDER_";
    typePrefixes_[FragmentType::CSS] = "CSS_CODE_PLACEHOLDER_";
    typePrefixes_[FragmentType::JS] = "JS_CODE_PLACEHOLDER_";
    typePrefixes_[FragmentType::ORIGIN_HTML] = "ORIGIN_HTML_PLACEHOLDER_";
    typePrefixes_[FragmentType::ORIGIN_CSS] = "ORIGIN_CSS_PLACEHOLDER_";
    typePrefixes_[FragmentType::ORIGIN_JS] = "ORIGIN_JS_PLACEHOLDER_";
}

std::string PlaceholderRegistry::generatePlaceholder(FragmentType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = typePrefixes_.find(type);
    if (it == typePrefixes_.end()) {
        return "UNKNOWN_PLACEHOLDER_" + std::to_string(++counter_);
    }
    
    return it->second + std::to_string(++counter_);
}

std::string PlaceholderRegistry::getPlaceholderPrefix(FragmentType type) const {
    auto it = typePrefixes_.find(type);
    return (it != typePrefixes_.end()) ? it->second : "";
}

FragmentType PlaceholderRegistry::getTypeFromPlaceholder(const std::string& placeholder) const {
    for (const auto& [type, prefix] : typePrefixes_) {
        if (Util::StringUtil::startsWith(placeholder, prefix)) {
            return type;
        }
    }
    return FragmentType::CHTL;
}

bool PlaceholderRegistry::isValidPlaceholder(const std::string& placeholder) const {
    for (const auto& [type, prefix] : typePrefixes_) {
        if (Util::StringUtil::startsWith(placeholder, prefix)) {
            return true;
        }
    }
    return false;
}

void PlaceholderRegistry::registerPlaceholder(const std::string& placeholder, const CodeFragment& fragment) {
    std::lock_guard<std::mutex> lock(mutex_);
    placeholders_[placeholder] = fragment;
}

CodeFragment PlaceholderRegistry::getFragment(const std::string& placeholder) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = placeholders_.find(placeholder);
    return (it != placeholders_.end()) ? it->second : CodeFragment();
}

bool PlaceholderRegistry::hasPlaceholder(const std::string& placeholder) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return placeholders_.find(placeholder) != placeholders_.end();
}

void PlaceholderRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    placeholders_.clear();
    counter_ = 0;
}

// PlaceholderStrategy implementation
PlaceholderStrategy::PlaceholderStrategy() : registry_(PlaceholderRegistry::getInstance()) {
}

bool PlaceholderStrategy::canHandle(const std::string& input) const {
    // Can handle any input that contains JS/CSS code that needs placeholder replacement
    return Util::StringUtil::contains(input, "<script") || 
           Util::StringUtil::contains(input, "<style") ||
           Util::StringUtil::contains(input, "function") ||
           Util::StringUtil::contains(input, "{{") ||
           Util::StringUtil::contains(input, "{{&}}");
}

std::vector<CodeFragment> PlaceholderStrategy::process(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Process script blocks
    std::regex scriptRegex(R"(<script[^>]*>(.*?)</script>)", std::regex_constants::dotall);
    std::sregex_iterator scriptBegin(input.begin(), input.end(), scriptRegex);
    std::sregex_iterator scriptEnd;
    
    for (auto it = scriptBegin; it != scriptEnd; ++it) {
        std::string scriptContent = (*it)[1].str();
        
        // Check if it contains CHTL JS syntax
        if (Util::StringUtil::contains(scriptContent, "{{") || 
            Util::StringUtil::contains(scriptContent, "Listen") ||
            Util::StringUtil::contains(scriptContent, "Animate")) {
            
            // Replace JS parts with placeholders, keep CHTL JS parts
            std::string processedContent = replaceWithPlaceholder(scriptContent, FragmentType::JS);
            
            CodeFragment fragment(FragmentType::CHTL_JS, processedContent);
            fragment.placeholderKey = registry_.generatePlaceholder(FragmentType::CHTL_JS);
            registry_.registerPlaceholder(fragment.placeholderKey, fragment);
            
            fragments.push_back(fragment);
        } else {
            // Pure JS content
            CodeFragment fragment(FragmentType::JS, scriptContent);
            fragment.placeholderKey = registry_.generatePlaceholder(FragmentType::JS);
            registry_.registerPlaceholder(fragment.placeholderKey, fragment);
            
            fragments.push_back(fragment);
        }
    }
    
    // Process style blocks
    std::regex styleRegex(R"(<style[^>]*>(.*?)</style>)", std::regex_constants::dotall);
    std::sregex_iterator styleBegin(input.begin(), input.end(), styleRegex);
    std::sregex_iterator styleEnd;
    
    for (auto it = styleBegin; it != styleEnd; ++it) {
        std::string styleContent = (*it)[1].str();
        
        // Check if it contains CHTL syntax
        if (Util::StringUtil::contains(styleContent, "@Style") ||
            Util::StringUtil::contains(styleContent, "Theme(") ||
            Util::StringUtil::contains(styleContent, "+") ||
            Util::StringUtil::contains(styleContent, "?")) {
            
            // Replace CSS parts with placeholders, keep CHTL parts
            std::string processedContent = replaceWithPlaceholder(styleContent, FragmentType::CSS);
            
            CodeFragment fragment(FragmentType::CHTL, processedContent);
            fragment.placeholderKey = registry_.generatePlaceholder(FragmentType::CHTL);
            registry_.registerPlaceholder(fragment.placeholderKey, fragment);
            
            fragments.push_back(fragment);
        } else {
            // Pure CSS content
            CodeFragment fragment(FragmentType::CSS, styleContent);
            fragment.placeholderKey = registry_.generatePlaceholder(FragmentType::CSS);
            registry_.registerPlaceholder(fragment.placeholderKey, fragment);
            
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::string PlaceholderStrategy::replaceWithPlaceholder(const std::string& content, FragmentType type) {
    std::string result = content;
    std::string placeholder = registry_.generatePlaceholder(type);
    
    // Store the original content
    CodeFragment fragment(type, content);
    registry_.registerPlaceholder(placeholder, fragment);
    
    // Replace content with placeholder
    return placeholder;
}

std::string PlaceholderStrategy::restoreFromPlaceholder(const std::string& content) {
    std::string result = content;
    PlaceholderRegistry& reg = PlaceholderRegistry::getInstance();
    
    // Find all placeholders in content and restore them
    std::regex placeholderRegex(R"(([A-Z_]+_PLACEHOLDER_\d+))");
    std::sregex_iterator begin(result.begin(), result.end(), placeholderRegex);
    std::sregex_iterator end;
    
    std::vector<std::string> placeholders;
    for (auto it = begin; it != end; ++it) {
        placeholders.push_back((*it)[1].str());
    }
    
    for (const auto& placeholder : placeholders) {
        if (reg.hasPlaceholder(placeholder)) {
            CodeFragment fragment = reg.getFragment(placeholder);
            result = Util::StringUtil::replaceAll(result, placeholder, fragment.content);
        }
    }
    
    return result;
}

// SliceStrategy implementation
SliceStrategy::SliceStrategy() {
}

bool SliceStrategy::canHandle(const std::string& input) const {
    return Util::StringUtil::contains(input, "{") && Util::StringUtil::contains(input, "}");
}

std::vector<CodeFragment> SliceStrategy::process(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Slice global style blocks
    auto globalStyleFragments = sliceGlobalStyleBlocks(input);
    fragments.insert(fragments.end(), globalStyleFragments.begin(), globalStyleFragments.end());
    
    // Slice local script blocks
    auto localScriptFragments = sliceLocalScriptBlocks(input);
    fragments.insert(fragments.end(), localScriptFragments.begin(), localScriptFragments.end());
    
    // Slice CHTL blocks
    auto chtlFragments = sliceCHTLBlocks(input);
    fragments.insert(fragments.end(), chtlFragments.begin(), chtlFragments.end());
    
    return fragments;
}

std::vector<CodeFragment> SliceStrategy::sliceGlobalStyleBlocks(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Find global style blocks (outside of elements)
    std::regex globalStyleRegex(R"(\s*style\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\})", std::regex_constants::dotall);
    std::sregex_iterator begin(input.begin(), input.end(), globalStyleRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string styleContent = (*it)[1].str();
        
        if (isCHTLAllowedInGlobalStyle(styleContent)) {
            CodeFragment fragment(FragmentType::CHTL, styleContent);
            fragments.push_back(fragment);
        } else {
            CodeFragment fragment(FragmentType::CSS, styleContent);
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> SliceStrategy::sliceLocalScriptBlocks(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Find local script blocks (inside elements)
    std::regex localScriptRegex(R"(\s*script\s*\{([^{}]*(?:\{[^{}]*\}[^{}]*)*)\})", std::regex_constants::dotall);
    std::sregex_iterator begin(input.begin(), input.end(), localScriptRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        std::string scriptContent = (*it)[1].str();
        
        if (isCHTLAllowedInLocalScript(scriptContent)) {
            // Contains CHTL JS syntax
            CodeFragment fragment(FragmentType::CHTL_JS, scriptContent);
            fragments.push_back(fragment);
        } else {
            // Pure JS content
            CodeFragment fragment(FragmentType::JS, scriptContent);
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> SliceStrategy::sliceCHTLBlocks(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Find CHTL blocks (elements, templates, custom, etc.)
    std::vector<std::string> blockBoundaries = findBlockBoundaries(input, "\\{", "\\}");
    
    for (const auto& boundary : blockBoundaries) {
        CodeFragment fragment(FragmentType::CHTL, boundary);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<std::string> SliceStrategy::findBlockBoundaries(const std::string& input, const std::string& startPattern, const std::string& endPattern) {
    std::vector<std::string> boundaries;
    
    std::regex blockRegex(startPattern + R"([^{}]*(?:\{[^{}]*\}[^{}]*)*)" + endPattern, std::regex_constants::dotall);
    std::sregex_iterator begin(input.begin(), input.end(), blockRegex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        boundaries.push_back(it->str());
    }
    
    return boundaries;
}

bool SliceStrategy::isCHTLAllowedInGlobalStyle(const std::string& content) {
    // Check for CHTL syntax allowed in global style blocks
    return Util::StringUtil::contains(content, "@Style") ||
           Util::StringUtil::contains(content, "Theme(") ||
           Util::StringUtil::contains(content, "+") ||
           Util::StringUtil::contains(content, "-") ||
           Util::StringUtil::contains(content, "*") ||
           Util::StringUtil::contains(content, "/") ||
           Util::StringUtil::contains(content, "?") ||
           Util::StringUtil::contains(content, "&&") ||
           Util::StringUtil::contains(content, "||");
}

bool SliceStrategy::isCHTLAllowedInLocalScript(const std::string& content) {
    // Check for CHTL JS syntax allowed in local script blocks
    return Util::StringUtil::contains(content, "{{") ||
           Util::StringUtil::contains(content, "Listen") ||
           Util::StringUtil::contains(content, "Animate") ||
           Util::StringUtil::contains(content, "Router") ||
           Util::StringUtil::contains(content, "Vir") ||
           Util::StringUtil::contains(content, "->") ||
           Util::StringUtil::contains(content, "&->");
}

// BoundaryStrategy implementation
BoundaryStrategy::BoundaryStrategy() {
}

bool BoundaryStrategy::canHandle(const std::string& input) const {
    return Util::StringUtil::contains(input, "<") && Util::StringUtil::contains(input, ">");
}

std::vector<CodeFragment> BoundaryStrategy::process(const std::string& input) {
    std::vector<CodeFragment> fragments;
    
    // Find style blocks
    auto styleBoundaries = findStyleBlocks(input);
    for (const auto& [start, end] : styleBoundaries) {
        std::string content = input.substr(start, end - start);
        CodeFragment fragment(FragmentType::CSS, content);
        fragments.push_back(fragment);
    }
    
    // Find script blocks
    auto scriptBoundaries = findScriptBlocks(input);
    for (const auto& [start, end] : scriptBoundaries) {
        std::string content = input.substr(start, end - start);
        CodeFragment fragment(FragmentType::JS, content);
        fragments.push_back(fragment);
    }
    
    // Find origin blocks
    auto originBoundaries = findOriginBlocks(input);
    for (const auto& [start, end] : originBoundaries) {
        std::string content = input.substr(start, end - start);
        CodeFragment fragment(FragmentType::CHTL, content);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<std::pair<size_t, size_t>> BoundaryStrategy::findStyleBlocks(const std::string& input) {
    return findBlockPairs(input, "<style", "</style>");
}

std::vector<std::pair<size_t, size_t>> BoundaryStrategy::findScriptBlocks(const std::string& input) {
    return findBlockPairs(input, "<script", "</script>");
}

std::vector<std::pair<size_t, size_t>> BoundaryStrategy::findOriginBlocks(const std::string& input) {
    return findBlockPairs(input, "[Origin]", "}");
}

std::vector<std::pair<size_t, size_t>> BoundaryStrategy::findBlockPairs(const std::string& input, const std::string& startTag, const std::string& endTag) {
    std::vector<std::pair<size_t, size_t>> boundaries;
    
    size_t start = 0;
    while (true) {
        size_t startPos = input.find(startTag, start);
        if (startPos == std::string::npos) break;
        
        size_t endPos = input.find(endTag, startPos + startTag.length());
        if (endPos == std::string::npos) break;
        
        endPos += endTag.length();
        
        if (isValidBoundary(input, startPos, endPos)) {
            boundaries.emplace_back(startPos, endPos);
        }
        
        start = endPos;
    }
    
    return boundaries;
}

bool BoundaryStrategy::isValidBoundary(const std::string& input, size_t start, size_t end) {
    // Basic validation - ensure the boundary doesn't cross other boundaries
    if (start >= end) return false;
    
    // Check for nested boundaries
    std::string content = input.substr(start, end - start);
    int depth = 0;
    
    for (char c : content) {
        if (c == '{') depth++;
        else if (c == '}') depth--;
        
        if (depth < 0) return false; // Unmatched closing brace
    }
    
    return depth == 0; // All braces should be matched
}

// UnifiedScanner implementation
UnifiedScanner::UnifiedScanner() 
    : stateMachine_(ScannerState::INIT)
    , enablePlaceholders_(true)
    , enableSlicing_(true)
    , enableBoundaryDetection_(true) {
    
    initializeStrategies();
    initializeStateMachine();
}

void UnifiedScanner::initializeStrategies() {
    placeholderStrategy_ = std::make_unique<PlaceholderStrategy>();
    sliceStrategy_ = std::make_unique<SliceStrategy>();
    boundaryStrategy_ = std::make_unique<BoundaryStrategy>();
    
    registerStrategy(std::make_unique<PlaceholderStrategy>());
    registerStrategy(std::make_unique<SliceStrategy>());
    registerStrategy(std::make_unique<BoundaryStrategy>());
}

void UnifiedScanner::initializeStateMachine() {
    // Add state handlers
    stateMachine_.setHandler(ScannerState::INIT, std::make_unique<ScannerInitHandler>());
    stateMachine_.setHandler(ScannerState::DETECT_BLOCKS, std::make_unique<ScannerDetectBlocksHandler>());
    stateMachine_.setHandler(ScannerState::REPLACE_PLACEHOLDERS, std::make_unique<ScannerReplacePlaceholdersHandler>());
    stateMachine_.setHandler(ScannerState::SLICE_CHTL_ALLOWED_IN_GLOBAL_STYLE, std::make_unique<ScannerSliceCHTLAllowedHandler>());
    stateMachine_.setHandler(ScannerState::SLICE_LOCAL_SCRIPT_CHTL_CJ, std::make_unique<ScannerSliceLocalScriptHandler>());
    stateMachine_.setHandler(ScannerState::FINALIZE, std::make_unique<ScannerFinalizeHandler>());
    stateMachine_.setHandler(ScannerState::ERROR, std::make_unique<ScannerErrorHandler>());
    
    // Add transitions
    stateMachine_.addTransition(ScannerState::INIT, ScannerState::DETECT_BLOCKS, "input_loaded");
    stateMachine_.addTransition(ScannerState::DETECT_BLOCKS, ScannerState::REPLACE_PLACEHOLDERS, "blocks_detected");
    stateMachine_.addTransition(ScannerState::REPLACE_PLACEHOLDERS, ScannerState::SLICE_CHTL_ALLOWED_IN_GLOBAL_STYLE, "placeholders_replaced");
    stateMachine_.addTransition(ScannerState::SLICE_CHTL_ALLOWED_IN_GLOBAL_STYLE, ScannerState::SLICE_LOCAL_SCRIPT_CHTL_CJ, "global_style_sliced");
    stateMachine_.addTransition(ScannerState::SLICE_LOCAL_SCRIPT_CHTL_CJ, ScannerState::FINALIZE, "local_script_sliced");
    stateMachine_.addTransition(ScannerState::FINALIZE, ScannerState::INIT, "completed");
    
    // Error transitions
    stateMachine_.addTransition(ScannerState::INIT, ScannerState::ERROR, "error");
    stateMachine_.addTransition(ScannerState::DETECT_BLOCKS, ScannerState::ERROR, "error");
    stateMachine_.addTransition(ScannerState::REPLACE_PLACEHOLDERS, ScannerState::ERROR, "error");
    stateMachine_.addTransition(ScannerState::SLICE_CHTL_ALLOWED_IN_GLOBAL_STYLE, ScannerState::ERROR, "error");
    stateMachine_.addTransition(ScannerState::SLICE_LOCAL_SCRIPT_CHTL_CJ, ScannerState::ERROR, "error");
    stateMachine_.addTransition(ScannerState::FINALIZE, ScannerState::ERROR, "error");
}

std::vector<CodeFragment> UnifiedScanner::scan(const std::string& input) {
    input_ = input;
    fragments_.clear();
    clearError();
    
    if (!processInput()) {
        return {};
    }
    
    return fragments_;
}

void UnifiedScanner::registerStrategy(std::unique_ptr<UnifiedScannerStrategy> strategy) {
    if (strategy) {
        strategies_[strategy->getName()] = std::move(strategy);
    }
}

void UnifiedScanner::setDefaultStrategy(const std::string& name) {
    // Implementation for setting default strategy
}

std::string UnifiedScanner::generatePlaceholder(FragmentType type) {
    return PlaceholderRegistry::getInstance().generatePlaceholder(type);
}

CodeFragment UnifiedScanner::getFragment(const std::string& placeholder) {
    return PlaceholderRegistry::getInstance().getFragment(placeholder);
}

std::string UnifiedScanner::restorePlaceholders(const std::string& content) {
    return placeholderStrategy_->restoreFromPlaceholder(content);
}

bool UnifiedScanner::hasError() const {
    return !error_.empty();
}

std::string UnifiedScanner::getError() const {
    return error_;
}

void UnifiedScanner::clearError() {
    error_.clear();
}

bool UnifiedScanner::processInput() {
    try {
        // Start the state machine
        stateMachine_.trigger("input_loaded");
        
        // Process through all strategies
        if (enablePlaceholders_) {
            auto placeholderFragments = placeholderStrategy_->process(input_);
            fragments_.insert(fragments_.end(), placeholderFragments.begin(), placeholderFragments.end());
        }
        
        if (enableSlicing_) {
            auto sliceFragments = sliceStrategy_->process(input_);
            fragments_.insert(fragments_.end(), sliceFragments.begin(), sliceFragments.end());
        }
        
        if (enableBoundaryDetection_) {
            auto boundaryFragments = boundaryStrategy_->process(input_);
            fragments_.insert(fragments_.end(), boundaryFragments.begin(), boundaryFragments.end());
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = e.what();
        return false;
    }
}

// State Handler implementations
void ScannerInitHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "initialized");
}

void ScannerInitHandler::onUpdate(StateContext<ScannerState>& context) {
    // Initialize processing
}

void ScannerDetectBlocksHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "detecting_blocks");
}

void ScannerDetectBlocksHandler::onUpdate(StateContext<ScannerState>& context) {
    // Detect code blocks
}

void ScannerReplacePlaceholdersHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "replacing_placeholders");
}

void ScannerReplacePlaceholdersHandler::onUpdate(StateContext<ScannerState>& context) {
    // Replace placeholders
}

void ScannerSliceCHTLAllowedHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "slicing_chtl_global_style");
}

void ScannerSliceCHTLAllowedHandler::onUpdate(StateContext<ScannerState>& context) {
    // Slice CHTL allowed in global style
}

void ScannerSliceLocalScriptHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "slicing_local_script");
}

void ScannerSliceLocalScriptHandler::onUpdate(StateContext<ScannerState>& context) {
    // Slice local script blocks
}

void ScannerFinalizeHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "finalizing");
}

void ScannerFinalizeHandler::onUpdate(StateContext<ScannerState>& context) {
    // Finalize processing
}

void ScannerErrorHandler::onEnter(StateContext<ScannerState>& context) {
    context.setData("status", "error");
}

void ScannerErrorHandler::onUpdate(StateContext<ScannerState>& context) {
    // Handle error state
}

} // namespace CHTL