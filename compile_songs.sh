#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

midi2tones=~/midi2tones/midi2tones
songs_path="arduino/songs_for_desi/song_scores.h"

echo "#pragma once" > "${songs_path}"

for filename in $PWD/midi/*.mid; do
    echo "${filename}"

    # midi2tones has strict input expectations: no full path, no extension
    relative_path="$(basename $filename)"
    no_extension="${relative_path%.*}"
    input="midi/${no_extension}"
    output_path="${input}.c"

    const_stub=$(echo "${no_extension}" | tr "[a-z]" "[A-Z]" | tr " " "_")
    title=$(echo "${no_extension}" | tr "[a-z]" "[A-Z]" | tr "_" " ")

    echo " - Creating ${output_path}"
    $midi2tones -o2 "${input}" >/dev/null
    sed -i '' "s/score/${const_stub}_SCORE/g" "${output_path}"

    echo " - Appending to ${songs_path}"
    cat "${output_path}" | grep -v "^//" >> "${songs_path}"

    echo " - Adding derived song length"
    echo "const uint16_t ${const_stub}_LENGTH = " >> "${songs_path}"
    cat "${output_path}" \
        | grep -oE ",\d+," | grep -oE "\d+" \
        | tr '\n' '+' \
        >> "${songs_path}"
    echo "0;" >> "${songs_path}"

    echo " - Deleting ${output_path}"
    rm "${output_path}"

    echo
done

}