# from SCons.Script import Import
Import("env")
import subprocess
import os

def before_build(source, target, env):
    pio_wolfssl_root = os.environ.get("PIO_WOLFSSL_ROOT", "Not_defined")
    print("pio_wolfssl_root = ", pio_wolfssl_root)
    print("pre action!")

env.AddPreAction("buildprog", before_build)
env.AddPreAction(
    "buildprog",
    env.VerboseAction("echo Running pre-build shell command", "Pre-build shell command executed")
)
print("TADA! 4")
pio_wolfssl_root = os.environ.get("PIO_WOLFSSL_ROOT", "Not_defined")
build_dir2 = env.subst("$PLATFORMIO_BUILD_DIR")
print("Build Directory2:", build_dir2)

print("pio_wolfssl_root = ", pio_wolfssl_root)
print("Project Directory:", env["PROJECT_DIR"])
build_dir = env.subst("$BUILD_DIR")
print("Build Directory:", build_dir)

with open(os.path.join(build_dir,"x509_crt_bundle_wolfssl.S"), 'w') as file:
    # Print text to the file
    print('Hello, World!', file=file)

#    x509_crt_bundle_wolfssl.S

print("Upload Port:", env.get("UPLOAD_PORT", "Not defined"))
print("Build Environment:", env["PIOENV"])
print("Platform:", env["PIOPLATFORM"])

# Define paths and command variables
build_dir = ("$BUILD_DIR")  # PlatformIO's build directory
esp_idf_wolfssl_dir = os.path.join(build_dir, "esp-idf", "wolfssl")

# Manually set PlatformIO core directory
# os.environ.get("PLATFORMIO_CORE_DIR")
platformio_core_dir = r"C:\Users\gojimmypi\.platformio"

# Define the commands to run the required CMake command
def generate_x509_crt_bundle_wolfssl(*args, **kwargs):
    command = [
        "cmd.exe", "/C",
        "cd /D " + build_dir + " && " +
        os.path.join(platformio_core_dir, ".platformio", "packages", "tool-cmake", "bin", "cmake.exe") +
        " -D DATA_FILE=" + os.path.join(build_dir, "esp-idf", "wolfssl", "x509_crt_bundle_wolfssl") +
        " -D SOURCE_FILE=" + os.path.join(build_dir, "x509_crt_bundle_wolfssl.S") +
        " -D FILE_TYPE=BINARY -P " +
        os.path.join(platformio_core_dir, ".platformio", "packages", "framework-espidf", "tools", "cmake", "scripts", "data_file_embed_asm.cmake")
    ]

    # Execute the command
    print("Running command:", " ".join(command))
    subprocess.run(" ".join(command), shell=True, check=True)

if __name__ == "__main__":
    generate_x509_crt_bundle_wolfssl()
# Register the command to run before building the firmware
# env.AddPreAction("$BUILD_DIR/firmware.bin", generate_x509_crt_bundle_wolfssl)
