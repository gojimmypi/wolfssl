if [ -e "../../components/wolfssl/README.md" ]; then
    if [ "$IDF_PATH" == "a" ]; then
        echo "It looks like autogen.sh is running from an Espressif component"
        echo "directory. However, the IDF_PATH is not set. If you would like"
        echo "to configure wolfSSL as an Espressif component, run the export."
    else
        echo "Found Espressif IDF_PATH=$IDF_PATH"
        echo "setup component makefiles here..."
    fi
else
    echo "Not an Espressif component"
fi