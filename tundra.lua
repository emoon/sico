local macosx = {
    Env = {
        CCOPTS = {
            "-Wall", "-Weverything", "-Werror",
            "-I.", "-DMACOSX", 
            "-Wno-missing-prototypes", "-Wno-padded",
            { "-O0", "-g"; Config = "*-*-debug" },
            { "-O3", "-g"; Config = "*-*-release" },
        },

        OPENCL_COMPILER = "$(OBJECTDIR)$(SEP)wclc$(PROGSUFFIX)",
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
            "/W4", "/I.", "/WX", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS", "/wd4996", "/wd4389",
            { "/Od"; Config = "*-*-debug" },
            { "/O2"; Config = "*-*-release" },
        },
    },
}

local win64 = {
    Env = {
		CPPPATH = {
			{ "external/windows/include" ; Config = "win32-*-*" },
		},
        GENERATE_PDB = "1",
        CCOPTS = {
            "/MT", "/W4", "/I.", "/WX", "/DUNICODE", "/D_UNICODE", "/DWIN32", "/D_CRT_SECURE_NO_WARNINGS", "/wd4152", "/wd4996", "/wd4389",
            { "/Od"; Config = "*-*-debug" },
            { "/O2"; Config = "*-*-release" },
        },
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
