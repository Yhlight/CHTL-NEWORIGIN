# CHTL Development Roadmap

## Introduction

This document outlines a prioritized development roadmap for the CHTL project, based on the findings from the comprehensive code review. The roadmap is designed to guide development efforts by focusing on the most critical missing features and architectural weaknesses. The primary goal is to bring the CHTL language and its tooling to a state of completeness that matches the ambition of the `CHTL.md` specification.

---

## Priority 1: Implement Core CHTL Language Features

**Objective:** Implement the missing high-level directives that are fundamental to CHTL's design for modularity, reuse, and customization.

*   **1.1. Implement `[Import]` System:**
    *   **Task:** Add parser logic and AST nodes to handle `[Import]` directives for CHTL, CSS, JS, and CMOD/CJMOD files.
    *   **Rationale:** This is the most critical missing feature. Without it, projects cannot be broken down into multiple files, making the language unsuitable for any non-trivial application.

*   **1.2. Implement `[Custom]` System:**
    *   **Task:** Implement the parser logic and AST nodes for `[Custom]` blocks, including support for specialization (e.g., adding styles, inserting/deleting elements) and the `delete` keyword.
    *   **Rationale:** Customization is a core pillar of CHTL's power. This feature allows for the creation of reusable, yet flexible, components.

*   **1.3. Implement `[Configuration]` System:**
    *   **Task:** Add support for parsing `[Configuration]` blocks to allow for keyword customization and compiler behavior modification.
    - **Rationale:** This feature is essential for adapting CHTL to different coding styles and project requirements.

*   **1.4. Implement `[Namespace]` System:**
    *   **Task:** Fully implement the `[Namespace]` directive to manage scope and prevent naming conflicts, especially when using `[Import]`.
    *   **Rationale:** As projects grow and imports are used, namespaces become essential for maintaining a clean and manageable codebase.

---

## Priority 2: Mature the Architectural Backbone

**Objective:** Develop a robust and reliable compilation pipeline by maturing the language separation and dispatching logic.

*   **2.1. Implement the "Placeholder Mechanism" in the Unified Scanner:**
    *   **Task:** Rearchitect the `Scanner` to use the "占位符机制" (placeholder mechanism) described in `CHTL.md`. This involves intelligently replacing non-target language fragments with placeholders to ensure accurate separation.
    *   **Rationale:** The current scanner is too simplistic. A robust placeholder system is the only way to reliably separate the CHTL, CHTL JS, CSS, and JS code blocks in complex, nested files.

*   **2.2. Enhance the Compiler Dispatcher:**
    *   **Task:** Improve the `Dispatcher` to work in tandem with the enhanced `Scanner`. It needs to manage the full lifecycle of code fragments and their placeholders, dispatching them to the correct compilers and reassembling them correctly.
    *   **Rationale:** A smarter dispatcher is needed to orchestrate the complex, multi-stage compilation process.

---

## Priority 3: Develop the Module Ecosystem

**Objective:** Build a functional module system to enable code sharing and the creation of a CHTL component library.

*   **3.1. Enhance the CMOD/CJMOD Packer:**
    *   **Task:** Expand the `cmod_packer` script into a full-featured tool. It must support parsing `info` files, handling dependencies, and packaging modules with complex sub-module structures.
    *   **Rationale:** A powerful module system is key to building an ecosystem around CHTL and encouraging community contributions.

*   **3.2. Implement `[Export]` Table Generation:**
    *   **Task:** Integrate logic into the `cmod_packer` to automatically generate the `[Export]` table within the `info` file.
    *   **Rationale:** The export table is critical for both performance (by allowing selective loading) and for enabling IDEs to provide intelligent code completion and module browsing.

---

## Priority 4: Improve Developer Experience

**Objective:** Make the CHTL compiler and language more user-friendly and easier to debug.

*   **4.1. Implement Robust Error Handling:**
    *   **Task:** Add comprehensive error detection and reporting to both the CHTL and CHTL JS parsers. Errors should be descriptive, providing line numbers and clear messages about the syntax issue.
    *   **Rationale:** Good error handling is non-negotiable for a usable programming language. It is one of the most important factors for developer productivity.

*   **4.2. Add Inline Code Documentation:**
    *   **Task:** Add comments and documentation blocks throughout the codebase, especially in complex areas like the parsers, generators, and the scanner.
    *   **Rationale:** The current lack of documentation makes the codebase difficult to understand and maintain. This is essential for long-term project health and for onboarding new contributors.

---

## Priority 5: Complete Partial Features

**Objective:** Finish the implementation of features that are currently incomplete.

*   **5.1. Implement Template Inheritance:**
    *   **Task:** Add support for both explicit (`inherit`) and implicit (nested `@Style`) template inheritance.
*   **5.2. Enhance Property Expressions:**
    *   **Task:** Implement support for cross-element property references and dynamic expressions.
*   **5.3. Finalize the "Salt Bridge":**
    *   **Task:** Fully implement the data sharing mechanism between the CHTL and CHTL JS compilers to enable seamless integration for features like responsive values.