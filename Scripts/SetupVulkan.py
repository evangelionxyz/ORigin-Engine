import os
import platform
import Utils
import subprocess
from pathlib import Path

class VulkanConfiguration:
    vulkan_major_minor_version = "1.3"
    vulakn_version = "1.3.296.0"
    vulkan_directory = os.path.abspath("Engine/ThirdParty/VulkanSDK")

    @classmethod
    def Validate(cls):
        if not cls.CheckVulkanSDK():
            print(">> Vulkan SDK not installed correctly.")
            return

        if not cls.CheckVulkanSDKDebugLibs():
            print(">> No Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print(">> Debug configuration disabled.")

    @classmethod
    def CheckVulkanSDK(cls):
        if platform.system() == "Windows":
            vulkan_sdk_env = os.environ.get("VULKAN_SDK")
            if vulkan_sdk_env is None:
                print(">> You don't have the Vulkan SDK installed!")
                cls.InstallVulkanSDK()
                return False
            else:
                print(f">> Located Vulkan SDK at {vulkan_sdk_env}")
                if (cls.vulkan_major_minor_version not in vulkan_sdk_env):
                    print(f">> You don't have the correct Vulkan SDK version! (Engine requires {cls.vulkan_major_minor_version})")
                    cls.InstallVulkanSDK()
                    return False
        print(f">> Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def InstallVulkanSDK(cls):
        vulkan_download_url = f"https://sdk.lunarg.com/sdk/download/{cls.vulakn_version}/windows/VulkanSDK-{cls.vulakn_version}-Installer.exe"
        vulkan_download_path = f"{cls.vulkan_directory}/VulkanSDK-{cls.vulakn_version}-Installer.exe"
        if not os.path.exists(vulkan_download_path):
            permissionGranted = False
            while not permissionGranted:
                reply = str(input(">> Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.vulakn_version))).lower().strip()[:1]
                if reply == 'n':
                    return
                permissionGranted = (reply == 'y')
            
            print("Downloading {0:s} to {1:s}".format("Vulkan SDK", vulkan_download_path))
            print("Please Wait...")
            Utils.DownloadFile(vulkan_download_url, vulkan_download_path)
        else:
            print(">> Vulkan already downloaded.")

        if platform.system() == "Windows":
            print(">> Running Vulkan SDK installer...")
            os.startfile(os.path.abspath(vulkan_download_path))
            print(">> Re-run this script after installation!")
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkan_sdk = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkan_sdk}/Lib/shaderc_sharedd.lib")
        return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()
