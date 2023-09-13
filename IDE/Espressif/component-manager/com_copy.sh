#!/bin/bash

search_dir="../ESP-IDF/examples"

# Use the find command to search for files containing "__ESP_COMPONENT_SOURCE__"
# and then use grep to filter out only the matching lines with filenames
# find "$search_dir" -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} \; | while read -r file; do
find "$search_dir" \( -type d -name 'build' -o -type d -name 'out' \) -prune -o -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} \; | while read -r file; do

  # Get the relative path of the file using the 'realpath' and 'relpath' commands
  relative_path=$(realpath --relative-to="$search_dir" "$file")
  echo "$relative_path"
done

exit 1


# Define the source directory and destination directory
source_dir="../../examples"
destination_dir="./examples"

# Check if the destination directory exists, and create it if it doesn't
if [ ! -d "$destination_dir" ]; then
    mkdir -p "$destination_dir"
fi

# Read the list of files from component_manifest.txt and copy them
while IFS= read -r file_path; do
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
echo    mkdir -p "$(dirname "$full_destination_path")"

    # Copy the file to the destination
echo    cp "$full_source_path" "$full_destination_path"

    echo "Copied: $full_source_path -> $full_destination_path"
done < "component_manifest.txt"

echo "Copy operation completed."
