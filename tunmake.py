import glob
import os
import sys

rootdir = "./"

def includes(dir, subdirs=False):
    res = []
    if subdirs:
        r = glob.glob(os.path.join(dir, "**/"), recursive=True)
        res += ["-I{}".format(rootdir) + os.path.normpath(f).replace(os.sep, "/") for f in r]
    else:
        res += ["-I{}".format(rootdir) + dir]
    return res

def defines(d):
    ds = d.split(" ")
    ds = ["-D{}".format(x) for x in ds]
    return ds

def libs(libStr):
    xs = libStr.split(" ")
    xs = ["-l{}".format(x) for x in xs]
    return xs

def flags(flagsStr):
    f = flagsStr.split(" ")
    return f

def units(dir):
    r = glob.glob(os.path.join(dir, "**/*.cpp"), recursive=True) + glob.glob(os.path.join(dir, "**/*.c"), recursive=True) + glob.glob(os.path.join(dir, "**/*.cc"), recursive=True)
    r = [os.path.normpath(f).replace(os.sep, "/") for f in r]
    r = [{"obj": "$bin/{}".format(x.replace(".cpp", ".o").replace(".cc", ".o").replace(".c", ".o")), "source": "{}{}".format(rootdir, x)} for x in r]
    return r

def shaders(dir, platform):
    res = []
    r = glob.glob(os.path.join(dir, "**/*.glsl"), recursive=True)
    r = [os.path.normpath(f).replace(os.sep, "/") for f in r]
    for rr in r:
        shaderName = rr.split("/")[-1]
        shaderPath = rr.replace(shaderName, "").replace(dir + "/", "")
        target = "{}{}/build/{}/{}{}.h".format(rootdir, dir, platform, shaderPath, shaderName)
        source = "{}{}/{}{}".format(rootdir, dir, shaderPath, shaderName)
        res += [{"target": target, "source": source}]
    return res

def models(sourceDir, targetDir):
    res = []
    r = glob.glob(os.path.join(sourceDir, "*.blend"), recursive=False)
    r = [os.path.normpath(f).replace(os.sep, "/") for f in r]
    for rr in r:
        modelName = rr.split("/")[-1]
        target = "{}{}/{}".format(rootdir, targetDir, modelName.replace(".blend", ".glb"))
        source = "{}{}/{}".format(rootdir, sourceDir, modelName.replace(".blend", ".glb"))
        res += [{"target": target, "source": source}]
    return res

def sounds(sourceDir, targetDir):
    res = []
    r = glob.glob(os.path.join(sourceDir, "*.wav"), recursive=False)
    r = [os.path.normpath(f).replace(os.sep, "/") for f in r]
    for rr in r:
        soundName = rr.split("/")[-1].split(".")[0]
        target = "{}{}/{}.ogg".format(rootdir, targetDir, soundName)
        source = "{}{}/{}.wav".format(rootdir, sourceDir, soundName)
        res += [{"target": target, "source": source}]
    return res

def addStrings(f, strings):
    for s in strings:
        f.write("{} = {}\n".format(s, strings[s]))

def addLists(f, lists):
    for x in lists:
        f.write("{} = {}\n".format(x, " ".join(lists[x])))

def addCompileRule(f):
    f.write("\nrule compile\n")
    f.write("  command = $compiler -MD -MF $out.d -c $in -o $out $includes $defines $cflags\n")
    f.write("  description = COMPILING $out\n")
    f.write("  depfile = $out.d\n")

def addLinkRule(f):
    f.write("\nrule link\n")
    f.write("  command = $compiler @$out.rsp -o $out $lflags $libs\n")
    f.write("  description = LINKING $out\n")
    f.write("  rspfile = $out.rsp\n")
    f.write("  rspfile_content = $in\n")

def addZipRule(f, zipTargets, projectName, targetDir):
    f.write("\nrule zip\n")
    #f.write("  command = cd {}/{} && $zipper a -tzip $out {}\n".format(targetDir, projectName, " ".join(zipTargets)))
    f.write("  command = bash scripts/zip.sh {}\n".format(projectName))
    f.write("  description = PACKAGING $out\n")
    f.write("build {}output/{}.zip: zip {}output/{}/index.html\n".format(rootdir, projectName, rootdir, projectName))

