-- QtLucide package definition for XMake package manager

package("qtlucide")
    set_homepage("https://github.com/AstroAir/QtLucide")
    set_description("Lucide Icons for Qt Applications - A QtAwesome-compatible library with SVG-based rendering")
    set_license("MIT")

    set_urls("https://github.com/AstroAir/QtLucide/archive/refs/tags/v$(version).tar.gz",
             "https://github.com/AstroAir/QtLucide.git")

    add_versions("1.0.0", "placeholder_sha256_hash")

    add_deps("qt6base", "qt6svg")
    
    if is_plat("linux") then
        add_syslinks("pthread")
    end

    on_install("windows", "linux", "macosx", function (package)
        local configs = {}
        
        -- Configure build options
        if package:config("examples") then
            table.insert(configs, "--examples=true")
        else
            table.insert(configs, "--examples=false")
        end
        
        if package:config("tests") then
            table.insert(configs, "--tests=true")
        else
            table.insert(configs, "--tests=false")
        end
        
        -- Import and build
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cxxtypes("QtLucide::QtLucide", {includes = "QtLucide/QtLucide.h"}))
    end)
package_end()
