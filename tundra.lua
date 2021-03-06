local macosx = {
    Env = {
        CCOPTS = {
			"-Wpedantic", "-Werror", "-Wall",
            "-I.", "-DMACOSX", 
            "-Wno-missing-prototypes", "-Wno-padded",
            { "-O0", "-g"; Config = "*-*-debug" },
            { "-O3", "-g"; Config = "*-*-release" },
        },

        OPENCL_COMPILER = "$(OBJECTDIR)$(SEP)sicoc$(PROGSUFFIX)",
    },
}

local unix = {
    Env = {
        OPENCL_COMPILER = "$(OBJECTDIR)$(SEP)sicoc$(PROGSUFFIX)",
    },
}

local win32 = {
    Env = {
		CPPPATH = {
			{ "external/windows/include" ; Config = "win32-*-*" },
		},
		LIBPATH = {
			{ "external/windows/lib/x86" ; Config = "win32-*-*" },
		},
    
        GENERATE_PDB = "1",
        CCOPTS = {
            "/W4", "/I.", "/WX", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS", "/wd4996", "/wd4389", "/wd4706", "/wd4204",
            { "/Od"; Config = "*-*-debug" },
            { "/O2"; Config = "*-*-release" },
        },

        OPENCL_COMPILER = "$(OBJECTDIR)$(SEP)sicoc$(PROGSUFFIX)",
    },
}

local win64 = {
    Env = {
	CPPPATH = {
		{ "external/windows/include" },
	},
	LIBPATH = {
		{ "external/windows/lib/amd64" },
	},

        GENERATE_PDB = "1",
        CCOPTS = {
            "/MT", "/W4", "/I.", "/WX", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS", "/wd4152", "/wd4996", "/wd4389", "/wd4204", "/wd4706",
            { "/Od"; Config = "*-*-debug" },
            { "/O2"; Config = "*-*-release" },
        },

        OPENCL_COMPILER = "$(OBJECTDIR)$(SEP)sicoc$(PROGSUFFIX)",
    },
}

Build {

    Units = "units.lua",

	Passes = {
		BuildCompiler = { Name="Build Compiler", BuildOrder = 1 },
	},

    Configs = {
        Config { Name = "macosx-clang", DefaultOnHost = "macosx", Inherit = macosx, Tools = { "clang-osx" } },
        Config { Name = "win32-msvc", DefaultOnHost = { "windows" }, Inherit = win32, Tools = { "msvc" } },
        Config { Name = "win64-msvc", DefaultOnHost = { "windows" }, Inherit = win64, Tools = { "msvc" } },
        Config { Name = "unix-gcc", DefaultOnHost = { "linux" }, Inherit = unix, Tools = { "gcc" } },
    },

    IdeGenerationHints = {
        Msvc = {
            -- Remap config names to MSVC platform names (affects things like header scanning & debugging)
            PlatformMappings = {
                ['win64-msvc'] = 'x64',
            },
            -- Remap variant names to MSVC friendly names
            VariantMappings = {
                ['release']    = 'Release',
                ['debug']      = 'Debug',
            },
        },
    },
    
}
