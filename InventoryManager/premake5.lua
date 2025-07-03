local projectRoot = path.getabsolute(".")
local workspaceRoot = path.join(projectRoot, "..")
local protoInputDir = path.join(workspaceRoot, "vendor", "protobufs", "csgo")
local protoOutDir = path.join(projectRoot, "build", "generated_protobufs")
local sdkProtoc = path.join(SDK_PATH, "devtools", "bin", "linux", "protoc")

project "InventoryManager"
   kind "SharedLib"
   language "C++"
   targetdir(path.join("bin", "%{cfg.buildcfg}"))
   location(path.join(projectRoot, "build"))
   visibility "Hidden"
   targetprefix ""

   files {
      path.join("src", "**.h"),
      path.join("src", "**.cpp"),
      path.join("src", "**", "*.h"),
      path.join("src", "**", "*.cpp"),
      path.join(protoOutDir, "**", "*.pb.cc"),
      path.join(protoOutDir, "**", "*.pb.h")
   }

   vpaths {
      ["Headers/*"] = "**.h",
      ["Sources/*"] = "**.cpp"
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      libdirs {
         path.join(workspaceRoot, "vendor", "funchook", "lib", "Debug"),
         path.join("src", "protobufs", "generated"),
         path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "lib", "linux64", "release"),
         path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "thirdparty", "protobuf-3.21.8", "src")
      }

   filter "configurations:Release or configurations:RelWithDebInfo"
      defines { "NDEBUG" }
      optimize "On"
      libdirs {
         path.join(workspaceRoot, "vendor", "funchook", "lib", "Release"),
         path.join("src", "protobufs", "generated"),
         path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "lib", "linux64", "release"),
         path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "thirdparty", "protobuf-3.21.8", "src")
      }

   filter "system:windows"
      cppdialect "C++20"
      include(path.join(workspaceRoot, "premake", "mm-windows.lua"))

   filter "system:linux"
      defines { "stricmp=strcasecmp" }
      cppdialect "C++2a"
      include(path.join(workspaceRoot, "premake", "mm-linux.lua"))
      linkoptions { "-static-libstdc++", "-static-libgcc" }
      disablewarnings { "register" }

   filter {}

   links {
      "spdlog",
      "funchook",
      "protobuf",
      "distorm",
      "ixwebsocket"
   }

   defines {
      "META_IS_SOURCE2",
      "HAVE_CONFIG_H",
      "HAVE_STDINT_H",
      "_ITERATOR_DEBUG_LEVEL=0",
      "__STDC_FORMAT_MACROS"
   }

   vectorextensions "sse"
   strictaliasing "Off"
   flags { "MultiProcessorCompile" }
   pic "On"

   includedirs {
      protoOutDir,
      protoInputDir,
      path.join(workspaceRoot, "vendor", "nlohmann"),
      path.join(workspaceRoot, "vendor", "moodycamel"),
      path.join(workspaceRoot, "vendor", "spdlog", "include"),
      path.join(workspaceRoot, "vendor", "funchook", "include"),
      path.join(workspaceRoot, "vendor", "fmt", "include"),
      path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "game", "shared"),
      path.join(workspaceRoot, "vendor", "hl2sdk-cs2", "thirdparty", "protobuf-3.21.8", "src"),
      path.join(workspaceRoot, "vendor", "ixwebsocket", "ixwebsocket"),
   }

   os.mkdir(protoOutDir)

   local protoFiles = {
      "cs_gameevents.proto", "cs_usercmd.proto", "cstrike15_gcmessages.proto",
      "cstrike15_usermessages.proto", "network_connection.proto", "networkbasetypes.proto",
      "engine_gcmessages.proto", "gcsdk_gcmessages.proto", "netmessages.proto",
      "steammessages.proto", "usermessages.proto", "gameevents.proto",
      "clientmessages.proto", "te.proto", "usercmd.proto"
   }

   function generateProtocCommands()
       local commands = {}

       for _, file in ipairs(protoFiles) do
           local inputFile = path.getabsolute(path.join(protoInputDir, file))

           local fileContent = io.readfile(inputFile)
           if not fileContent:find("syntax%s*=%s*\"proto2\"%s*;") then
               fileContent = 'syntax = "proto2";\n\n' .. fileContent
               io.writefile(inputFile, fileContent)
           end

           local command = string.format(
                   "%s --proto_path=%s --proto_path=%s --proto_path=%s --cpp_out=%s %s",
                   sdkProtoc,
                   path.getabsolute(protoInputDir),
                   path.join(SDK_PATH, "thirdparty", "protobuf-3.21.8", "src"),
                   path.join(SDK_PATH, "game", "shared"),
                   path.getabsolute(protoOutDir),
                   inputFile
           )

           table.insert(commands, command)
       end

       return commands
   end

   prebuildcommands(generateProtocCommands())
