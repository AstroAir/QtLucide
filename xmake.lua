-- QtLucide - Lucide Icons for Qt Applications
-- XMake build configuration equivalent to CMakeLists.txt

-- Project configuration
set_project("QtLucide")
set_version("1.0.0")
set_languages("cxx17")

-- Detect if QtLucide is being built as a submodule
local is_subproject = os.scriptdir() ~= os.projectdir()

-- Build options - default to false when used as subproject
local default_examples = not is_subproject
local default_tests = not is_subproject

option("examples")
    set_default(default_examples)
    set_description("Build QtLucide examples")
    set_showmenu(true)
option_end()

option("tests")
    set_default(default_tests)
    set_description("Build QtLucide tests")
    set_showmenu(true)
option_end()

option("install")
    set_default(not is_subproject)
    set_description("Install QtLucide files")
    set_showmenu(true)
option_end()

-- Add Qt6 packages
add_requires("qt6base")
-- Note: qt6svg is included in qt6base for most distributions

-- Python will be checked during build process

-- Custom rule for resource generation
rule("generate_resources")
    set_extensions(".svg")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        -- This rule will be triggered when SVG files change
        local python_cmd = "python"
        if os.host() == "windows" then
            python_cmd = "python.exe"
        end
        local build_script = path.join(os.projectdir(), "tools", "build_resources.py")
        local project_dir = os.projectdir()

        -- Add command to generate resources
        batchcmds:show_progress(opt.progress, "Generating QtLucide resources...")
        batchcmds:vrunv(python_cmd, {build_script, project_dir})
    end)
rule_end()

