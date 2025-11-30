# Repository Guidelines

## Project Structure & Module Organization

- `src/` C++ sources for the QtLucide library.
- `include/QtLucide/` public headers (API surface). Generated headers live here too.
- `resources/` Lucide SVGs and generated `.qrc`/metadata.
- `tests/` Qt Test suite (integrated via CTest/Meson).
- `examples/` Gallery and sample widgets used by tests.
- `tools/` Resource and validation scripts (e.g., `build_resources.py`).
- `scripts/` Cross‑platform build wrappers (`build.py`, `build_cmake.py`, `*.bat`).
- `cmake/`, `docs/`, `.clang-format`, `.editorconfig` for config and style.

## Build, Test, and Development Commands

- CMake (recommended):

  ```bash
  cmake -S . -B build -DQTLUCIDE_BUILD_TESTS=ON -DQTLUCIDE_BUILD_EXAMPLES=ON
  cmake --build build -j
  ctest --test-dir build --output-on-failure
  ```

- Meson (alternative):

  ```bash
  meson setup builddir_meson -Dtests=true -Dexamples=true
  meson compile -C builddir_meson
  meson test -C builddir_meson
  ```

- Unified scripts: `python3 scripts/build.py --test` (Windows: `scripts\build.bat --test`).
- Regenerate icon resources: `python3 build_all_resources.py` (auto‑runs in CMake builds).

## Coding Style & Naming Conventions

- Format: 4‑space indent, 100‑column limit. Enforce with `.clang-format` and `.editorconfig`.
- Language: C++17, Qt idioms. Document public APIs with Doxygen comments.
- Naming: Classes PascalCase (e.g., `QtLucideIconPainter`); methods lowerCamelCase (e.g., `initLucide`);
  enum values snake_case (e.g., `Icons::alert_circle`). Source files PascalCase in `src/`,
  public headers under `include/QtLucide/`.

## Testing Guidelines

- Framework: Qt Test (`Qt6::Test`). Add tests under `tests/` as `test_*.cpp/h` and list in
  `tests/CMakeLists.txt` (Meson: `tests/meson.build`).
- Run: `ctest --test-dir build --output-on-failure` or execute `QtLucideTests` directly.
- Keep tests deterministic; prefer unit tests for public APIs. Some heavy UI tests remain
  disabled in `tests/main.cpp` until stabilized.

## Commit & Pull Request Guidelines

- Commits: use Conventional Commits, e.g., `feat(icons): add theme support`,
  `fix(rendering): correct SVG scaling on HiDPI`.
- PRs: include a clear description, linked issues, test results, and screenshots for UI/icon changes.
- Before submitting: run formatters, ensure tests pass, and update `CHANGELOG.md`/docs when applicable.

## Security & Configuration Tips

- Do not edit generated headers (`QtLucideEnums.h`, `QtLucideStrings.h`)—regenerate via tools.
- Ensure Qt6 (Core/Gui/Widgets/Svg) and Python 3 are installed; Windows users can use `scripts/*.bat`.
