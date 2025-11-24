import tunmake as tun 

DEBUG = "Debug"
RELEASE = "Release"
WIN = "Win"
WEB = "Web"

desc = {
    "configs": [DEBUG, RELEASE],
    "platforms": [WIN, WEB],
}

name, platform, config, f = tun.init(desc)

strings = {}
lists = {}
units = []
shaders = []
models = []
sounds = []

res = {}
strings["name"] = "{}{}{}".format(name, platform.capitalize(), config.capitalize())
strings["bin"] = "build/{}/{}".format(platform, config)
lists["defines"] = tun.defines("RAUDIO_STANDALONE SUPPORT_FILEFORMAT_OGG SUPPORT_MODULE_RAUDIO")
lists["includes"] = []
lists["libs"] = []
lists["cflags"] = ["-std=c++20", "-Wno-ignored-attributes"]
lists["lflags"] = ["-Llibs"]
strings["zipper"] = "C:/Program Files/7-Zip/7z.exe"

lists["includes"] += tun.includes("src")
lists["includes"] += tun.includes("libs", subdirs=False)
lists["includes"] += tun.includes("libs/imgui", subdirs=False)

models += tun.models("assets/models", "res/models")
sounds += tun.sounds("assets/sounds", "res/sounds")

shaders += tun.shaders("shaders", platform)
shaderLang = "glsl300es" if platform == WEB else "glsl430"
shaderCompiler = "tools/sokol-shdc.exe"

units += tun.units("src")
units += tun.units("libs")

strings["output"] = "{}.exe".format(strings["name"])

if platform == WIN:
    strings["output"] = "{}.exe".format(strings["name"])
    strings["compiler"] = "g++"
    lists["libs"] += tun.libs("kernel32 user32 shell32 gdi32 ole32 pthread")
    lists["cflags"] += tun.flags("-Wno-stringop-overflow")
    lists["lflags"] += tun.flags("")
    lists["defines"] += tun.defines("SOKOL_GLCORE")
    lists["includes"] += tun.includes("shaders/build/win")
elif platform == WEB:
    strings["output"] = "{}/index.html".format(strings["name"])
    strings["compiler"] = "C:/emsdk/upstream/emscripten/em++.bat"
    lists["defines"] += tun.defines("SOKOL_GLES3 OS_WEB EMSCRIPTEN")
    lists["cflags"] += tun.flags("-Wno-nonportable-include-path -Wno-deprecated-builtins -Wno-nontrivial-memcall")
    lists["lflags"] += tun.flags("-sUSE_WEBGL2 --shell-file=shell.html --preload-file res@res -Wno-limited-postlink-optimizations -sSTACK_SIZE=1048576 -sINITIAL_MEMORY=268435456 -sALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=\"['_main', '_onWebLoad']\" -s EXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap', 'HEAPF32']\"")
    lists["includes"] += tun.includes("shaders/build/web")

if config == DEBUG:
    lists["cflags"] += tun.flags("-g -O0")
    lists["defines"] += tun.defines("DEBUG JPH_ENABLE_ASSERTS")
elif config == RELEASE:
    if (platform == WIN):
        lists["cflags"] += tun.flags("-O2")
    elif (platform == WEB):
        lists["cflags"] += tun.flags("-Os")
    lists["defines"] += tun.defines("NDEBUG")

tun.addRules(f, strings, lists, units)

if platform == WEB and config == RELEASE:
    tun.addZipRule(f, ["index.{}".format(x) for x in ["html", "wasm", "js", "data"]] + ["nesobaka.gif"], strings["name"], "output")

tun.close(f)

tun.addShaderRules(platform, shaderCompiler, shaderLang, shaders)

tun.addGltfRules(models)
tun.addSoundRules(sounds)

