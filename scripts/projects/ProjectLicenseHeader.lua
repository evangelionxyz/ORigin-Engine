-- Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

os.chdir("../../")
defaultArgument =
[[extensions: designer.cs generated.cs
extensions: .cs .cpp .h .glsl .hlsl
// Copyright (c) %CurrentYear% Evangelion Manuhutu | ORigin Engine

extensions: .aspx .ascx
<%-- Copyright (c) %CurrentYear% Evangelion Manuhutu | ORigin Engine--%>

extensions: .py
# Copyright (c) %CurrentYear% Evangelion Manuhutu | ORigin Engine

extensions: .lua
-- Copyright (c) %CurrentYear% Evangelion Manuhutu | ORigin Engine]]

-- checking file available
if(os.isfile("ORigin.sln.licenseheader"))
then
    if(io.readfile("ORigin.sln.licenseheader") ~= defaultArgument)
    then
        print("\nCreating new project ORigin.sln.licenseheader file...")
        io.writefile("ORigin.sln.licenseheader", defaultArgument)
    end
else
    io.writefile("ORigin.sln.licenseheader", defaultArgument)
end