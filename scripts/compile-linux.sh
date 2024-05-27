clear
    ./premake5 gmake && cd output/build && gmake config=debug_x86_64 && gmake config=release_x86_64 && cd ../..

sh replace_assets.sh