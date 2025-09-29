# CHTL Development Roadmap (Cycle 2)

## 1. Introduction

This document outlines the second strategic roadmap for the CHTL project, based on the findings of the updated (Cycle 2) `Code_Review_Report.md`. Having completed the initial feature implementation, the project's focus will now shift from foundational work to **refinement, completion, and ecosystem development**.

This roadmap is divided into three stages, designed to address the remaining implementation gaps before expanding the project's scope with official libraries and developer tools.

## 2. Development Stages

### Stage 1: Feature Refinement and Completion

**Goal:** Eliminate all remaining "Partially Implemented" features by replacing placeholder logic with fully functional code. This is the highest priority, as it will make the core language feature-complete.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **1.1 (Critical)** | **Implement Property Reference Evaluation:** | The `ExpressionEvaluator` currently has a placeholder for resolving property references (e.g., `.box.width`). This task involves implementing the logic for the evaluator to query the main CHTL AST, find the target element, and retrieve its computed style value. |
| **1.2 (High)** | **Integrate CJMOD API into Compiler:** | The CJMOD C++ API is defined, but the CHTL JS compiler does not yet use it. This task involves modifying the compiler pipeline to detect CJMOD imports, load the custom modules (as shared libraries), and call their registered `scan` and `exportResult` functions, allowing them to hook into the compilation process. |
| **1.3 (Medium)** | **Refine Dynamic/Responsive Value Runtime:** | The "Salt Bridge" context and basic runtime exist, but the system should be enhanced. This includes refining the JavaScript runtime for more complex update scenarios and ensuring all `$var$` bindings are correctly registered and updated. |
| **1.4 (Medium)** | **Finalize Conditional Rendering (`if`/`else`):** | The parser and expression evaluator are complete, but the final code generation for conditionally rendered blocks needs to be fully implemented and tested to ensure elements are correctly added or removed from the DOM. |
| **1.5 (Low)** | **Wire Up All Configuration Options:** | The `[Configuration]` block is parsed, but not all of its options are respected by the compiler. This task involves a pass through the codebase to connect the remaining options to their corresponding logic. |

### Stage 2: Ecosystem Development - Official Modules

**Goal:** Begin building the official CHTL component libraries as described in `CHTL.md`. This will not only provide value to end-users but also serve as a real-world test of the CMOD module system.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **2.1 (High)** | **Create `Chtholly` Module:** | Create the directory structure for the `chtl::Chtholly` official module. Begin by implementing two or three of its specified CMOD components, such as the **Accordion** and the **Right-click Menu**. |
| **2.2 (Medium)** | **Create `Yuigahama` Module:** | Create the directory structure for the `chtl::Yuigahama` official module and implement one of its key components, such as the **Music Player**. |
| **2.3 (Low)** | **Implement `printMylove` CJMOD:** | As a proof-of-concept for the now-integrated CJMOD API, implement the `printMylove` function from the `Chtholly` module. This will involve creating a C++ extension that uses the CJMOD API to convert an image to console art. |

### Stage 3: Developer Experience - IDE Support

**Goal:** Begin development of the VSCode IDE extension to improve the developer experience.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **3.1 (High)** | **Syntax Highlighting:** | Create a new VSCode extension project and implement syntax highlighting for the CHTL language. This will involve creating a TextMate grammar (`.tmLanguage.json`) file that defines the tokens and scopes for CHTL syntax. |
| **3.2 (Medium)** | **Code Formatting:** | Add a code formatter to the extension. This will likely involve integrating an existing C++ or JS formatter and configuring it to match the CHTL style guidelines. |
| **3.3 (Low)** | **Module Unpacking & IntelliSense:** | Implement the logic for the extension to automatically unpack `.cmod` files and use their `[Export]` tables to provide basic code completion and hover-information for imported module components. |

## 3. Conclusion

This roadmap provides a clear path to move the CHTL project from a feature-complete state to a mature and usable ecosystem. By prioritizing the final refinement of the core language before expanding into modules and tooling, we can ensure a stable and robust foundation for future growth.