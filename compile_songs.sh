#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

midi2tones=~/midi2tones/midi2tones
songs_path="arduino/my_baby_is_a_punching_machine/songs.h"

echo "#pragma once" > "${songs_path}"

for filename in $PWD/midi/*.mid; do
    # midi2tones has strict input expectations: no full path, no extension
    relative_path="$(basename $filename)"
    stub="${relative_path%.*}"
    input="midi/${stub}"
    output_path="${input}.c"

    const_name=$(echo "${stub}" | tr '[a-z]' '[A-Z]')

    echo "INPUT:    ${input}"
    echo "STUB:     ${stub}"
    echo "CONST:    ${const_name}"
    echo

    $midi2tones -o2 "${input}"
    sed -i '' "s/score/${const_name}/g" "${output_path}"
    echo

    echo "Appending ${output_path} code to ${songs_path}"
    cat "${output_path}" | grep -v "^//" >> "${songs_path}"

    echo "Deleting ${output_path}"
    rm "${output_path}"
done

}