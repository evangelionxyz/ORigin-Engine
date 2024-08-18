import os
import platform
import Utils
import subprocess
from pathlib import Path

class VulkanConfiguration:
    requiredVulkanVersion = "1.3"
    installVulkanVersion = "1.3.261.1"
    vulkanDirectory = "./ORigin/vendor/VulkanSDK"

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
            vulkanSDK = os.environ.get("VULKAN_SDK")
            if vulkanSDK is None:
                print(">> You don't have the Vulkan SDK installed!")
                cls.__InstallVulkanSDK()
                return False
            else:
                print(f">> Located Vulkan SDK at {vulkanSDK}")
                if (cls.requiredVulkanVersion not in vulkanSDK):
                    print(f">> You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
                    cls.__InstallVulkanSDK()
                    return False
        # elif platform.system() == "Linux":
        #     vulkanSDK = os.path.abspath(f"ORigin/vendor/VulkanSDK/{cls.installVulkanVersion}/vulkansdk")
        #     if not os.path.exists(vulkanSDK):
        #         print(">> You don't have the Vulkan SDK installed!")
        #         cls.__InstallVulkanSDK()
        #         return False
        #     else:
        #         print(f">> Located Vulkan SDK at {vulkanSDK}")
        #         if (cls.requiredVulkanVersion not in vulkanSDK):
        #             print(f">> You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
        #             cls.__InstallVulkanSDK()
        #             return False


        print(f">> Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def __InstallVulkanSDK(cls):
        # [Windows] https://sdk.lunarg.com/sdk/download/1.3.261.1/windows/VulkanSDK-1.3.261.1-Installer.exe
        # [Linux  ] https://sdk.lunarg.com/sdk/download/1.3.261.1/linux/vulkansdk-linux-x86_64-1.3.261.1.tar.xz

        if platform.system() == "Window":
            vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
            vulkanDownloadFilepath = f"{cls.vulkanDirectory}/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
        # elif platform.system() == "Linux":
        #     vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/linux/vulkansdk-linux-x86_64-{cls.installVulkanVersion}.tar.xz"
        #     vulkanDownloadFilepath = f"{cls.vulkanDirectory}/VulkanSDK-{cls.installVulkanVersion}.tar.xz"


        # check if not downloaded
        if not os.path.exists(vulkanDownloadFilepath):
            permissionGranted = False
            while not permissionGranted:
                reply = str(input(">> Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
                if reply == 'n':
                    return
                permissionGranted = (reply == 'y')
            
            print("Downloading {0:s} to {1:s}".format("Vulkan SDK", vulkanDownloadFilepath))
            print("Please Wait...")
            Utils.DownloadFile(vulkanInstallURL, vulkanDownloadFilepath)
        else:
            print(">> Vulkan already downloaded.")

        # execute
        if platform.system() == "Windows":
            print(">> Running Vulkan SDK installer...")
            os.startfile(os.path.abspath(vulkanDownloadFilepath))
            print(">> Re-run this script after installation!")
        # elif platform.system() == "Linux":
        #     Utils.ExtractArchiveFile(vulkanDownloadFilepath, deleteArchiveFile = False)
        #     print(">> Re-run this script after completely extracted !")
            
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        if platform.system() == "Windows":
            vulkanSDK = os.environ.get("VULKAN_SDK")
            shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
            return shadercdLib.exists()
        # elif platform.system() == "Linux":
        #     shadercdLib = "ORigin/vendor/VulkanSDK/x86_64//lib/libshaderc_sharedd.a"
        #     # TODO: Check the debug lib
        #     # return shadercdLib.exists()
        #     return False

if __name__ == "__main__":
    VulkanConfiguration.Validate()