-- QtLucide library target
target("QtLucide")
    set_kind("static")  -- Change to static library for easier linking

    -- Add Qt6 packages
    add_packages("qt6base")

    -- Enable Qt features
    add_rules("qt.static")

    -- Add Qt include directories manually for Windows
    if is_plat("windows") then
        add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtCore")
        add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtGui")
        add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtWidgets")
        add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtSvg")
        add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6")
        add_linkdirs("D:/msys64/mingw64/qt6-static/lib")
        add_links("Qt6Core", "Qt6Gui", "Qt6Widgets", "Qt6Svg")
    else
        add_frameworks("QtCore", "QtGui", "QtWidgets", "QtSvg")
    end

    -- Source files
    add_files("src/QtLucide.cpp")
    add_files("src/QtLucideIconEngine.cpp")
    add_files("src/QtLucideIconPainter.cpp")

    -- Header files that need MOC processing
    add_files("include/QtLucide/QtLucide.h")

    -- Other header files
    add_headerfiles("include/QtLucide/QtLucideIconEngine.h")
    add_headerfiles("include/QtLucide/QtLucideIconPainter.h")
    add_headerfiles("include/QtLucide/QtLucideEnums.h")
    add_headerfiles("include/QtLucide/QtLucideStrings.h")
    add_includedirs("include", {public = true})

    -- Add generated QRC file
    add_files("resources/icons/lucide_icons.qrc")

    -- Resources should be generated manually using: xmake generate-resources



    -- Set target properties
    set_basename("QtLucide")
    set_prefixname("")

    -- Export symbols for shared library
    if is_plat("windows") then
        add_defines("QTLUCIDE_LIBRARY")
    end

    -- Installation (only when not used as subproject or explicitly requested)
    if has_config("install") then
        on_install(function (target)
            -- Install headers
            os.cp("include/QtLucide", path.join(target:installdir(), "include"))

            -- Install library
            os.cp(target:targetfile(), path.join(target:installdir(), "lib"))

            -- Install package config (simplified version)
            local config_dir = path.join(target:installdir(), "lib", "cmake", "QtLucide")
            os.mkdir(config_dir)

            -- Create a simple config file
            local config_content = [[
# QtLucide CMake configuration file
find_dependency(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

if(NOT TARGET QtLucide::QtLucide)
    add_library(QtLucide::QtLucide SHARED IMPORTED)
    set_target_properties(QtLucide::QtLucide PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../../lib/libQtLucide.so"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../../include"
        INTERFACE_LINK_LIBRARIES "Qt6::Core;Qt6::Gui;Qt6::Widgets;Qt6::Svg"
    )
endif()
]]
            io.writefile(path.join(config_dir, "QtLucideConfig.cmake"), config_content)
        end)
    end
target_end()

-- Examples
if has_config("examples") then
    -- Basic usage example
    target("QtLucideExample")
        set_kind("binary")
        add_packages("qt6base")
        add_rules("qt.application")

        -- Set C++ standard
        set_languages("cxx17")

        -- Add Qt include directories manually for Windows
        if is_plat("windows") then
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtCore")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtGui")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtWidgets")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6")
            add_linkdirs("D:/msys64/mingw64/qt6-static/lib")
            add_links("Qt6Core", "Qt6Gui", "Qt6Widgets")
        end

        add_files("examples/basic_usage/main.cpp")
        add_files("examples/basic_usage/MainWindow.cpp")
        add_files("examples/basic_usage/MainWindow.h")

        add_deps("QtLucide")
        add_includedirs("include")

        -- Set as GUI application
        if is_plat("windows") then
            set_kind("binary")
            -- Don't use -mwindows to allow console output for debugging
        elseif is_plat("macosx") then
            set_kind("binary")
            add_ldflags("-Wl,-rpath,@executable_path/../Frameworks", {force = true})
        end
    target_end()

    -- Gallery example - Complete QtLucide Gallery Application
    target("QtLucideGallery")
        set_kind("binary")
        add_packages("qt6base")
        add_rules("qt.application")

        -- Set C++ standard
        set_languages("cxx17")

        -- Add Qt include directories manually for Windows
        if is_plat("windows") then
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtCore")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtGui")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtWidgets")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtSvg")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtConcurrent")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtNetwork")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6")
            add_linkdirs("D:/msys64/mingw64/qt6-static/lib")
            add_links("Qt6Core", "Qt6Gui", "Qt6Widgets", "Qt6Svg", "Qt6Concurrent", "Qt6Network")
        end

        -- Gallery source files (all 58 files)
        -- Configuration
        add_files("examples/gallery/src/config/LayoutConfig.h")

        -- Core functionality
        add_files("examples/gallery/src/core/GalleryTypes.h")
        add_files("examples/gallery/src/core/BatchExportManager.h")
        add_files("examples/gallery/src/core/BatchExportManager.cpp")
        add_files("examples/gallery/src/core/ExportProgressWidget.cpp")

        -- Core managers
        add_files("examples/gallery/src/core/managers/ContentManager.h")
        add_files("examples/gallery/src/core/managers/ContentManager.cpp")
        add_files("examples/gallery/src/core/managers/IconMetadataManager.h")
        add_files("examples/gallery/src/core/managers/IconMetadataManager.cpp")
        add_files("examples/gallery/src/core/managers/ImageMetadataManager.h")
        add_files("examples/gallery/src/core/managers/ImageMetadataManager.cpp")
        add_files("examples/gallery/src/core/managers/ManagerStubs.h")
        add_files("examples/gallery/src/core/managers/ManagerStubs.cpp")
        add_files("examples/gallery/src/core/managers/FavoritesManager.h")
        add_files("examples/gallery/src/core/managers/FavoritesManager.cpp")

        -- Core models
        add_files("examples/gallery/src/core/models/IconItem.h")
        add_files("examples/gallery/src/core/models/IconItem.cpp")

        -- Core utils
        add_files("examples/gallery/src/core/utils/GalleryLogger.h")
        add_files("examples/gallery/src/core/utils/GalleryLogger.cpp")
        add_files("examples/gallery/src/core/utils/ErrorHandler.h")
        add_files("examples/gallery/src/core/utils/ErrorHandler.cpp")

        -- UI themes
        add_files("examples/gallery/src/ui/themes/ThemeManager.h")
        add_files("examples/gallery/src/ui/themes/ThemeManager.cpp")

        -- UI layouts
        add_files("examples/gallery/src/ui/layouts/ResponsiveLayoutManager.h")
        add_files("examples/gallery/src/ui/layouts/ResponsiveLayoutManager.cpp")

        -- UI controllers
        add_files("examples/gallery/src/ui/controllers/SearchController.h")
        add_files("examples/gallery/src/ui/controllers/SearchController.cpp")
        add_files("examples/gallery/src/ui/controllers/PerformanceMonitor.h")
        add_files("examples/gallery/src/ui/controllers/PerformanceMonitor.cpp")

        -- UI widgets - grids
        add_files("examples/gallery/src/ui/widgets/grids/IconGridWidget.h")
        add_files("examples/gallery/src/ui/widgets/grids/IconGridWidget.cpp")
        add_files("examples/gallery/src/ui/widgets/grids/IconThumbnailGridWidget.h")
        add_files("examples/gallery/src/ui/widgets/grids/IconThumbnailGridWidget.cpp")

        -- UI widgets - search
        add_files("examples/gallery/src/ui/widgets/search/SearchWidget.h")
        add_files("examples/gallery/src/ui/widgets/search/SearchWidget.cpp")
        add_files("examples/gallery/src/ui/widgets/search/IconSearchWidget.h")
        add_files("examples/gallery/src/ui/widgets/search/IconSearchWidget.cpp")
        add_files("examples/gallery/src/ui/widgets/search/CategoryFilterWidget.h")
        add_files("examples/gallery/src/ui/widgets/search/CategoryFilterWidget.cpp")

        -- UI widgets - panels
        add_files("examples/gallery/src/ui/widgets/panels/CategorySidebarWidget.h")
        add_files("examples/gallery/src/ui/widgets/panels/CategorySidebarWidget.cpp")
        add_files("examples/gallery/src/ui/widgets/panels/IconDetailsPanel.h")
        add_files("examples/gallery/src/ui/widgets/panels/IconDetailsPanel.cpp")
        add_files("examples/gallery/src/ui/widgets/panels/FileBrowserWidget.h")
        add_files("examples/gallery/src/ui/widgets/panels/FileBrowserWidget.cpp")

        -- UI widgets - viewers
        add_files("examples/gallery/src/ui/widgets/viewers/ImageViewerWidget.h")
        add_files("examples/gallery/src/ui/widgets/viewers/ImageViewerWidget.cpp")

        -- UI dialogs
        add_files("examples/gallery/src/ui/dialogs/ExportDialog.h")
        add_files("examples/gallery/src/ui/dialogs/ExportDialog.cpp")
        add_files("examples/gallery/src/ui/dialogs/ImportDialog.h")
        add_files("examples/gallery/src/ui/dialogs/ImportDialog.cpp")
        add_files("examples/gallery/src/ui/dialogs/IconExportDialog.h")
        add_files("examples/gallery/src/ui/dialogs/IconExportDialog.cpp")
        add_files("examples/gallery/src/ui/dialogs/PreferencesDialog.h")
        add_files("examples/gallery/src/ui/dialogs/PreferencesDialog.cpp")

        -- UI windows
        add_files("examples/gallery/src/ui/windows/GalleryMainWindow.h")
        add_files("examples/gallery/src/ui/windows/GalleryMainWindow.cpp")

        -- Main application
        add_files("examples/gallery/src/main/main.cpp")

        -- Gallery include directories
        add_includedirs("include")
        add_includedirs("examples/gallery/src")
        add_includedirs("examples/gallery/src/core")
        add_includedirs("examples/gallery/src/core/utils")
        add_includedirs("examples/gallery/src/core/managers")
        add_includedirs("examples/gallery/src/core/models")
        add_includedirs("examples/gallery/src/ui")
        add_includedirs("examples/gallery/src/ui/themes")
        add_includedirs("examples/gallery/src/ui/controllers")
        add_includedirs("examples/gallery/src/ui/widgets")
        add_includedirs("examples/gallery/src/ui/widgets/grids")
        add_includedirs("examples/gallery/src/ui/widgets/panels")
        add_includedirs("examples/gallery/src/ui/widgets/search")
        add_includedirs("examples/gallery/src/ui/widgets/viewers")
        add_includedirs("examples/gallery/src/ui/dialogs")
        add_includedirs("examples/gallery/src/ui/layouts")
        add_includedirs("examples/gallery/src/ui/windows")
        add_includedirs("examples/gallery/src/main")
        add_includedirs("examples/gallery/src/config")

        -- Link QtLucide
        add_deps("QtLucide")

        -- Set output name
        set_basename("QtLucide Gallery")

        -- Set as GUI application
        if is_plat("windows") then
            set_kind("binary")
            -- Don't use -mwindows to allow console output for debugging
        elseif is_plat("macosx") then
            set_kind("binary")
            add_ldflags("-Wl,-rpath,@executable_path/../Frameworks", {force = true})
        end

        -- Installation
        on_install(function (target)
            os.cp(target:targetfile(), path.join(target:installdir(), "bin"))
        end)
    target_end()
