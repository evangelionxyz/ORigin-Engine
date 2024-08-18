import os
import subprocess
import platform
import Utils

os.chdir('./../')
from SetupPython import PythonConfiguration as PythonRequirements
if platform.system() == "Windows":
    PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements
VulkanRequirements.Validate()

from SetupPremake import PremakeConfiguration as PremakeRequirements
premakeInstalled = PremakeRequirements.Validate()

print(">> Updating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

def RegisterEngineEnvVariable():
    variable_name = "ORiginEngine"
    variable_path = os.getcwd()
    if os.environ.get(variable_name) is None:
        print(">> Registering Origin Engine path...")
        if platform.system() == "Windows":
            Utils.SetWindowsSystemEnvironmentVariable(variable_name, variable_path)
        elif platform.system() == "Linux":
            Utils.SetLinuxSystemEnvironmentVariable(variable_name, variable_path)

    print(f">> Engine path is registered to {variable_path} as {variable_name}")


# Windows Only
def RegisterMSBuild():
    vswherePath = "./Scripts/vswhere/vswhere.exe"
    if not os.path.exists(vswherePath):
        print(">> Downloading VSWhere")
        vswhereLink = "https://github.com/microsoft/vswhere/releases/download/3.1.7/vswhere.exe"
        Utils.DownloadFile(vswhereLink, vswherePath)

    visual_studio_path = Utils.FindVisualStudioPath(vswherePath)
    variable_path = f"{visual_studio_path}\\MSBuild\\Current\\Bin"
    if Utils.AddNewWindowsSystemPathEnvironment(variable_path):
        print(f">> MSBuild path is registered to {variable_path}")


if premakeInstalled:
    RegisterEngineEnvVariable()
    if platform.system() == "Windows":
        print("\n>> Generating OR1.sln...")
        premake_path = os.path.abspath("./Scripts/premake5.bat")
        subprocess.call([premake_path, "nopause"])
        RegisterMSBuild()

    elif platform.system() == "Linux":
        print("\n>> Generating GNU Makefiles...")
        premake_path = os.path.abspath("./Scripts/premake5.sh")
        os.chmod(premake_path, 0o755)
        subprocess.call([premake_path])
    else:
        print(">> Unsupported platform.")

    print(">> Setup Completed!")
else:
    print(">> ORigin requires Premake to generate project file.")
