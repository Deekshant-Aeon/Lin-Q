add_rules("mode.debug", "mode.release")
set_project("LinQ")
set_languages("cxx17")
set_version("1.0.0")

add_cxflags("cl::/utf-8", "cl::/await:strict")
add_cxflags("gcc::-finput-charset=UTF-8", "gcc::-fexec-charset=UTF-8")
add_cxflags("clang::-finput-charset=UTF-8", "clang::-fexec-charset=UTF-8")

target("LinQ")
    set_kind("headeronly")
    add_includedirs("src", {public = true})

target("example")
    set_kind("binary")
    add_files("examples/main.cpp")
    add_deps("LinQ")