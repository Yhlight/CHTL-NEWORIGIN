#include "CompilerDispatcher.h"
#include "CHTLCommon.h"
#include "UnifiedScanner.h"
#include "CodeMerger.h"
#include <iostream>
#include <fstream>
#include <chrono>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher()
    : totalFilesProcessed(0), totalErrors(0), totalWarnings(0) {
    scanner = std::make_unique<UnifiedScanner>("", "");
    merger = std::make_unique<CodeMerger>();
}

void CompilerDispatcher::compileFile(const std::string& filepath) {
    try {
        startTime = std::chrono::steady_clock::now();
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Starting compilation of: " << filepath << std::endl;
        }
        
        // Read input file
        std::string source = readInputFile(filepath);
        
        // Initialize compilation
        initializeCompilation();
        
        // Phase 1: Scanning
        phase1_Scanning(source, filepath);
        
        // Phase 2: Parsing
        phase2_Parsing();
        
        // Phase 3: Code Generation
        phase3_CodeGeneration();
        
        // Phase 4: Merging
        phase4_Merging();
        
        // Phase 5: Output
        phase5_Output();
        
        // Finalize compilation
        finalizeCompilation();
        
        totalFilesProcessed++;
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Compilation completed successfully" << std::endl;
        }
        
    } catch (const CHTLException& e) {
        reportError(e.what(), filepath, e.getLine(), e.getColumn());
        throw;
    } catch (const std::exception& e) {
        reportError(e.what(), filepath);
        throw;
    }
}

void CompilerDispatcher::compileString(const std::string& source, const std::string& filename) {
    try {
        startTime = std::chrono::steady_clock::now();
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Starting compilation of string source" << std::endl;
        }
        
        // Initialize compilation
        initializeCompilation();
        
        // Phase 1: Scanning
        phase1_Scanning(source, filename);
        
        // Phase 2: Parsing
        phase2_Parsing();
        
        // Phase 3: Code Generation
        phase3_CodeGeneration();
        
        // Phase 4: Merging
        phase4_Merging();
        
        // Phase 5: Output
        phase5_Output();
        
        // Finalize compilation
        finalizeCompilation();
        
        totalFilesProcessed++;
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] String compilation completed successfully" << std::endl;
        }
        
    } catch (const CHTLException& e) {
        reportError(e.what(), filename, e.getLine(), e.getColumn());
        throw;
    } catch (const std::exception& e) {
        reportError(e.what(), filename);
        throw;
    }
}

void CompilerDispatcher::phase1_Scanning(const std::string& source, const std::string& filename) {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Phase 1: Scanning" << std::endl;
    }
    
    try {
        // Set source for scanner
        scanner->setSource(source, filename);
        
        // Perform unified scanning
        auto fragments = scanner->scan();
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Found " << fragments.size() << " code fragments" << std::endl;
            for (const auto& fragment : fragments) {
                std::cout << "[DEBUG]   - " << fragment.type << " (" << fragment.content.length() << " chars)" << std::endl;
            }
        }
        
        // Validate fragments
        for (const auto& fragment : fragments) {
            if (fragment.type == FragmentType::UNKNOWN) {
                reportWarning("Unknown fragment type detected", filename, fragment.line, fragment.column);
            }
        }
        
    } catch (const std::exception& e) {
        reportError("Scanning phase failed: " + std::string(e.what()), filename);
        throw;
    }
}

void CompilerDispatcher::phase2_Parsing() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Phase 2: Parsing" << std::endl;
    }
    
    try {
        // TODO: Implement parsing phase
        // This would involve:
        // 1. CHTL Parser for CHTL fragments
        // 2. CHTL JS Parser for CHTL JS fragments
        // 3. CSS Parser for CSS fragments
        // 4. JS Parser for JS fragments
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Parsing phase completed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        reportError("Parsing phase failed: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::phase3_CodeGeneration() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Phase 3: Code Generation" << std::endl;
    }
    
    try {
        // TODO: Implement code generation phase
        // This would involve:
        // 1. CHTL Generator for HTML output
        // 2. CHTL JS Generator for JavaScript output
        // 3. CSS Generator for CSS output
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Code generation phase completed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        reportError("Code generation phase failed: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::phase4_Merging() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Phase 4: Merging" << std::endl;
    }
    
    try {
        // TODO: Implement code merging phase
        // This would involve:
        // 1. Merging HTML, CSS, and JS outputs
        // 2. Resolving placeholders
        // 3. Applying inline options
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Merging phase completed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        reportError("Merging phase failed: " + std::string(e.what()));
        throw;
    }
}

void CompilerDispatcher::phase5_Output() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Phase 5: Output" << std::endl;
    }
    
    try {
        // TODO: Implement output phase
        // This would involve:
        // 1. Writing final output file
        // 2. Applying formatting options
        // 3. Generating separate CSS/JS files if needed
        
        if (isDebugMode()) {
            std::cout << "[DEBUG] Output phase completed" << std::endl;
        }
        
    } catch (const std::exception& e) {
        reportError("Output phase failed: " + std::string(e.what()));
        throw;
    }
}

