# CHTL Project: Strategic Development Roadmap

**Date:** 2025-09-29
**Author:** Jules, AI Software Engineer

## 1. Introduction

This document outlines a strategic roadmap for the development of the CHTL compiler. It is based on the detailed gap analysis presented in the `Code_Review_Report.md` and is designed to guide the project from its current state to a feature-complete and stable release.

The roadmap is divided into four distinct phases, each with a clear focus. This phased approach ensures that foundational components are built first, providing a stable base for more advanced features.

## 2. Development Phases

### Phase 1: Solidify the Core Architecture

**Goal:** To implement the complete compilation pipeline as envisioned in the `CHTL.md` specification. This is the highest priority and is essential for the success of the entire project.

**Key Objectives:**

1.  **Integrate Third-Party Compilers:**
    - Choose and integrate a suitable CSS parsing library (e.g., `libsass`, `libcss`). Modify `CMakeLists.txt` to link against it.
    - Choose and integrate a suitable JavaScript engine/parser (e.g., `QuickJS`, `V8`, or an ANTLR-based JS grammar). Modify `CMakeLists.txt` to link against it.

2.  **Implement the `UnifiedScanner`:**
    - Develop the logic within `UnifiedScanner` to accurately identify and separate a `.chtl` source file into four distinct types of code blocks: CHTL, CHTL JS, standard CSS, and standard JS.
    - Implement the "placeholder" mechanism described in the specification to isolate native code blocks while preserving the structure for the CHTL/CHTL JS compilers.

3.  **Implement the `CompilerDispatcher`:**
    - Develop the logic to receive code fragments from the `UnifiedScanner`.
    - Route CHTL fragments to the `CHTLCompiler`.
    - Route CHTL JS fragments to the `CHTLJSCompiler`.
    - Route CSS fragments to the integrated CSS compiler.
    - Route JS fragments to the integrated JS compiler.

4.  **Implement the `CodeMerger`:**
    - Develop the logic to assemble the compiled outputs from all compilers into a final set of HTML, CSS, and JS files.

### Phase 2: CHTL & CHTL JS Feature Completion

**Goal:** To build out the rich feature sets of both the CHTL and CHTL JS languages on top of the stable core architecture.

**Key Objectives:**

1.  **Complete CHTL Core Features:**
    - Ensure all basic elements, attributes, comments, and text nodes are parsed and generated correctly.
    - Fully implement local and global `style` blocks, including attribute calculations and conditional expressions.
    - Fully implement the `[Template]` and `[Custom]` systems, including inheritance and specialization (`delete`, `insert`).
    - Implement the `[Import]`, `[Namespace]`, and `[Configuration]` systems.

2.  **Complete CHTL JS Core Features:**
    - Solidify the enhanced selector (`{{...}}`) and its integration with the DOM.
    - Implement the declarative `Listen` and `Delegate` event systems.
    - Implement the `Animate` and `Router` functions.
    - Fully develop the `Vir` virtual object system and the "salt bridge" mechanism for communication with the CHTL compiler.
    - Implement responsive values (`$js_var$`) and dynamic conditional rendering.

3.  **Develop a Comprehensive Test Suite:**
    - For every feature implemented, create corresponding unit and integration tests in the `chtl_tests` executable to ensure correctness and prevent regressions.

### Phase 3: Build the Tooling and Ecosystem

**Goal:** To create the tools that will make CHTL a productive and developer-friendly language.

**Key Objectives:**

1.  **Develop the Command-Line Interface (CLI):**
    - Build a user-friendly CLI application that wraps the `chtl_compiler` executable.
    - Implement the command-line flags specified in `CHTL.md`, such as `--inline`, `--inline-css`, `--inline-js`, and `--default-struct`.
    - Add features for project initialization and module management.

2.  **Develop the VSCode Extension:**
    - Create a new repository or directory for the VSCode extension.
    - Implement syntax highlighting for `.chtl` and `.cjjs` files.
    - Implement code formatting.
    - Provide intelligent code completion and suggestions by having the extension invoke the compiler in a "language server" mode. This includes using the `[Export]` block from CMOD modules to provide context-aware suggestions.
    - Integrate the compiler to provide real-time error reporting and a live preview panel.

### Phase 4: Create Official Modules

**Goal:** To provide a rich set of pre-built components that showcase the power of CHTL and provide value to users out-of-the-box.

**Key Objectives:**

1.  **Implement the CMOD/CJMOD System:**
    - Finalize the `cmod_packer` utility.
    - Implement the full module resolution and import logic, including support for sub-modules and the `chtl::` official module prefix.
    - Finalize the CJMOD API for extending the CHTL JS compiler.

2.  **Develop the `Chtholly` Module:**
    - Create the UI components specified in `CHTL.md` (Accordion, Photo Album, etc.) as CMODs.
    - Implement the `printMylove` and `iNeverAway` utilities as CJMODs.

3.  **Develop the `Yuigahama` Module:**
    - Create the UI components specified for this module as CMODs.

4.  **Establish a Module Repository/Registry:**
    - Plan and create a central location for users to discover and share CHTL modules.