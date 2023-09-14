#!/bin/bash

search_dir="../ESP-IDF/examples"


# Define the source directory and destination directory.
# We start in           IDE/Espressif/component-manager
# We want examples from IDE/Espressif/ESP-IDF/examples
source_dir="../ESP-IDF/examples"

# We'll copy examples to publish into our local examples
# We start in           IDE/Espressif/component-manager
# Copy example files to IDE/Espressif/component-manager/examples
destination_dir="examples"

# Check if the destination directory exists, and create it if it doesn't
if [ ! -d "$destination_dir" ]; then
    mkdir -p "$destination_dir"
fi

MISSING_FILES=N
# Read the list of files from component_manifest.txt and copy them
while IFS= read -r file_path; do

    if [[ "$file_path" == "#"* ]]; then
        # If there's a manifest line starting with "#", treat it as a comment and just display:
        echo "$file_path"
    else
        # Remove leading and trailing whitespace from the file path
        file_path=$(echo "$file_path" | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')

        # Check if the file path is empty (blank line)
        if [ -z "$file_path" ]; then
            continue
        fi

        # Construct the full source and destination paths
        full_source_path="$source_dir/$file_path"
        full_destination_path="$destination_dir/$file_path"

        # Create the directory structure in the destination if it doesn't exist
        mkdir -p "$(dirname "$full_destination_path")"

        # Copy the file to the destination
        cp "$full_source_path" "$full_destination_path"
        THIS_ERROR_CODE=$?
        if [ $THIS_ERROR_CODE -eq 0 ]; then
            echo "Copied: $full_source_path -> $full_destination_path"
        else
            MISSING_FILES=Y
            # echo "WARNING: File not copied:  $full_source_path"
        fi # copy error check
    fi # manifest list
done < "component_manifest.txt"

#**************************************************************************************************
# Check if we are missing any files
#**************************************************************************************************
if [ "${MISSING_FILES^}" == "Y" ]; then
    echo "Some example files not copied. Continue?"
    #**************************************************************************************************
    # Confirm we actually want to proceed to publish.
    #**************************************************************************************************
    COMPONENT_MANAGER_CONTINUE=
    until [ "${COMPONENT_MANAGER_CONTINUE^}" == "Y" ] || [ "${COMPONENT_MANAGER_CONTINUE^}" == "N" ]; do
        read -r -n1 -p "Proceed? (Y/N) " COMPONENT_MANAGER_CONTINUE
        COMPONENT_MANAGER_CONTINUE=${COMPONENT_MANAGER_CONTINUE^};
        echo;
    done

    if [ "${COMPONENT_MANAGER_CONTINUE}" == "Y" ]; then
        echo "Continuing with missing files"
    else
        echo "Exiting..."
        exit 1
    fi
fi # missing files check



echo "Copy operation completed."
