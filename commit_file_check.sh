#!/bin/bash
#
# Syntax:
#   ./my-file-check filename.c
#
# Example:
#

# Run shell check to ensure this a good script.
shellcheck "$0"

# put the most reent commit ID in TARGET_COMMIT_ID
TARGET_COMMIT_ID=$(git rev-parse HEAD)

# Get the current year
CURRENT_YEAR=$(date +"%Y")

# show the files in the commit
echo ""
echo "Files in this commit:"
git diff-tree --no-commit-id --name-only "$TARGET_COMMIT_ID" -r
echo ""

# git show --color --unified=0 $TARGET_COMMIT_ID | grep -E '^\+' | grep -Ev '^\+\+\+' | sed 's/^\+//'

# git show --color=never --unified=0 $TARGET_COMMIT_ID | grep -E '^\+' | grep -vE '^\+\+\+' | sed 's/^\+//'

# Check that no line exceeds 80 chars
# awk 'length($0) > 80' $1
echo "Files that exceed 80 characters:"
git diff --unified=0 "$TARGET_COMMIT_ID"^ "$TARGET_COMMIT_ID" -- '*.h' '*.c' | grep '^\+' | grep -Ev '^\+\+\+' | awk '{if (length($0) > 80) print NR ": " $0}'

# Check the copyright date is correct
# List all files in the commit and check for the copyright notice
echo ""
echo "Files with incorrect copyright year:"
git show --name-only --pretty=format: "$TARGET_COMMIT_ID" | while read -r file; do
    if git show "$TARGET_COMMIT_ID":"$file" | grep -q "Copyright (C)"; then
        if ! git show "$TARGET_COMMIT_ID":"$file" | grep -q "Copyright (C) [0-9]\{4\}-$CURRENT_YEAR"; then
            echo "Copyright year does not match for $file"
            echo ""
        fi
    fi
done

# Check that there are no "TODO" items

# Check there are not "//" comments
echo ""
echo "Files with C++ comments:"
git show --name-only --pretty=format: "$TARGET_COMMIT_ID" | grep -E '\.(c|h)$' | while IFS= read -r file; do
    # Check for "//" comments in the file
    if git show "$TARGET_COMMIT_ID":"$file" | grep -n "//" | grep -v "https://" | grep -v "http://"; then
        echo "File contains // comments: $file"
        echo ""
    fi
done

# use wc_ptr_t not uintptr_t
echo ""
echo "Files with uintptr_t that should be wc_ptr_t:"
git show --name-only --pretty=format: "$TARGET_COMMIT_ID" | grep -E '\.(c|h)$' | while IFS= read -r file; do
    # Check for uintptr_t in the file
    if git show "$TARGET_COMMIT_ID":"$file" | grep -n "uintptr_t"; then
        echo "File uses wc_ptr_t: $file"
        echo ""
    fi
done

echo ""
echo "Files with TODO text:"
git show --name-only --pretty=format: "$TARGET_COMMIT_ID" | while IFS= read -r file; do
    # Check for and TODO comments
    if git show "$TARGET_COMMIT_ID":"$file" | grep -n "TODO"; then
        echo "File has TODO text: $file"
        echo ""
    fi
done

echo ""
echo "Fixing line endings:"
# Quietly ensure LF line endings
git show --name-only --pretty=format: "$TARGET_COMMIT_ID" | grep -E '\.(c|h|md)$' | while IFS= read -r file; do
    if file "$file" | grep -q "CRLF"; then
        dos2unix "$file"
    fi
done
