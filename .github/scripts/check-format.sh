#!/bin/bash
set -e

FAILED=0

# Only staged changes
FILES=$(git diff --cached --name-only --diff-filter=ACM -- '*.c' '*.h' '*.cpp' '*.hpp')

for FILE in $FILES; do
    # Get only added lines in the staged diff
    git diff --cached --unified=0 -- "$FILE" | grep '^+' | grep -v '^+++' | while IFS= read -r line; do
        CODE="${line:1}"  # Remove leading '+'

        if echo "$CODE" | grep -q $'\t'; then
            echo "❌ $FILE: contains TAB character: $CODE"
            FAILED=1
        fi

        if ! echo "$CODE" | LC_ALL=C grep -q '^[ -~]*$'; then
            echo "❌ $FILE: contains non-ASCII characters: $CODE"
            FAILED=1
        fi

        if [ "${#CODE}" -gt 80 ]; then
            echo "❌ $FILE: exceeds 80 characters (${#CODE}): $CODE"
            FAILED=1
        fi
    done
done

if [ "$FAILED" -eq 1 ]; then
    echo "🚫 Formatting checks failed. Commit blocked."
    exit 1
else
    echo "✅ All staged lines passed formatting checks."
fi
