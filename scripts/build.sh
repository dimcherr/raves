#rm -rf output
start=$(date +%s%3N)
python build.py $1 $2 $3 && ninja -f SoundBuild.ninja && ninja -f GLTFBuild.ninja && ninja -f ShadersBuild$2.ninja && ninja -f Build$2$3.ninja && ninja -f Build$2$3.ninja -t compdb >> compile_commands.json || exit 1
end=$(date +%s%3N)
duration=$((end - start))
echo "BUILD TIME: ${duration}ms"

if [ $2 == "Web" ]; then
    cd output/$4
    start chrome http://localhost:9222
    python -m http.server 9222
fi
exit 0
