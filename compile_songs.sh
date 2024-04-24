#!/bin/bash

{

# Exit on error
set -o errexit
set -o errtrace

midi2tones=~/midi2tones/midi2tones
lengths_path="arduino/songs_for_desi/song_lengths.h"
scores_path="arduino/songs_for_desi/song_scores.h"
titles_path="arduino/songs_for_desi/song_titles.h"

echo "#pragma once" > "${scores_path}"
echo >> "${scores_path}"
echo "// Generated by ./compile_songs.sh" >> "${scores_path}"
echo "// Don't edit manually!" >> "${scores_path}"
echo  >> "${scores_path}"

# TODO: DRY
echo "#pragma once" > "${lengths_path}"
echo >> "${lengths_path}"
echo "// Generated by ./compile_songs.sh" >> "${lengths_path}"
echo "// Don't edit manually!" >> "${lengths_path}"
echo  >> "${lengths_path}"

i=0

for filename in $PWD/midi/*.mid; do
    echo "${i}: ${filename}"

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

    echo " - Appending to ${scores_path}"
    cat "${output_path}" | grep -v "^//" >> "${scores_path}"

    echo " - Deriving length for ${lengths_path}"
    math_string=$(
        cat "${output_path}" \
            | grep -oE ",\d+," | grep -oE "\d+" \
            | tr '\n' '+';
        echo "0"
    )
    echo "const uint16_t ${const_stub}_LENGTH = $((math_string));" \
        >> "${lengths_path}"

    echo " - Checking ${titles_path}"
    if ! grep -q $const_stub $titles_path; then
        echo "   - Stubbing ${const_stub}"
        echo >> "${titles_path}"
        echo "// TODO: move these into place" >> "${titles_path}"
        echo "  ${const_stub}_TITLE," >> "${titles_path}"
        echo "const char ${const_stub}_TITLE[] PROGMEM = \"${title}\";" \
            >> "${titles_path}"
    fi

    echo " - Deleting ${output_path}"
    rm "${output_path}"

    echo

    i=$((i + 1))
done

echo "
const uint16_t SONG_SCORES[] = {" >> "${scores_path}"
echo "
const uint16_t SONG_LENGTHS[] = {" >> "${lengths_path}"

for filename in $PWD/midi/*.mid; do
    # TODO: DRY
    relative_path="$(basename $filename)"
    no_extension="${relative_path%.*}"
    const_stub=$(echo "${no_extension}" | tr "[a-z]" "[A-Z]" | tr " " "_")

    echo "  ${const_stub}_SCORE," >> "${scores_path}"
    echo "  ${const_stub}_LENGTH," >> "${lengths_path}"
done

echo "};" >> "${scores_path}"
echo "};" >> "${lengths_path}"

}

echo "
# define SONGS_COUNT     ${i}" \
    >> "${lengths_path}"