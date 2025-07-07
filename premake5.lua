require "clion"
include("premake/utils")

SDK_PATH = os.getenv("HL2SDKCS2")
MM_PATH = os.getenv("MMSOURCE112")
WS_PATH = os.getenv("WSPATH")

if not SDK_PATH then error("INVALID HL2SDK PATH") end
if not MM_PATH then error("INVALID METAMOD PATH") end
if not WS_PATH then error("INVALID WEBSOCKET PATH") end

workspace "FUNPLAY-CS2"
   configurations { "Debug", "Release", "RelWithDebInfo" }
   platforms { "win64", "linux64" }
   location "build"

   include(path.join("premake", "spdlog.lua"))
   include(path.join("DemoVoiceFix", "premake5.lua"))