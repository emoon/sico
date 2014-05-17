require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"

StaticLibrary {

    Name = "CLW",

    Env = { 
        CCOPTS = { "-Wno-format-nonliteral"; Config = "macosx-*-*" },
    },

    Sources = { "lib/CLW.c" },

    Frameworks = { "OpenCL" },
}

StaticLibrary {

    Name = "CuTest",

    Env = { 
        CPPPATH = { "tests/CuTest" },
        CCOPTS = { "-Wno-format-nonliteral"; Config = "macosx-*-*" },
    },

    Sources = { 
        Glob {
            Dir = "tests/CuTest",
            Extensions = { ".c" },
        },
    },
}

-------------- Examples ------------------------

Program {
    Name = "show_devices",

    Env = {
        CPPPATH = { "lib" },
    },

    Sources = { "examples/show_devices/show_devices.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "CLW" },
    Frameworks = { "OpenCL" },
}

-------------- Programs ------------------------

Program {
    Name = "wclc",

    Env = {
        CPPPATH = { "lib" },
    },

    Sources = { "compiler/wclc.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "CLW" },
    Frameworks = { "OpenCL" },
}

--- Programs ---

Default "show_devices"
Default "wclc"
