
FORMAT="clang-format"

FORMAT_FLAGS="-i"

HH_EXT="hh"
CC_EXT="cc"

find -name *.${HH_EXT} -o -name *.${CC_EXT} | xargs ${FORMAT} ${FORMAT_FLAGS}
