---
name: Bug report
about: Create a report to help us improve QtLucide
title: '[BUG] '
labels: 'bug'
assignees: ''

---

## Bug Description

A clear and concise description of what the bug is.

## To Reproduce

Steps to reproduce the behavior:

1. Go to '...'
2. Click on '....'
3. Scroll down to '....'
4. See error

## Expected Behavior

A clear and concise description of what you expected to happen.

## Actual Behavior

A clear and concise description of what actually happened.

## Screenshots

If applicable, add screenshots to help explain your problem.

## Environment

**Desktop (please complete the following information):**

- OS: [e.g. Windows 11, Ubuntu 22.04, macOS 13.0]
- Qt Version: [e.g. 6.5.0, 6.6.0]
- QtLucide Version: [e.g. 1.0.0]
- Compiler: [e.g. MSVC 2022, GCC 11, Clang 15]

**Additional context:**

- Build type: [Debug/Release]
- CMake version: [e.g. 3.25.0]
- Any custom build flags or options

## Code Sample

If applicable, provide a minimal code example that reproduces the issue:

```cpp
#include <QtLucide/QtLucide.h>

// Your minimal reproduction code here
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();
QIcon icon = lucide->icon("problematic-icon-name");
```

## Error Messages

If there are any error messages, compiler warnings, or console output, please include them here:

```
Paste error messages here
```

## Additional Context

Add any other context about the problem here. For example:

- Does this happen with all icons or specific ones?
- Does this happen in both Debug and Release builds?
- Any workarounds you've found?
- Related issues or discussions?

## Checklist

- [ ] I have searched existing issues to ensure this is not a duplicate
- [ ] I have provided a minimal code example that reproduces the issue
- [ ] I have included my environment information
- [ ] I have tested with the latest version of QtLucide
