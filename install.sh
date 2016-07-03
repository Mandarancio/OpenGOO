#!/bin/sh

prefix=/opt/

if [ "$1" != "" ]
then
prefix=${1}/
fi

dist=${prefix}OpenGOO

mkdir $dist
cp OpenGOO $dist/OpenGOO.bin

cp -r Levels $dist

res_dir=$dist/resources

mkdir $res_dir
cd resources
cp *.index $res_dir
cp *.level $res_dir
cp *.svg $res_dir

chmod +x $dist/OpenGOO.bin