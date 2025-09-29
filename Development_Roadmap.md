# CHTL Development Roadmap

## 1. Introduction

This document outlines a strategic and prioritized roadmap for the continued development of the CHTL project. It is based on the findings of the `Code_Review_Report.md` and is designed to guide the project from its current architecturally-sound state to a feature-complete and robust implementation as envisioned in the `CHTL.md` specification.

The roadmap is divided into four distinct stages, each with a clear focus. This staged approach will ensure that foundational components are completed first, providing a stable base for subsequent features.

## 2. Development Stages

### Stage 1: Core Expression Engine & Dynamic Styling

**Goal:** Implement the runtime logic for all dynamic features. This is the most critical next step, as many advanced features depend on it.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **1.1 (Critical)** | **Expression Parser & AST:** | Design and implement a dedicated parser and AST for style expressions (e.g., `100px + 50px`, `width > 50px ? "red" : "blue"`). This will replace the current method of storing expressions as raw strings in the `DynamicStyleNode`. |
| **1.2 (Critical)** | **Expression Evaluator:** | Create an evaluation engine that can process the expression AST. This engine must handle property arithmetic, unit conversions, conditional logic (`? :`), and logical operators (`&&`, `||`). |
| **1.3 (High)** | **Cross-Element Property References:** | Integrate the expression evaluator with the `parsedNodes` list in the parser to enable the resolution of property references (e.g., `width: .box.width / 2`). |
| **1.4 (High)** | **Conditional Rendering Logic:** | Implement the evaluation logic for the conditions in `if`/`else` blocks, for both static and dynamic conditions. |
| **1.5 (Medium)** | **"Salt Bridge" Integration:** | Fully integrate the expression engine with the `SharedContext` ("Salt Bridge") to enable dynamic updates based on CHTL JS variables (`$var$`). |

### Stage 2: CHTL JS Feature Completion

**Goal:** Transition the CHTL JS compiler from a structural representation to a fully functional code generator.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **2.1 (High)** | **Implement CHTL JS Generators:** | Complete the code generation logic for all CHTL JS AST nodes (`Animate`, `Listen`, `Router`, `Delegate`, etc.). The current generator is a skeleton; this task involves writing the actual JavaScript output for each feature. |
| **2.2 (High)** | **Automatic Class/ID Injection:** | Implement the logic that automatically injects `class` or `id` attributes onto an element when a selector like `{{.box}}` or `{{#box}}` is used in a local `script` block. |
| **2.3 (Medium)** | **`ScriptLoader` Implementation:** | Write the runtime JavaScript for the AMD-style `ScriptLoader` and ensure the generator correctly outputs the loader configuration. |
| **2.4 (Medium)** | **Virtual Object (`Vir`) Logic:** | Implement the compile-time logic for `Vir` objects, ensuring they are correctly resolved to function references or other values during code generation. |

### Stage 3: Module System Finalization

**Goal:** Complete the CMOD and CJMOD module systems to enable code reuse and distribution.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **3.1 (High)** | **CMOD Import Resolution:** | Implement the full file and module resolution logic for CMOD imports, including searching in the official and local `module` directories. |
| **3.2 (High)** | **CMOD Packer Finalization:** | Finalize the `cmod_packer` tool to correctly bundle all module files and generate the `[Export]` table in the module's info file. |
| **3.3 (Medium)** | **CJMOD API Implementation:** | Design and implement the C++ API (`Syntax`, `Arg`, `CJMODScanner`, etc.) that will allow developers to extend CHTL JS with their own syntax. |
| **3.4 (Low)** | **CJMOD Packer:** | Create the packer tool for bundling CJMOD modules. |

### Stage 4: Tooling & Ecosystem

**Goal:** Build the developer tools that will make CHTL a productive and enjoyable language to use.

| Priority | Task | Description |
| :--- | :--- | :--- |
| **4.1 (High)** | **Robust CLI:** | Develop a full-featured command-line interface with support for all specified flags (`--inline`, `--default-struct`, etc.) and error reporting. |
| **4.2 (Medium)** | **VSCode Extension - Core Features:** | Create a basic VSCode extension that provides syntax highlighting and code formatting for `.chtl` files. |
| **4.3 (Low)** | **VSCode Extension - Advanced Features:** | Enhance the VSCode extension with advanced features like code completion (using the JSON query tables from module unpacking), real-time preview, and right-click compilation commands. |
| **4.4 (Low)** | **Official Modules (`Chtholly`, `Yuigahama`):** | Begin development of the official component libraries once the core language and module systems are stable. |

## 3. Conclusion

This roadmap provides a clear path forward for the CHTL project. By focusing on the stages in the order presented, the development team can systematically address the current gaps in the implementation and build upon the project's strong architectural foundation. Successful completion of these stages will result in a powerful and feature-rich language that fulfills the ambitious vision of the `CHTL.md` specification.