#!/bin/bash

# requires  autoconf, automake and libtool
# See https://github.com/wolfSSL/wolfssl/blob/master/INSTALL

# we expect to be starting in the config_check directory, so move to the parent
cd ".."

# the location of wolfSSL where the ./configure script should run
WOLFSSL_REPO="$PWD"


if [ ! -f "configure" ]; then
    echo "configure not found! did you forget to run autogen.sh in $PWD?"
    exit 1
fi

echo This WOLFSSL_REPO = $PWD

# the directory where output files go (a github repo is helpful for tracking changes)
WOLFSSL_FILE_ROOT="$WOLFSSL_REPO/config_check"
echo "WOLFSSL_FILE_ROOT = $WOLFSSL_FILE_ROOT"

mkdir -p "$WOLFSSL_FILE_ROOT"

# set a variable for the input command
WOLFSSL_CMD_FILE="$WOLFSSL_FILE_ROOT/cmd.txt"

# setup some variables for output files
WOLFSSL_OUTPUT="$WOLFSSL_FILE_ROOT/output.txt"
WOLFSSL_OPTIONS="$WOLFSSL_FILE_ROOT/options.h"
WOLFSSL_YES="$WOLFSSL_FILE_ROOT/Enabled-Features.txt"
WOLFSSL_NO="$WOLFSSL_FILE_ROOT/Disabled-Features.txt"

# we'll want to run configure from the root directory of wolfssl
cd "$WOLFSSL_REPO"

# save current help text for reference
./configure --help > "./help.txt"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Error"
    exit $retVal
fi


# show the command text found
echo "CMD File= $WOLFSSL_CMD_FILE"
echo ""

# get the contents of the command file, trimming all text after the # character
WOLFSSL_CMD="$(cat $WOLFSSL_CMD_FILE | cut -d'#' -f1)"

echo "Running command: "         > $WOLFSSL_OUTPUT
echo ""                         >> $WOLFSSL_OUTPUT
echo "CMD = $WOLFSSL_CMD"        | tee --append "$WOLFSSL_OUTPUT"
echo ""

echo Running configure from $PWD | tee --append "$WOLFSSL_OUTPUT"

echo ""                         >> $WOLFSSL_OUTPUT
echo "------------------------" >> $WOLFSSL_OUTPUT
echo "Output:"                  >> $WOLFSSL_OUTPUT
echo "------------------------" >> $WOLFSSL_OUTPUT
echo ""                         >> $WOLFSSL_OUTPUT

# Execute the command:
# bash -c $WOLFSSL_CMD
$(echo $WOLFSSL_CMD)             | tee --append "$WOLFSSL_OUTPUT"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Error"
    exit $retVal
fi

# save the generated options.h
echo ""
echo moving $PWD/wolfssl/options.h to "$WOLFSSL_OPTIONS"
mv wolfssl/options.h "$WOLFSSL_OPTIONS"

# pull out the enabled and disabled features into separate files
echo ""
echo "Saving enabled summary to $WOLFSSL_YES"
grep  "\*" "$WOLFSSL_OUTPUT" | grep yes > "$WOLFSSL_YES"

echo ""
echo "Saving disabled summary to $WOLFSSL_NO"
grep  "\*" "$WOLFSSL_OUTPUT" | grep no > "$WOLFSSL_NO"

echo ""
echo "See output history in $WOLFSSL_OUTPUT"

echo ""
echo "Done! Thank you for using wolfSSL"
echo ""
