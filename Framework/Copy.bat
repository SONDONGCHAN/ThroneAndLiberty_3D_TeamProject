// 명령어			옵션				원본 파일이 있는 위치				사본 파일을 저장할 위치

xcopy				/y/s				.\Engine\Public\*					.\Reference\Headers\
xcopy				/y					.\Engine\Bin\Engine.lib				.\Reference\Librarys\
xcopy				/y					.\Engine\ThirdPartyLib\*.lib		.\Reference\Librarys\
xcopy				/y					.\Engine\ThirdPartyLib\*\*.lib		.\Reference\Librarys\

xcopy				/y					.\Engine\Bin\*.dll					.\Client\Bin\
xcopy				/y					.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Assimp\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Assimp\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Assimp\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Assimp\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Tool_Map\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Tool_Map\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Tool_Map\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Tool_Map\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Tool_UI\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Tool_UI\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Tool_UI\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Tool_UI\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Tool_Effect\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Tool_Effect\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Tool_Effect\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Tool_Effect\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Tool_Animation\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Tool_Animation\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Tool_Animation\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Tool_Animation\Bin\ShaderFiles\

xcopy				/y					.\Engine\Bin\Engine.dll				.\Tool_CC\Bin\
xcopy				/y					.\Engine\Bin\jsoncpp.dll			.\Tool_CC\Bin\
xcopy				/y					.\Engine\Bin\fmod.dll				.\Tool_CC\Bin\
xcopy				/y					.\Engine\Bin\fmodL.dll				.\Assimp\Bin\
xcopy				/y					.\Client\Bin\ShaderFiles\*.*		.\Tool_CC\Bin\ShaderFiles\