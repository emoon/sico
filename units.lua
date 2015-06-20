require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"

DefRule {
	Name = "OpenCLCompile",
	Command = "$(OPENCL_COMPILER) $(<) $(@)",

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

-----------------------------------------------

StaticLibrary {

    Name = "sico",
	Pass = "BuildCompiler",

    Env = { 
        CCOPTS = { "-Wno-format-nonliteral"; Config = "macosx-*-*" },
    },

    Propagate = {
	Libs = { "OpenCL"; Config = "unix-*" },
    },

    Sources = { "src/sico.c" },

    Frameworks = { "OpenCL" },
}

------------------------------------------------

StaticLibrary {

    Name = "cmocka",

    Env = { 
        CPPPATH = { "external/cmocka/include" },
        CCOPTS = {
       		{ "-Wno-everything" ; Config = "macosx-*-*" },
        	{ "/wd4204", "/wd4701", "/wd4703" ; Config = { "win32-*-*", "win64-*-*" } },
       },
    },

    Sources = { 
        Glob {
            Dir = "external/cmocka",
            Extensions = { ".c" },
        },
    },
}

------------------------------------------------

StaticLibrary {
	Name = "minifb",

	Env = { 
		CPPPATH = { "external/minifb/include" }, 
	},

	Sources = FGlob {
		Dir = "external/minifb/src",
		Extensions = { ".c", ".h", ".s", ".m" },
		Filters = {
			{ Pattern = "[/\\]windows[/\\]"; Config = { "win32-*", "win64-*" } },
			{ Pattern = "[/\\]macosx[/\\]"; Config = "mac*-*" },
			{ Pattern = "[/\\]x11[/\\]"; Config = { "x11-*" } },
		},

		Recursive = true,
	},

	Propagate = {
		Libs = {
			"user32.lib"; Config = "win32-*",
			"ws2_32.lib"; Config = "win32-*",
			"gdi32.lib"; Config = "win32-*",
		},

		Frameworks = { "Cocoa" },
	},
}

-------------- Examples ------------------------

Program {
    Name = "show_devices",
    Env = { CPPPATH = { "src" }, },
    Sources = { "examples/basic/show_devices/show_devices.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

-----------------------------------------------

Program {
    Name = "add_floats",
    Env = { CPPPATH = { "src" }, },
    Sources = { 
    	"examples/basic/add_floats/add_floats.c" ,
    	OpenCLCompile { Source = "examples/basic/add_floats/add_floats.cl" },
   	},
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

-----------------------------------------------

Program {
    Name = "mandelbrot_fractal",
    Env = { 
		CPPPATH = { "src", "external/minifb/include" }, 
    },
    Sources = { 
    	"examples/advanced/mandelbrot_fractal/mandelbrot_fractal.c" ,
    	OpenCLCompile { Source = "examples/advanced/mandelbrot_fractal/mandelbrot_fractal.cl" },
   	},
    Libs = { 
		{ "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } },
		{ "X11"; Config = "unix-gcc-*" }, 
	},

    Depends = { "sico", "minifb" },
    Frameworks = { "OpenCL" },

}

------------------------------------------------

Program {
    Name = "tests",
    Env = { CPPPATH = { "src", "external/cmocka/include" }, },
    Sources = { 
    	"tests/tests.c",
    	-- OpenCLCompile { Source = "tests/add_floats.cl" },
   	},
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico", "cmocka" },
    Frameworks = { "OpenCL" },
}

-------------- Programs ------------------------

Program {
    Name = "sicoc",
	Pass = "BuildCompiler",
	Target = "$(OPENCL_COMPILER)";

    Env = {
        CPPPATH = { "src" },
    },

    Sources = { "compiler/sicoc.c" },
    Libs = { { "OpenCL.lib", "kernel32.lib" ; Config = { "win32-*-*", "win64-*-*" } } },
    Depends = { "sico" },
    Frameworks = { "OpenCL" },
}

--- Programs ---

Default "show_devices"
Default "add_floats"
Default "mandelbrot_fractal"
Default "tests"
Default "sicoc"
