---
type: "manual"
---

Please update the .gitignore file located at `.gitignore` to prevent unnecessary files from being tracked by Git. 

Specifically, analyze the current project structure and existing .gitignore content, then enhance it by:

1. Adding patterns for any missing build artifacts, cache directories, and temporary files that are currently present in the project
2. Including comprehensive patterns for the build systems used (CMake, xmake)
3. Adding IDE-specific ignore patterns for common development environments
4. Including patterns for Python cache files (since the project uses Python build scripts)
5. Adding patterns for documentation generation artifacts
6. Organizing the .gitignore with clear section headers for better maintainability
7. Removing any duplicate entries
8. Adding patterns for testing artifacts, coverage reports, and profiling data

The goal is to create a comprehensive .gitignore that prevents all unnecessary files from being committed while maintaining good organization and readability.
