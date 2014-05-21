require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"

-- Used to generate the moc cpp files as needed for .h that uses Q_OBJECT for QtTool(s)

DefRule {
	Name = "OpenCLCompile",
	Command = "$(OPENCL_COMPILER) $(<) -o $(@)",

	Blueprint = {
		Source = { Required = true, Type = "string", Help = "Input filename", },
		OutName = { Required = false, Type = "string", Help = "Output filename", },
	},

	Setup = function (env, data)
		return {
			InputFiles    = { data.Source },
			OutputFiles   = { "$(OBJECTDIR)/_generated/" .. data.Source .. ".temp" },
		}
	end,
}


StaticLibrary {

    Name = "sico",
	Pass = "BuildCompiler",

    Env = { 
        CCOPTS = { "-Wno-format-nonliteral"; Config = "macosx-*-*" },
    },

    Sources = { "lib/sico.c" },

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
    Env = { CPPPATH = { "lib" }, },
    Sources = { "examples/show_devices/show_devices.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

Program {
    Name = "add_floats",
    Env = { CPPPATH = { "lib" }, },
    Sources = { 
    	"examples/add_floats/add_floats.c" ,
    	OpenCLCompile { Source = "examples/add_floats/add_floats.cl" },
   	},
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

-------------- Programs ------------------------

Program {
    Name = "sicoc",
	Pass = "BuildCompiler",
	Target = "$(OPENCL_COMPILER)";

    Env = {
        CPPPATH = { "lib" },
    },

    Sources = { "compiler/wclc.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

--- Programs ---

Default "show_devices"
Default "add_floats"
Default "sicoc"
