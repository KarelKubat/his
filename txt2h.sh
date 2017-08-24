#!/bin/sh

echo "/* Auto-generated from $1. Do not edit. */" > $2
echo "/* Instead adapt $1 and run 'make $2' */" >> $2
echo >> $2
echo "#define $3 \\" >> $2
# sed 's:^:    ":' $1 | sed 's:$:\\n" \\:' >> $2
echo '    "\n"' >> $2
