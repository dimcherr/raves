#rm -rf output
start=$(date +%s%3N)
python build.py $1 $2 $3 && ninja -f SoundBuild.ninja -j8 && ninja -f GLTFBuild.ninja -j8 && ninja -f ShadersBuild$2.ninja -j8 && ninja -f Build$2$3.ninja -j8 -k0 && ninja -f Build$2$3.ninja -j8 -t compdb >> compile_commands.json || exit 1
end=$(date +%s%3N)
duration=$((end - start))
echo "BUILD TIME: ${duration}ms"

if [ $2 == "Web" ]; then
    cd output/$4
    start chrome http://localhost:9222
    python -m http.server 9222
fi
exit 0
