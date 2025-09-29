# CHTL Development Roadmap

## Introduction

This document outlines a prioritized development roadmap for the CHTL project, based on the findings of the comprehensive code review. The goal is to provide a clear path from the current early alpha stage to a feature-complete, stable release.

The roadmap is divided into five distinct phases, designed to be tackled sequentially. Each phase builds upon the last, starting with solidifying the existing foundation and progressively adding more advanced capabilities.

---

## Phase 1: Foundational Features & Core Functionality

**Goal:** Achieve a stable, usable core language by completing partially implemented features. This phase is critical for establishing a reliable base for future development.

*   **1.1. Implement Full `[Origin]` Support:**
    *   **Task:** Rework the `parseOriginDefinition` function to capture the raw content within an `[Origin]` block and store it in a dedicated `OriginNode`.
    *   **Task:** Implement the generator logic to output the raw content of an `OriginNode` without modification.
    *   **Task:** Implement support for named origin blocks and their usage.

*   **1.2. Complete `[Template]` and `[Custom]` Systems:**
    *   **Task:** Implement usage of `@Var` templates in style blocks and attributes.
    *   **Task:** Implement template inheritance (both compositional and explicit `inherit`).
    *   **Task:** Implement the `[Custom]` specialization logic, starting with `@Style` specializations (`delete`, providing values to parameterless properties).

*   **1.3. Complete `[Namespace]` System:**
    *   **Task:** Implement the `parseNamespaceDefinition` function to correctly manage the parser's `namespaceStack`.
    *   **Task:** Implement the "default namespace on import" rule as a configurable option.

*   **1.4. Make Constraints (`except`) Functional:**
    *   **Task:** Implement the logic in the parser or generator to enforce the constraints defined in an element's `constraints` list, throwing errors for invalid children.
    *   **Task:** Implement global constraints within a namespace.

---

## Phase 2: Advanced CSS Integration

**Goal:** Implement the powerful, productivity-enhancing features of local style blocks, which are a core selling point of CHTL.

*   **2.1. Implement Style Block Selectors:**
    *   **Task:** Implement `parseClassOrIdSelector` to handle `.class` and `#id` selectors within a `style {}` block.
    *   **Task:** Implement the automatic addition of `class` and `id` attributes to the parent element, respecting the `DISABLE_STYLE_AUTO_ADD_*` configuration flags.
    *   **Task:** Implement the `&` selector for contextual inference (e.g., `&:hover`).

*   **2.2. Implement Style Property Expressions:**
    *   **Task:** Rework the style value parser (`parseStyleExpression`) to be a proper recursive descent parser.
    *   **Task:** Implement property arithmetic (`+`, `-`, `*`, `/`, `%`, `**`).
    *   **Task:** Implement property conditional expressions (`? :`).
    *   **Task:** Implement referenced properties from other elements (e.g., `width: .box.width / 2;`).

---

## Phase 3: CHTL JS Integration

**Goal:** Bridge the critical gap between CHTL and CHTL JS, enabling the two languages to work together as specified.

*   **3.1. Implement Local `script` Blocks:**
    *   **Task:** Add parsing logic to `ElementParsingStrategy` to recognize `script {}` blocks.
    *   **Task:** Capture the content of a `script` block and store it in a `ScriptNode`.

*   **3.2. Activate the Compiler Pipeline:**
    *   **Task:** Refactor the main parsing loop to use the `UnifiedScanner` to preprocess the source code, separating CHTL, JS, and CHTL JS fragments.
    *   **Task:** Implement the `CompilerDispatcher` to send the appropriate fragments to the CHTL Parser, CHTL JS Parser, and external JS/CSS compilers.
    *   **Task:** Implement the `CodeMerger` to reassemble the final HTML, CSS, and JS output from the results of the various compilers.

*   **3.3. Implement CHTL / CHTL JS Interaction:**
    *   **Task:** Implement the "Salt Bridge" (`SharedContext`) to allow communication between the CHTL and CHTL JS compilers.
    *   **Task:** Implement responsive values (`$var$`) and dynamic property expressions.

---

## Phase 4: Ecosystem and Modules

**Goal:** Build out the module system and advanced import/export features to support code reuse and library distribution.

*   **4.1. Complete the CMOD System:**
    *   **Task:** Implement the ability for the `Loader` to read from `.cmod` archives.
    *   **Task:** Enhance the `cmod_packer` to parse `[Info]` blocks and automatically generate `[Export]` blocks.
    *   **Task:** Implement the `[Export]` block lookup in the `ModuleResolver` to optimize imports.

*   **4.2. Implement the CJMOD System:**
    *   **Task:** Create a `cjmod_packer` utility.
    *   **Task:** Implement the CJMOD API (`Syntax`, `Arg`, `CJMODScanner`, etc.) as described in the specification.
    *   **Task:** Integrate CJMOD loading into the `CompilerDispatcher`.

*   **4.3. Complete the Import System:**
    *   **Task:** Implement all selective and wildcard import types.
    *   **Task:** Implement the `as` keyword for renaming imports.
    *   **Task:** Add support for official module directories.

---

## Phase 5: Tooling, Refinement, and Documentation

**Goal:** Improve developer experience, add VSCode support, and ensure the project is well-documented.

*   **5.1. VSCode Extension:**
    *   **Task:** Develop a VSCode extension providing syntax highlighting, code formatting, and IntelliSense based on the `[Export]` blocks from imported modules.

*   **5.2. CLI Enhancements:**
    *   **Task:** Implement the `--inline` flags and other CLI options specified in `CHTL.md`.

*   **5.3. Documentation:**
    *   **Task:** Write comprehensive user documentation that reflects the implemented features.
    *   **Task:** Update the `README.md` with clear build and usage instructions.

*   **5.4. Increase Test Coverage:**
    *   **Task:** Add unit and integration tests for all newly implemented features in each phase.