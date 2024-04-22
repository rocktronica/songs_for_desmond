#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

fps="4"
rows="11"
suffix="92x64"

stub="walk"
input="local/IMG_6863-walk-bw.mov"

output_dir="local/${stub}"

mkdir -pv $output_dir >/dev/null

ffmpeg -y -i $input \
    -vf "fps=${fps},tile=1x${rows}" \
    "${output_dir}/${stub}_${fps}_${rows}_%02d_${suffix}.png"

convert "${output_dir}/${stub}_${fps}_${rows}_01_${suffix}.png" \
    -colors 2 \
    "${output_dir}/${stub}_${suffix}.png"

# open $output_dir

}