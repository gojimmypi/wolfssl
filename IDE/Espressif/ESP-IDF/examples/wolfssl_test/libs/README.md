This directory contains target-specific user_settings.h files.

Files are copied when processing the project CMakeLists.txt to the ESP-IDF or local project components directories.

If a file already exists, a copy is made in both the target directory, and locally here.

It is best to not have same-name components in both the ESP-IDF and local project.

