add_rules("mode.debug", "mode.release")
add_requires("libopus", {optional = false})

SDK_PATH = "./.deps/hl2sdk"
MM_PATH = "./.deps/metamod-source"
-- local SDK_PATH = os.getenv("HL2SDKCS2")
-- local MM_PATH = os.getenv("MMSOURCE")
includes("@builtin/xpack")

target("windows")  
    set_filename("AudioPlayer.dll")
    set_kind("shared")
    set_plat("windows")
    add_defines("PLATFORM_WINDOWS")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("public/**.h")
    add_packages("libopus")

    add_files({
        SDK_PATH.."/tier1/convar.cpp",
        SDK_PATH.."/public/tier0/memoverride.cpp",
        SDK_PATH.."/tier1/generichash.cpp",
        SDK_PATH.."/tier1/keyvalues3.cpp",
        "protobuf/generated/usermessages.pb.cc",
        "protobuf/generated/network_connection.pb.cc",
        "protobuf/generated/networkbasetypes.pb.cc",
        "protobuf/generated/engine_gcmessages.pb.cc",
        "protobuf/generated/steammessages.pb.cc",
        "protobuf/generated/gcsdk_gcmessages.pb.cc",
        "protobuf/generated/cstrike15_gcmessages.pb.cc",
        "protobuf/generated/cstrike15_usermessages.pb.cc",
        "protobuf/generated/usercmd.pb.cc",
        "protobuf/generated/cs_usercmd.pb.cc"
    })

    add_links({
        SDK_PATH.."/lib/public/win64/2015/libprotobuf.lib",
        SDK_PATH.."/lib/public/win64/tier0.lib",
        SDK_PATH.."/lib/public/win64/tier1.lib",
        SDK_PATH.."/lib/public/win64/interfaces.lib",
        SDK_PATH.."/lib/public/win64/mathlib.lib",
    })

    add_linkdirs({
        "vendor/funchook/lib/Release",
    })

     add_links({
        "funchook",
        "distorm",
        "d3d9"
    })

    add_links("psapi");
    add_files("src/utils/plat_win.cpp");
    
    add_includedirs({
        "src",
        "vendor/funchook/include",
        "vendor",
        -- sdk
        SDK_PATH,
        SDK_PATH.."/thirdparty/protobuf-3.21.8/src",
        SDK_PATH.."/common",
        SDK_PATH.."/game/shared",
        SDK_PATH.."/game/server",
        SDK_PATH.."/public",
        SDK_PATH.."/public/engine",
        SDK_PATH.."/public/mathlib",
        SDK_PATH.."/public/tier0",
        SDK_PATH.."/public/tier1",
        SDK_PATH.."/public/entity2",
        SDK_PATH.."/public/game/server",
        -- metamod
        MM_PATH.."/core",
        MM_PATH.."/core/sourcehook",
    })

    add_defines({
        "COMPILER_MSVC",
        "COMPILER_MSVC64",
        "PLATFORM_64BITS",
        "X64BITS",
        "WIN32",
        "WINDOWS",
        "CRT_SECURE_NO_WARNINGS",
        "CRT_SECURE_NO_DEPRECATE",
        "CRT_NONSTDC_NO_DEPRECATE",
        "_MBCS",
        "META_IS_SOURCE2"
    })

    set_languages("cxx20")

target("linux")
    set_filename("AudioPlayer.so")
    set_kind("shared")
    set_plat("linux")
    add_defines("PLATFORM_LINUX")
    add_files("src/*.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("public/**.h")
    add_packages("libopus")
    set_symbols("hidden")

    add_files({
        -- SDK_PATH.."/tier1/convar.cpp",
        -- SDK_PATH.."/public/tier0/memoverride.cpp",
        -- SDK_PATH.."/tier1/generichash.cpp",
        -- SDK_PATH.."/tier1/keyvalues3.cpp",
        "protobuf/generated/usermessages.pb.cc",
        "protobuf/generated/network_connection.pb.cc",
        "protobuf/generated/networkbasetypes.pb.cc",
        "protobuf/generated/engine_gcmessages.pb.cc",
        "protobuf/generated/steammessages.pb.cc",
        "protobuf/generated/gcsdk_gcmessages.pb.cc",
        "protobuf/generated/cstrike15_gcmessages.pb.cc",
        "protobuf/generated/cstrike15_usermessages.pb.cc",
        "protobuf/generated/usercmd.pb.cc",
        "protobuf/generated/cs_usercmd.pb.cc"
    })

    add_links({
        SDK_PATH.."/lib/linux64/release/libprotobuf.a",
        SDK_PATH.."/lib/linux64/libtier0.so",
        SDK_PATH.."/lib/linux64/tier1.a",
        SDK_PATH.."/lib/linux64/interfaces.a",
        SDK_PATH.."/lib/linux64/mathlib.a",
    })

    add_linkdirs({
        "vendor/funchook/lib/Release",
    })

     add_links({
        "funchook",
        "distorm",
    })

    add_files("src/utils/plat_unix.cpp");
    
    add_includedirs({
        "src",
        "vendor/funchook/include",
        "vendor",
        -- sdk
        SDK_PATH,
        SDK_PATH.."/thirdparty/protobuf-3.21.8/src",
        SDK_PATH.."/common",
        SDK_PATH.."/game/shared",
        SDK_PATH.."/game/server",
        SDK_PATH.."/public",
        SDK_PATH.."/public/engine",
        SDK_PATH.."/public/mathlib",
        SDK_PATH.."/public/tier0",
        SDK_PATH.."/public/tier1",
        SDK_PATH.."/public/entity2",
        SDK_PATH.."/public/game/server",
        -- metamod
        MM_PATH.."/core",
        MM_PATH.."/core/sourcehook",
    })

    add_defines({
        "_LINUX",
        "LINUX",
        "POSIX",
        "GNUC",
        "COMPILER_GCC",
        "PLATFORM_64BITS",
        "META_IS_SOURCE2",
        "_GLIBCXX_USE_CXX11_ABI=1"
    })

    set_languages("cxx17")

after_build(function (target)
    local output_dir = target:targetdir()
    local src_dir = target:scriptdir()

    os.mkdir(path.join(output_dir, "addons/AudioPlayer/bin"))
    os.mkdir(path.join(output_dir, "addons/metamod"))
    os.cp(path.join(src_dir, "AudioPlayer.vdf"), path.join(output_dir, "addons/metamod"))
    os.mv(path.join(output_dir, "AudioPlayer.dll"), path.join(output_dir, "addons/AudioPlayer/bin"))
    os.rm(path.join(output_dir, "AudioPlayer.lib"))
    os.rm(path.join(output_dir, "AudioPlayer.exp"))
end)