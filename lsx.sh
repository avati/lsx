#!/bin/sh

dir=$1;

if [ ! -d "$dir" ]; then
    echo "Usage: $0 <dir>"
    exit 1;
fi

make lsx

strace -Tc ./lsx $dir