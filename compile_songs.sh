#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

midi2tones=~/midi2tones/midi2tones
songs_path="arduino/my_baby_is_a_punching_machine/songs.h"

echo "#pragma once" > "${songs_path}"

for filename in $PWD/midi/*.mid; do
    echo "${filename}"

    # midi2tones has strict input expectations: no full path, no extension
    relative_path="$(basename $filename)"
    no_extension="${relative_path%.*}"
    input="midi/${no_extension}"
    output_path="${input}.c"

    const_stub=$(echo "${no_extension}" | tr "[a-z]" "[A-Z]" | tr " " "_")
    title=$(echo "${no_extension}" | tr "_" " ")

    echo " - Creating ${output_path}"
    $midi2tones -o2 "${input}" >/dev/null
    sed -i '' "s/score/${const_stub}_SCORE/g" "${output_path}"

    echo " - Appending to ${songs_path}"
    cat "${output_path}" | grep -v "^//" >> "${songs_path}"
    echo "const char ${const_stub}_TITLE[] PROGMEM = \"${title}\";" >> "${songs_path}"

    echo " - Deleting ${output_path}"
    rm "${output_path}"

    echo
done

}