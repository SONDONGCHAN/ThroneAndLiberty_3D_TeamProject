// 명령어	옵션			원본 파일이 있는 위치			사본 파일을 저장할 위치

del /q ..\..\..\Client\Bin\DataFiles\MapData\*
del /q ..\..\..\Client\Bin\Resources\Level_Dungeon\Textures\Terrain\*

del /q ..\..\..\Client\Bin\Resources\Level_Dungeon\Models\ModelTexture\Environment\*
del /q ..\..\..\Client\Bin\Resources\Level_Dungeon\Models\NonAnim\Environment\*

xcopy		/y/s		.\Map\*						..\..\..\Client\Bin\DataFiles\MapData\
xcopy		/y/s		.\TerrainTexture\*			..\..\..\Client\Bin\Resources\Level_Dungeon\Textures\Terrain\
xcopy		/y/s		.\ModelTexture\*			..\..\..\Client\Bin\Resources\Level_Dungeon\Models\ModelTexture\Environment\
xcopy		/y/s		.\Model\*					..\..\..\Client\Bin\Resources\Level_Dungeon\Models\NonAnim\Environment\