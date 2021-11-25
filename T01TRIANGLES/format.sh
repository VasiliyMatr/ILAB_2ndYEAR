
FORMATTER="clang-format"
FORM_FLAGS="-i"
FILES="headers/* source/triangles.cc source/geom3D-impl/* source/geom3D-utests/*"

for FILE in ${FILES}
do
    ${FORMATTER} ${FORM_FLAGS} ${FILE}
done
