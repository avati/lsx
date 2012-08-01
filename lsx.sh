#!/bin/sh

dir=$1;

if [ ! -d "$dir" ]; then
    echo "Usage: $0 <dir>"
    exit 1;
fi

make lsx

echo 3 > /proc/sys/vm/drop_caches

strace -Tc ./lsx $dir
