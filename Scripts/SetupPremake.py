import os
import platform

from pathlib import Path

import Utils

class PremakeConfiguration:
    premakeVersion = "5.0.0-beta2"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "Scripts/premake"
    userPlatform = platform.system()

    if(userPlatform == "Windows"):
        premakeArchiveUrls = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip"
        
    elif(platform.system() == "Linux"):
        premakeArchiveUrls = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-linux.tar.gz"

    @classmethod
    def Validate(cls):
        if (not cls.CheckIfPremakeInstalled()):
            print(">> Premake is not installed.")
            return False
        print(f">> Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")
        return True

    @classmethod
    def CheckIfPremakeInstalled(cls):
        if (cls.userPlatform == "Windows"):
            premakeExe = Path(f"{cls.premakeDirectory}/premake5.exe")
        elif (cls.userPlatform == "Linux"):
            premakeExe = Path(f"{cls.premakeDirectory}/premake5")
        
        if not premakeExe.exists():
            return cls.InstallPremake()

        return True

    @classmethod
    def InstallPremake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input(">> Premake not found.\nWould you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        if(cls.userPlatform == "Windows"):
            premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-windows.zip"
        elif (cls.userPlatform == "Linux"):
            premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-linux.tar.gz"
            
        print("Downloading {0:s} to {1:s}".format(cls.premakeArchiveUrls, premakePath))
        Utils.DownloadFile(cls.premakeArchiveUrls, premakePath)
        print("Extracting", premakePath)
        Utils.ExtractArchiveFile(premakePath, deleteArchiveFile=True)
        print(f">> Premake {cls.premakeVersion} has been downloaded to '{cls.premakeDirectory}'")

        premakeLicensePath = f"{cls.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.DownloadFile(cls.premakeLicenseUrl, premakeLicensePath)
        print(f">> Premake License file has been downloaded to '{cls.premakeDirectory}'")
        return True