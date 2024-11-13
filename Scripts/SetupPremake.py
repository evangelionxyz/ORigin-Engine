import os
import platform
from pathlib import Path
import Utils

class PremakeConfiguration:
    premake_version = "5.0.0-beta2"
    premake_license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premake_directory = os.path.abspath("Scripts/premake")
    user_platform = platform.system()

    if(user_platform == "Windows"):
        premakeArchiveUrls = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-windows.zip"
        
    elif(platform.system() == "Linux"):
        premakeArchiveUrls = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-linux.tar.gz"

    @classmethod
    def Validate(cls):
        if (not cls.is_premake_installed()):
            print(">> Premake is not installed.")
            return False
        return True

    @classmethod
    def is_premake_installed(cls):
        if (cls.user_platform == "Windows"):
            premake_exe = Path(f"{cls.premake_directory}/premake5.exe")
            print(premake_exe)
        elif (cls.user_platform == "Linux"):
            premake_exe = Path(f"{cls.premake_directory}/premake5")
        
        if not premake_exe.exists():
            return cls.install_premake()

        return True

    @classmethod
    def install_premake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input(">> Premake not found.\nWould you like to download Premake {0:s}? [Y/N]: ".format(cls.premake_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        if(cls.user_platform == "Windows"):
            premakePath = f"{cls.premake_directory}/premake-{cls.premake_version}-windows.zip"
        elif (cls.user_platform == "Linux"):
            premakePath = f"{cls.premake_directory}/premake-{cls.premake_version}-linux.tar.gz"
            
        print("Downloading {0:s} to {1:s}".format(cls.premakeArchiveUrls, premakePath))
        Utils.download_file(cls.premakeArchiveUrls, premakePath)
        print("Extracting", premakePath)
        Utils.ExtractArchiveFile(premakePath, deleteArchiveFile=True)
        print(f">> Premake {cls.premake_version} has been downloaded to '{cls.premake_directory}'")

        premakeLicensePath = f"{cls.premake_directory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premake_license_url, premakeLicensePath))
        Utils.download_file(cls.premake_license_url, premakeLicensePath)
        print(f">> Premake License file has been downloaded to '{cls.premake_directory}'")
        return True