end

-- Tests
if has_config("tests") then
    target("QtLucideTests")
        set_kind("binary")
        add_packages("qt6base")
        add_rules("qt.console")

        -- Set C++ standard
        set_languages("cxx17")

        -- Add Qt include directories manually for Windows
        if is_plat("windows") then
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtCore")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtGui")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtWidgets")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtSvg")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtSvgWidgets")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6/QtTest")
            add_includedirs("D:/msys64/mingw64/qt6-static/include/qt6")
            add_linkdirs("D:/msys64/mingw64/qt6-static/lib")
            add_links("Qt6Core", "Qt6Gui", "Qt6Widgets", "Qt6Svg", "Qt6SvgWidgets", "Qt6Test")
        end

        -- Test sources - unit/core tests
        add_files("tests/main.cpp")
        add_files("tests/unit/core/test_qtlucide.cpp")
        add_files("tests/unit/core/test_icon_loading.cpp")
        add_files("tests/unit/core/test_svg_rendering.cpp")
        add_files("tests/unit/core/test_error_handling.cpp")
        add_files("tests/unit/core/test_memory_management.cpp")
        add_files("tests/unit/core/test_boundary_conditions.cpp")
        add_files("tests/unit/core/test_thread_safety.cpp")

        -- Include gallery sources needed for tests
        add_files("examples/gallery/src/core/utils/GalleryLogger.cpp")
        add_files("examples/gallery/src/core/managers/IconMetadataManager.cpp")
        add_files("examples/gallery/src/core/models/IconItem.cpp")

        -- Test headers that need MOC
        add_files("tests/unit/core/test_qtlucide.h")
        add_files("tests/unit/core/test_icon_loading.h")
        add_files("tests/unit/core/test_svg_rendering.h")
        add_files("tests/unit/core/test_error_handling.h")
        add_files("tests/unit/core/test_memory_management.h")
        add_files("tests/unit/core/test_boundary_conditions.h")
        add_files("tests/unit/core/test_thread_safety.h")
        add_files("examples/gallery/src/core/utils/GalleryLogger.h")
        add_files("examples/gallery/src/core/managers/IconMetadataManager.h")
        add_files("examples/gallery/src/core/models/IconItem.h")

        add_deps("QtLucide")
        add_includedirs("include")
        add_includedirs("examples/gallery/src")
        add_includedirs("examples/gallery/src/core")
        add_includedirs("examples/gallery/src/core/utils")
        add_includedirs("examples/gallery/src/core/managers")
        add_includedirs("examples/gallery/src/core/models")
        add_includedirs("examples/gallery/src/ui")
        add_includedirs("examples/gallery/src/ui/themes")
        add_includedirs("examples/gallery/src/ui/layouts")
        add_includedirs("examples/gallery/src/ui/widgets")
        add_includedirs("examples/gallery/src/ui/widgets/grids")
        add_includedirs("examples/gallery/src/ui/widgets/search")
        add_includedirs("examples/gallery/src/ui/widgets/panels")
        add_includedirs("examples/gallery/src/ui/widgets/viewers")
        add_includedirs("examples/gallery/src/ui/dialogs")
        add_includedirs("examples/gallery/src/ui/windows")
        add_includedirs("examples/gallery/src/main")
        add_includedirs("examples/gallery/src/config")

        -- Link Qt Test framework
        add_frameworks("QtTest")

        -- Set as console application
        set_kind("binary")

        -- Add test runner
        on_run(function (target)
            os.exec(target:targetfile())
        end)
    target_end()
end

-- Global installation rules
on_install(function (target)
    print("QtLucide installation completed!")
end)

-- Custom tasks
task("clean-resources")
    set_menu {
        usage = "xmake clean-resources",
        description = "Clean generated resource files",
    }
    on_run(function ()
        local files_to_clean = {
            "resources/icons/lucide_icons.qrc",
            "resources/icons/metadata/icons.json",
            "include/QtLucide/QtLucideEnums.h",
            "include/QtLucide/QtLucideStrings.h"
        }

        for _, file in ipairs(files_to_clean) do
            if os.isfile(file) then
                os.rm(file)
                print("Removed: " .. file)
            end
        end
        print("Resource cleanup completed!")
    end)
task_end()

task("generate-resources")
    set_menu {
        usage = "xmake generate-resources",
        description = "Generate resource files from SVG icons",
    }
    on_run(function ()
        local python_cmd = "python"
        if os.host() == "windows" then
            python_cmd = "python.exe"
        end
        local build_script = path.join(os.projectdir(), "tools", "build_resources.py")
        local project_dir = os.projectdir()

        print("Generating QtLucide resources...")
        os.execv(python_cmd, {build_script, project_dir})
        print("Resource generation completed!")
    end)
task_end()