double CompilerDispatcher::getCompilationTime() const {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    return duration.count() / 1000.0;
}

void CompilerDispatcher::reportError(const std::string& message, const std::string& filename, int line, int col) {
    totalErrors++;
    
    std::cerr << "Error";
    if (!filename.empty()) {
        std::cerr << " in " << filename;
    }
    if (line > 0) {
        std::cerr << " at line " << line;
        if (col > 0) {
            std::cerr << ", column " << col;
        }
    }
    std::cerr << ": " << message << std::endl;
}

void CompilerDispatcher::reportWarning(const std::string& message, const std::string& filename, int line, int col) {
    totalWarnings++;
    
    std::cerr << "Warning";
    if (!filename.empty()) {
        std::cerr << " in " << filename;
    }
    if (line > 0) {
        std::cerr << " at line " << line;
        if (col > 0) {
            std::cerr << ", column " << col;
        }
    }
    std::cerr << ": " << message << std::endl;
}

void CompilerDispatcher::enableDebugMode(bool enable) {
    options.debugMode = enable;
}

void CompilerDispatcher::printDebugInfo() {
    if (!isDebugMode()) return;
    
    std::cout << "\n=== DEBUG INFO ===" << std::endl;
    std::cout << "Files processed: " << totalFilesProcessed << std::endl;
    std::cout << "Errors: " << totalErrors << std::endl;
    std::cout << "Warnings: " << totalWarnings << std::endl;
    std::cout << "Compilation time: " << getCompilationTime() << " seconds" << std::endl;
    
    const auto& fragments = scanner->getFragments();
    std::cout << "Code fragments: " << fragments.size() << std::endl;
    
    for (const auto& fragment : fragments) {
        std::cout << "  - Type: " << static_cast<int>(fragment.type) 
                  << ", Size: " << fragment.content.length() 
                  << ", Line: " << fragment.line << std::endl;
    }
    
    std::cout << "==================\n" << std::endl;
}

void CompilerDispatcher::initializeCompilation() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Initializing compilation" << std::endl;
    }
    
    // Reset statistics
    totalErrors = 0;
    totalWarnings = 0;
    
    // TODO: Initialize other compilation components
}

void CompilerDispatcher::finalizeCompilation() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Finalizing compilation" << std::endl;
        printDebugInfo();
    }
    
    // TODO: Finalize compilation components
}

void CompilerDispatcher::validateOutput() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Validating output" << std::endl;
    }
    
    // TODO: Validate generated output
}

std::string CompilerDispatcher::readInputFile(const std::string& filepath) {
    try {
        return FileSystem::readFile(filepath);
    } catch (const std::exception& e) {
        reportError("Cannot read input file: " + std::string(e.what()), filepath);
        throw;
    }
}

void CompilerDispatcher::writeOutputFile(const std::string& content) {
    try {
        FileSystem::writeFile(options.outputFile, content);
    } catch (const std::exception& e) {
        reportError("Cannot write output file: " + std::string(e.what()));
        throw;
    }
}

std::string CompilerDispatcher::generateOutputFilename(const std::string& inputFile) {
    std::string basename = FileSystem::getBasename(inputFile);
    size_t dotPos = basename.find_last_of('.');
    if (dotPos != std::string::npos) {
        basename = basename.substr(0, dotPos);
    }
    return basename + ".html";
}

void CompilerDispatcher::processImports() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Processing imports" << std::endl;
    }
    
    // TODO: Process import statements
}

void CompilerDispatcher::resolveDependencies() {
    if (isDebugMode()) {
        std::cout << "[DEBUG] Resolving dependencies" << std::endl;
    }
    
    // TODO: Resolve module dependencies
}

void CompilerDispatcher::printCompilationSummary() {
    std::cout << "\n=== COMPILATION SUMMARY ===" << std::endl;
    std::cout << "Files processed: " << totalFilesProcessed << std::endl;
    std::cout << "Errors: " << totalErrors << std::endl;
    std::cout << "Warnings: " << totalWarnings << std::endl;
    std::cout << "Compilation time: " << getCompilationTime() << " seconds" << std::endl;
    std::cout << "===========================\n" << std::endl;
}

void CompilerDispatcher::printErrorSummary() {
    if (totalErrors > 0 || totalWarnings > 0) {
        std::cerr << "\n=== ERROR SUMMARY ===" << std::endl;
        std::cerr << "Errors: " << totalErrors << std::endl;
        std::cerr << "Warnings: " << totalWarnings << std::endl;
        std::cerr << "=====================\n" << std::endl;
    }
}

} // namespace CHTL