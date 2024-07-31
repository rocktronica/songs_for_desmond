#!/bin/bash

# USAGE
# 1. ./make_sprite_sheet.sh
# 2. Tidy in pixelmator
# 3. Simplify name to example_92x64, etc
# 4. convert w/ https://www.bloggingadeadhorse.com/TeamARGSpriteConverter/

{

# Exit on error
set -o errexit
set -o errtrace

fps="4"
rows="11"
suffix="92x64"

stub="IMG_6863-walk-color"
input="local/IMG_6863-walk-color.mov"

output_dir="local/${stub}"

mkdir -pv $output_dir >/dev/null

ffmpeg -y -i $input \
    -vf "fps=${fps},tile=1x${rows}" \
    "${output_dir}/${stub}_${fps}_${rows}_%02d_${suffix}.png"

convert "${output_dir}/${stub}_${fps}_${rows}_01_${suffix}.png" \
    -colors 2 -depth 2 \
    "${output_dir}/${stub}_${suffix}.png"

open $output_dir

}