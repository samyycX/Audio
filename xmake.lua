add_rules("mode.debug", "mode.release")
add_requires("libopus", {optional = false})

SDK_PATH = "./.deps/hl2sdk"
MM_PATH = "./.deps/metamod-source"
includes("@builtin/xpack")

target("windows")  
    set_filename("audio.dll")
    set_kind("shared")
    set_plat("windows")
    add_defines("PLATFORM_WINDOWS")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("public/**.h")
    add_packages("libopus")

    add_files({
        "protobuf/generated/network_connection.pb.cc",
        "protobuf/generated/networkbasetypes.pb.cc",
        "protobuf/generated/steammessages.pb.cc",
        "protobuf/generated/netmessages.pb.cc",
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
    set_filename("audio.so")
    set_kind("shared")
    set_plat("linux")
    add_defines("PLATFORM_LINUX")
    add_files("src/*.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("public/**.h")
    add_packages("libopus")
    add_cxxflags("-fvisibility=default")

    add_files({
        "protobuf/generated/network_connection.pb.cc",
        "protobuf/generated/networkbasetypes.pb.cc",
        "protobuf/generated/steammessages.pb.cc",
        "protobuf/generated/netmessages.pb.cc",
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
        "_GLIBCXX_USE_CXX11_ABI=0"
    })

    set_languages("cxx20")

target("swext_windows")  
    set_filename("audio.ext.dll")
    set_kind("shared")
    set_plat("windows")
    add_defines("PLATFORM_WINDOWS")
    add_files("swext/src/**.cpp")
    add_files("swext/vendor/swiftly-ext/**.cpp")
    add_files("swext/vendor/lua/**.cpp")
    remove_files("swext/vendor/lua/onelua.cpp")
    add_headerfiles("swext/**.h")
    add_headerfiles("public/**.h")

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
    })

    add_files("src/utils/plat_win.cpp");


    add_includedirs({
        "vendor",
        "vendor/funchook/include",
        "swext/vendor",
        "swext/vendor/swiftly-ext/hooks",
        "swext/vendor/lua",
        "swext/vendor/LuaBridge/Source",
        "swext/src",
        "public",
        -- sdk
        SDK_PATH,
        SDK_PATH.."/thirdparty/protobuf-3.21.8/src",
        SDK_PATH.."/thirdparty/protobuf-3.21.8/third_party/googletest/googletest/include",
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

    set_languages("cxx17")

target("swext_linux")  
    set_filename("audio.ext.so")
    set_kind("shared")
    set_plat("linux")
    add_defines("PLATFORM_LINUX")
    add_files("swext/src/**.cpp")
    add_files("swext/vendor/swiftly-ext/**.cpp")
    add_files("swext/vendor/lua/**.cpp")
    remove_files("swext/vendor/lua/onelua.cpp")
    add_headerfiles("swext/**.h")
    add_headerfiles("public/**.h")

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
        "vendor",
        "vendor/funchook/include",
        "swext/vendor",
        "swext/vendor/swiftly-ext/hooks",
        "swext/vendor/lua",
        "swext/vendor/LuaBridge/Source",
        "swext/src",
        "public",
        -- sdk
        SDK_PATH,
        SDK_PATH.."/thirdparty/protobuf-3.21.8/src",
        SDK_PATH.."/thirdparty/protobuf-3.21.8/third_party/googletest/googletest/include",
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
        "_GLIBCXX_USE_CXX11_ABI=0"
    })

    set_languages("cxx17")

xpack("audio_linux")
    set_formats("zip")
    add_installfiles("build/linux/x86_64/release/audio.so", {filename = "audio.so", prefixdir = "audio/bin"})
    add_installfiles("audio.vdf", {filename = "audio.vdf", prefixdir = "metamod"})

xpack("audio_windows")
    set_formats("zip")
    add_installfiles("build/windows/x64/release/audio.dll", {filename = "audio.dll", prefixdir = "audio/bin"})
    add_installfiles("audio.vdf", {filename = "audio.vdf", prefixdir = "metamod"})

xpack("swext_audio_windows")
    set_formats("zip")
    add_installfiles("build/windows/x64/release/audio.ext.dll", {filename = "audio.ext.dll", prefixdir = ""})

xpack("swext_audio_linux")
    set_formats("zip")
    add_installfiles("build/linux/x86_64/release/audio.ext.so", {filename = "audio.ext.so", prefixdir = ""})
