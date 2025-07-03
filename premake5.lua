require "clion"
include("premake/utils")

SDK_PATH = os.getenv("HL2SDKCS2")
MM_PATH = os.getenv("MMSOURCE112")

if not SDK_PATH then error("INVALID HL2SDK PATH") end
if not MM_PATH then error("INVALID METAMOD PATH") end

workspace "FUNPLAY-CS2"
   configurations { "Debug", "Release", "RelWithDebInfo" }
   platforms { "win64", "linux64" }
   location "build"

   include(path.join("premake", "spdlog.lua"))

   include(path.join("InventoryManager", "premake5.lua"))