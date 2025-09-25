#!/bin/sh

TEMPLATES_DIR="`dirname $0`"
PROJECT_NAME="$1"

if [ -z "$PROJECT_NAME" ]; then
   echo "usage: $0 <project_name>"
fi

PROJECT_UPPER="`echo "$PROJECT_NAME" | tr '[:lower:]' '[:upper:]'`"
PROJECT_LOWER="`echo "$PROJECT_NAME" | tr '[:upper:]' '[:lower:]'`"

mkdir src

for f in main.c.m4 Makefile.m4; do
   OUTPUT_NAME="`basename "$f" .m4`"
   
   # Source files go in src/.
   case $OUTPUT_NAME in *.c)
      OUTPUT_NAME="src/$OUTPUT_NAME"
   esac

   m4 \
      "-DTEMPLATE=$PROJECT_UPPER" \
      "-Dtemplate=$PROJECT_LOWER" \
      "$TEMPLATES_DIR/$f" > "$TEMPLATES_DIR/../../$OUTPUT_NAME"
done
