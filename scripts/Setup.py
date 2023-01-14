import os
import subprocess
import platform

from SetupPython import PythonConfiguration as PythonRequirements

PythonRequirements.Validate()

from SetupPremake import PremakeConfiguration as PremakeRequirements

from SetupVulkan import VulkanConfiguration as VulkanRequirements
os.chdir('./../')

premakeInstalled = PremakeRequirements.Validate()
VulkanRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if premakeInstalled:
    if platform.system() == "Windows":
        path = os.path.abspath("./scripts/premake5.bat")
        print(f"\nRunning Premake")
        subprocess.call([path, "nopause"])

    print("Setup Completed!")
else:
    print("ORigin requires Premake to generate project file.")