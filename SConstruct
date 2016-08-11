import os

vars = Variables(None, ARGUMENTS)

vars.AddVariables(
	BoolVariable("debug", "Make a debug build", False)
)

env = Environment(
	variables = vars,
	LIBPATH = ["nbt/bin"],
	CPPPATH = ["src", "nbt/src"],
)

Help(vars.GenerateHelpText(env))

env.MergeFlags("--std=c++11 -Wall -Wextra -Wpedantic")
if env["debug"]:
	env.MergeFlags("-O0 -g")
else:
	env.MergeFlags("-O3")

sources = [
	"src/main.cpp",
	"src/convert.cpp",
	"src/threads.cpp",
	"src/MCMap.cpp",
	"src/MTMap.cpp",
]

env.Program("bin/MC2MT", sources,
	LIBS = ["nbt", "z", "boost_filesystem", "boost_system", "sqlite3", "pthread"]
)

