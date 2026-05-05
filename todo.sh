#!/bin/bash

echo "file,line,comment" > todo.csv

for f in src/*.h api/*/*/*.h; do
   if echo "$f" | grep -q null; then
      continue
   fi

   grep -H -n TODO "$f" | sed 's|TODO: ||g' | tr ',' '-' | tr ':' ',' | sed 's|   ||g' | sed 's|/\* ||g' | sed 's| \*/||g' >> todo.csv
done

