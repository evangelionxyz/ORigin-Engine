import os
import subprocess
import platform
import Utils

os.chdir('./../')
from SetupPython import PythonConfiguration as PythonRequirements
PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements
VulkanRequirements.Validate()

from SetupPremake import PremakeConfiguration as PremakeRequirements
premakeInstalled = PremakeRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])


def RegisterEngineVariable():
    variable_name = "ORiginEngine"
    if os.environ.get(variable_name) is None:
      print("Registering Origin Engine path...")
      variable_path = os.getcwd()
      Utils.SetSystemEnvironmentVariable(variable_name, variable_path)
      print(f"Engine path is registered to {variable_path} as {variable_name}")


def RegisterMSBuild():
    visual_studio_path = Utils.FindVisualStudioPath("Scripts/vswhere/vswhere.exe")
    variable_path = f"{visual_studio_path}\\MSBuild\\Current\\Bin"
    if Utils.AddNewSystemPathEnvironment(variable_path):
        print(f"MSBuild path is registered to {variable_path}")


if premakeInstalled:
    if platform.system() == "Windows":
        print("\nGenerating OR1.sln...")
        origin_sln = os.path.abspath("./Scripts/premake5.bat")
        subprocess.call([origin_sln, "nopause"])

        RegisterEngineVariable()
        RegisterMSBuild()

    print("Setup Completed!")
else:
    print("ORigin requires Premake to generate project file.")
