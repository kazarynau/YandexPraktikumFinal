#!/bin/sh

BASE_DIR=$(dirname "$0")
OUT_DIR=png

rm -rf $OUT_DIR
mkdir $OUT_DIR

java -jar $PLANTUML_JAR -tpng -o $OUT_DIR "$BASE_DIR/*.*uml"
