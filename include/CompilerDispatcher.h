#pragma once

#include "CHTLCommon.h"
#include "UnifiedScanner.h"
#include "CodeMerger.h"
#include <memory>
#include <chrono>

namespace CHTL {

// Compiler options
struct CompilerOptions {
    std::string outputFile;
    bool defaultStruct = false;
    bool inlineOutput = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool debugMode = false;
    
    CompilerOptions() = default;
};

// Compiler dispatcher - coordinates all compilation phases
class CompilerDispatcher {
private:
    CompilerOptions options;
    std::unique_ptr<UnifiedScanner> scanner;
    std::unique_ptr<CodeMerger> merger;
    
    // Compilation statistics
    size_t totalFilesProcessed;
    size_t totalErrors;
    size_t totalWarnings;
    std::chrono::steady_clock::time_point startTime;
    
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    // Configuration
    void setOptions(const CompilerOptions& opts) { options = opts; }
    const CompilerOptions& getOptions() const { return options; }
    
    // Main compilation methods
    void compileFile(const std::string& filepath);
    void compileString(const std::string& source, const std::string& filename = "");
    
    // Compilation phases
    void phase1_Scanning(const std::string& source, const std::string& filename);
    void phase2_Parsing();
    void phase3_CodeGeneration();
    void phase4_Merging();
    void phase5_Output();
    
    // Statistics
    size_t getFilesProcessed() const { return totalFilesProcessed; }
    size_t getErrors() const { return totalErrors; }
    size_t getWarnings() const { return totalWarnings; }
    double getCompilationTime() const;
    
    // Error handling
    void reportError(const std::string& message, const std::string& filename = "", int line = -1, int col = -1);
    void reportWarning(const std::string& message, const std::string& filename = "", int line = -1, int col = -1);
    
    // Debugging
    void enableDebugMode(bool enable = true);
    bool isDebugMode() const { return options.debugMode; }
    void printDebugInfo();
    
private:
    // Internal compilation helpers
    void initializeCompilation();
    void finalizeCompilation();
    void validateOutput();
    
    // File handling
    std::string readInputFile(const std::string& filepath);
    void writeOutputFile(const std::string& content);
    std::string generateOutputFilename(const std::string& inputFile);
    
    // Module handling
    void processImports();
    void resolveDependencies();
    
    // Error reporting
    void printCompilationSummary();
    void printErrorSummary();
};

} // namespace CHTL