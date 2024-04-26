rm -rf ./build

echo "--------------------------------"
echo "No OQS, No Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
                                      || exit 1
cmake --build build --verbose


rm -rf ./build
echo "--------------------------------"
echo "No OQS, Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_EXPERIMENTAL=ON       \
                                      || exit 1
cmake --build build --verbose


rm -rf ./build
echo "--------------------------------"
echo "OQS, No Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_OQS=ON                \
                                      || echo -e "\nOk: failure as expected when missing experimental\n"

rm -rf ./build
echo "--------------------------------"
echo "OQS, Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_OQS=ON                \
    -DWOLFSSL_EXPERIMENTAL=ON       \
                                      || exit 1
cmake --build build --verbose


# Kyber

# No Kyber, No Experimental: tested above
# No Kyber, Experimental : tested above


rm -rf ./build
echo "--------------------------------"
echo "Kyber, No Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_KYBER=ON              \
                                      || echo -e "\nOk: failure as expected when missing experimental\n"

rm -rf ./build
echo "--------------------------------"
echo "Kyber, Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_KYBER=ON              \
    -DWOLFSSL_EXPERIMENTAL=ON       \
                                      || exit 1
cmake --build build --verbose


rm -rf ./build
echo "--------------------------------"
echo "OQS, Kyber, Experimental"
echo "--------------------------------"
cmake -Wno-dev -B build \
    -DCMAKE_INSTALL_PREFIX=/usr     \
    -DCMAKE_BUILD_TYPE=None         \
    -DWOLFSSL_CURVE25519=ON         \
    -DWOLFSSL_CURVE448=ON           \
    -DWOLFSSL_ED25519=ON            \
    -DWOLFSSL_ED448=ON              \
    -DWOLFSSL_REPRODUCIBLE_BUILD=ON \
    -DWOLFSSL_EXAMPLES=ON           \
    -DWOLFSSL_CRYPT_TESTS=ON        \
    -DWOLFSSL_OQS=ON                \
    -DWOLFSSL_KYBER=ON              \
    -DWOLFSSL_EXPERIMENTAL=ON       \
                                      || echo -e "\nOk: failure as expected cannot enable both Kyber and OQS at the same time\n"
cmake --build build --verbose


echo "Success: Kyber and OQS tests completed."