def addCompileShaderRule(f, shaders, shaderLang):
    f.write("\nrule compileShader\n")
    f.write("  command = {}tools/sokol-shdc.exe -i $in -o $out --slang {}\n".format(rootdir, shaderLang))
    for s in shaders:
        f.write("build {}: compileShader {}\n".format(s["target"], s["source"]))

def addUnits(f, units):
    f.write("\n")
    for unit in units:
        f.write("build {}: compile {}\n".format(unit["obj"], unit["source"]))
    f.write("\nbuild {}output/$output: link $\n$\n".format(rootdir))
    for unit in units:
        f.write("{} $\n".format(unit["obj"]))
    f.write("$\n\n")

def addRules(f, strings, lists, units):
    sortedStrings = dict(sorted(strings.items()))
    sortedLists = dict(sorted(lists.items()))
    addStrings(f, sortedStrings)
    addLists(f, sortedLists)
    addCompileRule(f)
    addLinkRule(f)
    addUnits(f, units)

def addShaderRules(platform, shaderCompiler, shaderLang, shaders):
    os.makedirs("build", exist_ok=True)
    f = open("ShadersBuild{}.ninja".format(platform), "w")
    f.write("compiler = {}\n".format(shaderCompiler))
    f.write("slang = {}\n\n".format(shaderLang))
    f.write("rule compileShader\n")
    f.write("  command = $compiler -i $in -o $out --slang $slang\n")
    f.write("  description = COMPILING SHADER $out\n\n")
    for s in shaders:
        f.write("build {}: compileShader {}\n".format(s["target"], s["source"]))
    f.write("\n")
    f.close()

def addGltfRules(models):
    f = open("GLTFBuild.ninja", "w")
    f.write("gltfpack = tools/gltfpack.exe\n")
    #f.write("rule compressGLTF\n")
    #f.write("  command = $gltfpack -i $in -o $out -tc -noq -v -tu\n")
    #f.write("  description = COMPRESSING GLTF $out\n\n")
    f.write("rule exportGLTF\n")
    f.write("  command = blender $in --background --python scripts/exportGLTF.py $out\n")
    f.write("  description = EXPORTING GLTF $out\n\n")
    for model in models:
        #f.write("build {}: compressGLTF {}\n".format(model["target"], model["source"]))
        f.write("build {}: exportGLTF {}\n".format(model["target"], model["source"].replace(".glb", ".blend")))
    f.write("\n")
    f.close()
    pass

def addSoundRules(sounds):
    f = open("SoundBuild.ninja", "w")
    f.write("compiler = ffmpeg\n")
    f.write("rule compileSound\n")
    f.write("  command = $compiler -i $in $out -y\n")
    f.write("  description = COMPILING SOUND $out\n\n")
    for sound in sounds:
        f.write("build {}: compileSound {}\n".format(sound["target"], sound["source"]))
    f.write("\n")
    f.close()
    pass

def init(desc):
    name = sys.argv[1]
    platform = sys.argv[2]
    config = sys.argv[3]
    possiblePlatforms = " ".join(desc["platforms"])
    possibleConfigs = " ".join(desc["configs"])
    assert platform in desc["platforms"], "Unknown platform {}. Usage: \"build.py [name] [{}] [{}]\"".format(platform, possiblePlatforms, possibleConfigs)
    assert config in desc["configs"], "Unknown config {}. Usage: \"build.py [name] [{}] [{}]\"".format(config, possiblePlatforms, possibleConfigs)

    #os.makedirs("build", exist_ok=True)
    f = open("Build{}{}.ninja".format(platform, config), "w")
    print("GENERATING NINJA FOR [NAME={} PLATFORM={} CONFIG={}]".format(name, platform, config))
    return name, platform, config, f

def close(f):
    print("DONE!")
    f.close()
