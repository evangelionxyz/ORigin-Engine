import os
import subprocess
import platform
import Utils

os.chdir('./../')

from SetupPython import PythonConfiguration as python_config
from SetupPremake import PremakeConfiguration as premake_config

python_config.Validate()
premakeInstalled = premake_config.Validate()

if platform.system() == "Windows":
    from SetupVulkan import VulkanConfiguration as vulkan_config
    vulkan_config.Validate()


print(">> Updating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])


def register_variables():
    variable_name = "ORiginEngine"
    variable_path = os.getcwd()
    if os.environ.get(variable_path) is None:
        print(">> Registering Origin Engine path...")
        if platform.system() == "Windows":
            Utils.SetWindowsSystemEnvironmentVariable(variable_name, variable_path)
            print(f">> Origin Engine path setted to {variable_path}")

def register_msbuild():
    vswherePath = "./Scripts/vswhere/vswhere.exe"
    if not os.path.exists(vswherePath):
        print(">> Downloading VSWhere")
        vswhereLink = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe"
        Utils.DownloadFile(vswhereLink, vswherePath)

    visual_studio_path = Utils.FindVisualStudioPath(vswherePath)
    variable_path = f"{visual_studio_path}\\MSBuild\\Current\\Bin"
    if Utils.AddNewWindowsSystemPathEnvironment(variable_path):
        print(f">> MSBuild path is registered to {variable_path}")

if platform.system() == "Windows":
    if premakeInstalled:
        register_variables()
        print("\n>> Generating OR1.sln...")
        premake_path = os.path.abspath("./Scripts/premake5.bat")
        subprocess.call([premake_path, "nopause"])
        register_msbuild()
else:
    print(">> ORigin requires Premake to generate project file.")
    
print(">> Setup Completed!")
