#!/usr/bin/env bash
start=$(date +%s%3N)

echo "PREPARE..."
python build.py "$1" "$2" "$3"
ninja -f SoundBuild.ninja -j16
ninja -f GLTFBuild.ninja -j16
ninja -f "ShadersBuild$2.ninja" -j16

echo "BUILD..."
ninja -f "Build$2$3.ninja" -j16 -k0
ninja -f "Build$2$3.ninja" -j16 -t compdb > compile_commands.json

end=$(date +%s%3N)
duration=$((end - start))
echo "BUILD TIME: ${duration}ms"

if [ "$2" == "Web" ]; then
    cd "output/$4"
    start chrome http://localhost:9222
    python -m http.server 9222
fi

