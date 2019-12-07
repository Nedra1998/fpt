#!/bin/bash

echo > "$1"
for file in $2/*.glsl; do
  filename=$(echo "$file" | sed -E 's/.*\/([^\/]+)$/\1/g' | sed -E 's/\.| |-/_/g' | sed -E 's/_glsl/_shader_src/g')
  echo -e "const char $filename[] = {$(hexdump "$file" -ve '1/1 "0x%.2x,"')0x00};\nstatic const long unsigned int ${filename}_size = sizeof($filename);\n" >> "$1"
done

