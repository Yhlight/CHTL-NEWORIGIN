# CHTL Development Roadmap

## 1. Vision

The goal is to evolve CHTL from its current state into a fully-featured, robust, and production-ready language ecosystem as envisioned in `CHTL.md`. This roadmap prioritizes the maturation of the CHTL JS pipeline, the implementation of missing features, and the overall stability of the compiler.

This roadmap is divided into three strategic phases:
*   **Phase 1: Solidify the Foundation**
*   **Phase 2: Feature Parity and Expansion**
*   **Phase 3: Ecosystem and Polish**

---

## 2. Phase 1: Solidify the Foundation

This phase focuses on bringing the CHTL JS pipeline up to the same level of quality and robustness as the main CHTL compiler.

*   **1.1. Refactor the CHTL JS Parser**
    *   **Goal:** Replace the current string/regex-based parser with a token-based parser.
    *   **Tasks:**
        *   Implement a `CHTLJSLexer` that produces a stream of tokens for all CHTL JS syntax.
        *   Re-implement the `CHTLJSParser` to consume this token stream and build the AST. This should be modeled after the robust, state-based design of the main CHTL parser.
    *   **Rationale:** This is the most critical technical debt to address. A token-based parser is more robust, maintainable, and extensible, which is essential for implementing the full CHTL JS feature set.

*   **1.2. Implement the Unified Scanner Specification**
    *   **Goal:** Fully implement the fine-grained placeholder mechanism for interleaved JS and CHTL JS code.
    *   **Tasks:**
        *   Enhance the `UnifiedScanner` to recursively replace non-CHTL JS code blocks (like `function() { ... }`) inside a script with placeholders, as detailed in `CHTL.md`.
        *   The `CodeMerger` will need to be updated to handle the decoding of these nested placeholders.
    *   **Rationale:** This is the core requirement for enabling the seamless mix of CHTL JS and standard JavaScript, which is a major selling point of the language.

*   **1.3. Enhance Unit Testing**
    *   **Goal:** Establish a comprehensive test suite for the existing, functional components.
    *   **Tasks:**
        *   Write unit tests for the `CHTLParser` covering all implemented features (`[Template]`, `[Import]`, `if/else`, etc.).
        *   Write unit tests for the `cmod_packer`.
        *   Write integration tests that compile a sample `.chtl` file and verify the HTML, CSS, and JS output.
    *   **Rationale:** A strong test suite is essential for stability and will prevent regressions as new, more complex features are added.

---

## 3. Phase 2: Feature Parity and Expansion

This phase focuses on implementing the remaining language features from `CHTL.md` and completing the module system.

*   **2.1. Complete CHTL Feature Set**
    *   **Goal:** Implement all remaining CHTL language features.
    *   **Tasks:**
        *   Implement the full feature set for `[Custom]` element specializations (`insert`, `delete`, `[index]`).
        *   Implement `except` clauses for constraints.
        *   Implement advanced `[Configuration]` options.

*   **2.2. Complete CHTL JS Feature Set**
    *   **Goal:** Implement all remaining CHTL JS language features.
    *   **Tasks:**
        *   Implement dynamic property expressions (`width: {{box}}->width > 2 ? ...`).
        *   Implement the `iNeverAway` and `util...then` expression features.
        *   Implement the full `printMylove` functionality from the "Chtholly" official module.

*   **2.3. Finalize the CJMOD System**
    *   **Goal:** Implement the full CJMOD API for extending CHTL JS.
    *   **Tasks:**
        *   Implement the `Syntax`, `Arg`, `CJMODScanner`, and `CHTLJSFunction` classes as described in the `CJMOD API` section of the spec.
        *   Create a mechanism for CJMODs to register "transformers" that the `CHTLJSCompiler` can execute.
        *   Build the official "Chtholly" CJMOD module as a reference implementation.
    *   **Rationale:** A powerful module system is key to building a community and allowing for third-party extensions.

---

## 4. Phase 3: Ecosystem and Polish

This phase focuses on the developer experience and tools surrounding the core compiler.

*   **4.1. CLI Enhancements**
    *   **Goal:** Improve the CLI and fix minor issues.
    *   **Tasks:**
        *   Refactor the `CompilerDispatcher` to accept the more granular `--inline-css` and `--inline-js` flags directly, removing the `TODO` noted in the code.
        *   Implement the "interactive" CLI program with enhanced rendering features.

*   **4.2. VSCode Extension**
    *   **Goal:** Create a VSCode extension to support CHTL development.
    *   **Tasks:**
        *   Implement basic syntax highlighting for `.chtl` and `.cjjs` files.
        *   Implement code completion based on the `[Export]` blocks of CMODs.
        *   Integrate the compiler to provide real-time error checking and a live preview feature.

*   **4.3. Documentation and Official Modules**
    *   **Goal:** Create comprehensive documentation and build out the official modules.
    *   **Tasks:**
        *   Write user-facing documentation that is more approachable than the technical specification.
        *   Build all the CMOD components listed for the "Chtholly" and "Yuigahama" official modules (e.g., Accordion, Photo Album, Music Player).
    *   **Rationale:** Good documentation and a rich set of official components are crucial for attracting users and driving adoption of the language.