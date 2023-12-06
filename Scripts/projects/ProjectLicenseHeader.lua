-- Copyright (c) Evangelion Manuhutu | ORigin Engine

os.chdir("../../")
defaultArgument =
[[extensions: designer.cs generated.cs
extensions: .cs .cpp .h .glsl .hlsl
// Copyright (c) Evangelion Manuhutu | ORigin Engine

extensions: .aspx .ascx
<%-- Copyright (c) Evangelion Manuhutu | ORigin Engine--%>

extensions: .py
# Copyright (c) Evangelion Manuhutu | ORigin Engine

extensions: .lua
-- Copyright (c) Evangelion Manuhutu | ORigin Engine]]

-- checking file available
if(os.isfile("OR1.sln.licenseheader"))
then
    if(io.readfile("OR1.sln.licenseheader") ~= defaultArgument)
    then
        print("\nCreating new project OR1.sln.licenseheader file...")
        io.writefile("OR1.sln.licenseheader", defaultArgument)
    end
else
    io.writefile("OR1.sln.licenseheader", defaultArgument)
end