
TEST_FILES="t1 t2"
EXEC_NAME="cache"

for FILE in ${TEST_FILES}
do
    echo "Testin with file ${FILE}:"
    ./${EXEC_NAME} < ${FILE}
done

echo