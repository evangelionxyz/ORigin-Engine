import os
import subprocess
import platform
import Utils

os.chdir('./../')

from SetupPython import PythonConfiguration as python_config
from SetupPremake import PremakeConfiguration as premake_config

python_config.Validate()
premake_installed = premake_config.Validate()

if platform.system() == "Windows":
    from SetupVulkan import VulkanConfiguration as vulkan_config
    vulkan_config.Validate()


print(">> Updating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])


def register_variables():
    engine_var_name = "ORiginEngine"
    engine_var_path = os.getcwd()

    premake_var_name = "premake5"
    premake_var_path = os.path.abspath("./Scripts/premake")

    print("> Registering ORigin Engine path...")
    Utils.set_env_variable(engine_var_name, engine_var_path)
    print("> Registering Premake5 path...")
    Utils.set_env_variable(premake_var_name, premake_var_path)

# Windows only
def register_msbuild():
    vswherePath = "./Scripts/vswhere/vswhere.exe"
    if not os.path.exists(vswherePath):
        print(">> Downloading VSWhere")
        vswhereLink = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe"
        Utils.download_file(vswhereLink, vswherePath)

    visual_studio_path = Utils.find_visual_studio(vswherePath)
    variable_path = f"{visual_studio_path}\\MSBuild\\Current\\Bin"
    if Utils.set_system_path_variable(variable_path):
        print(f">> MSBuild path is registered to {variable_path}")

if premake_installed:
    register_variables()
    if platform.system() == "Windows":
        print(">> Generating OR1.sln...")
        premake_path = os.path.abspath("./Scripts/premake5.bat")
        subprocess.call([premake_path, "nopause"])
        register_msbuild()
    elif platform.system() == "Linux":
        print(">> Generating Makefile...")
        premake_path = os.path.abspath("./Scripts/premake5.sh")
        os.chmod(premake_path, 0o755)
        subprocess.call([premake_path])
    else:
        print(">> ORigin requires Premake to generate project file.")

    print(">> Setup Completed!")
else:
    print("Premake5 does not installed")
    

