local projectRoot = path.getabsolute(".")
local workspaceRoot = path.join(projectRoot, "..")

project "ixwebsocket"
    kind "StaticLib"
    language "C++"

    libdirs {
        path.join(workspaceRoot, "vendor", "OpenSSL", "build", "lib")
    }

    includedirs {
        path.join(workspaceRoot, "vendor", "OpenSSL", "build", "include"),
        path.join(WS_PATH),
        path.join(WS_PATH, "ixwebsocket")
    }

    links { "ssl", "crypto", "pthread" }
    buildoptions { "-fPIC" }

    defines {
        "IXWEBSOCKET_USE_TLS",
        "IXWEBSOCKET_USE_OPEN_SSL"
    }

    files {
        path.join(WS_PATH, "ixwebsocket", "**.cpp"),
        path.join(WS_PATH, "ixwebsocket", "**.h")
    }

    pic "On"
