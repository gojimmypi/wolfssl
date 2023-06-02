#!/bin/bash

# check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo "Please follows the instruction of ESP-IDF installation and set IDF_PATH."
    exit 1
fi

# make sure it actually exists
if [ ! -d "$IDF_PATH" ]; then
    echo "ESP-IDF Development Framework doesn't exist.: $IDF_PATH"
    exit 1
fi

# is export.sh in the IDF path?
if [ ! -e "$IDF_PATH/export.sh" ]; then
    echo "ESP-IDF export.sh: $IDF_PATH/export.sh"
    exit 1
fi

# check if IDF_COMPONENT_API_TOKEN is set
if [ -z "IDF_COMPONENT_API_TOKEN" ]; then
    echo "Please follow the instructions and set IDF_COMPONENT_API_TOKEN."
    exit 1
fi

echo ""
echo "Publishing local wolfSSL source to ESP Registry: components.espressif.com"
echo ""
echo "WARNING: The live wolfSSL will be replaced upon completion."
echo ""
echo "Current source directory:"
echo ""
pwd
echo ""
echo "Version to publish in local idf_component.yml (version numbers cannot be reused!)"
echo ""
grep "version:" idf_component.yml
echo ""

# copy all source files related to the ESP Component Registry
# All files from the wolfssl-gojimmypi\IDE\Espressif\ESP-IDF\examples
# directory that contain the text: __ESP_COMPONENT_SOURCE__
# will be copied to the local ESP Registry ./examples/ directory
echo "Copying __ESP_COMPONENT_SOURCE__ tagged files..."
# grep -r --files-with-matches "__ESP_COMPONENT_SOURCE__" ../ESP-IDF/examples/ | xargs -I {} cp --parents ./new_examples/ {}

# find ../ESP-IDF/examples/ -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} + | xargs -I {} cp --parents {} ./new_examples/

# go to the root of the Espressif examples
export PUB_CURRENT_PATH=$(pwd)
echo Current Path saved: $PUB_CURRENT_PATH

cd ../../Espressif/ESP-IDF/examples

echo "Copying example sample files tagged with __ESP_COMPONENT_SOURCE__ from:"
echo ""
pwd
echo ""

echo "Found example [source] files to copy to [destination]:"
echo ""
find ./ -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} + | xargs -I {} echo {}   ../../component-manager/examples/{}

# The cp command seems to not like creating a directory struture, even with --parents
# so we create the directory in advance:
echo "Creating directories in destination..."
find ./ -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} + | xargs -I {} sh -c 'mkdir --parents ../../component-manager/examples/"$(dirname {})"'
find ../../component-manager/examples/ -type d

# need to test build
# need to purge build and managed components directories


# this is the same as the "Found File" above, but copying instead os displaying:
echo Copying files...
find ./ -type f -exec grep -l "__ESP_COMPONENT_SOURCE__" {} + | xargs -I {} cp   {}   ../../component-manager/examples/{}

cd "$PUB_CURRENT_PATH"
echo "Returned to path:"
pwd

# find  ./examples/ -maxdepth 1 -mindepth 1 -type d | xargs -I {} sh -c 'cd {} && idf.py build'


# build all the projects in ./examples/
# if an error is encountered, create a semaphore file called build_failed.txt
find  ./examples/ -maxdepth 1 -mindepth 1 -type d | xargs -I {} sh -c 'cd {} && echo "\n\nBuilding {}\n\n" && idf.py build || touch ../build_failed.txt'

# is export.sh in the IDF path?
if [ -e "build_failed.txt" ]; then
    echo "Build failed!"
    exit 1
fi

# Delete any managed components and build directories before uploading.
# The files *should* be excluded by default, so this is just local housekeeping.
find  ./examples/ -maxdepth 1 -type d | xargs -I {} rm -r {}/managed_components
find  ./examples/ -maxdepth 1 -type d | xargs -I {} rm -r {}/build

echo ""
echo "Samples file to publish:"
echo ""
find ./examples/
echo ""


# Confirm we actually want to proceed.
COMPONENT_MANAGER_PUBLISH=
until [ "${COMPONENT_MANAGER_PUBLISH^}" == "Y" ] || [ "${COMPONENT_MANAGER_PUBLISH^}" == "N" ]; do
    read -n1 -p "Proceed (Y/N) " COMPONENT_MANAGER_PUBLISH
    COMPONENT_MANAGER_PUBLISH=${COMPONENT_MANAGER_PUBLISH^};
    echo;
done

if [ "${COMPONENT_MANAGER_PUBLISH}" == "Y" ]; then
    echo;
    echo "Here we go!"
    echo ""
    echo "Creating files in .\dist\ then creating .tgz to upload. Please be patient..."
    # The component will be called "wolfssl__wolfssl". There's no way to change that at this time.
    # Unfortunately, there is no way to change the build-system name of a dependency installed
    # by the component manager. It's always `namespace__component`.
    # compote component upload --namespace wolfssl --name wolfssl
    echo ""
    echo "View the new component at https://components.espressif.com/components/wolfssl/wolfssl"
    echo ""
    echo "Done!"
    echo ""
else
    echo;
    echo "No files published!"
fi


