solution "LibTask"
	configurations { "Debug", "Release" }
	platforms { "native", "x32", "x64" }
	location ( "build/" .. _ACTION )

	configuration "Debug"
		targetdir "bin/debug"

	configuration "Release"
		targetdir "bin/release"
	
	configuration "vs*"
		defines { "WINDOWS" }
	
   project "LibTask"
      kind "StaticLib"
      language "C"
	  
      files { "src/*.h", "src/*.c" }
 
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }  

   -- Project for unitesting, and aompiler assumption proving
   project "UnitTest"
      kind "ConsoleApp"
      language "C++"
	  
      files { "src/test/**.h*", "src/test/**.c*" }
	  includedirs { "third_party/lol_unit" }
	  flags { "NoRTTI", "FatalWarnings" }
	  links { "LibTask" }
 
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }  
		 
