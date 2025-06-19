awk 'BEGIN {c=0} /BEGIN CERTIFICATE/ {c++} {print > ("cert" c ".pem")}' mozillarootcerts.pem
for f in cert*.pem; do
    echo "Inspecting $f:"
    openssl x509 -in "$f" -noout -subject -issuer -serial
done
