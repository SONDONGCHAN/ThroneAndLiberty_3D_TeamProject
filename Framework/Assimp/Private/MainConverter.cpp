#include "MainConverter.h"
#include "GameInstance.h"

#include <filesystem>
#include "AssimpModel.h"
#include <string>
#include <string.h>

CMainConverter::CMainConverter()
{
}

CMainConverter::~CMainConverter()
{
}

HRESULT CMainConverter::Initialize()
{
    GRAPHIC_DESC GraphicDesc = {};

    GraphicDesc.hWnd = g_hWnd;
    GraphicDesc.isWindowed = true;
    GraphicDesc.iWinSizeX = g_iWinSizeX;
    GraphicDesc.iWinSizeY = g_iWinSizeY;

  //  if (FAILED(CGameInstance::GetInstance()->InitializeEngine(g_hInst, 1, GraphicDesc)))
   //     return E_FAIL;

  //  CGameInstance::GetInstance()->LoadLevelResource(0);
    FindFBXFile(m_strBasePath);
    return S_OK;
}

void CMainConverter::Tick(_float _fTimeDelta)
{
   // CGameInstance::GetInstance()->TickEngine(_fTimeDelta);
}

HRESULT CMainConverter::Render()
{
    return S_OK;
}

void CMainConverter::FindFBXFile(wstring _strFilePath)
{
    filesystem::path basePath(_strFilePath);

    for (const auto& entry : filesystem::directory_iterator(basePath)) {
        /*if. File : .png 라면 texture Create*/
        if (entry.is_regular_file()) {
            if (".fbx" == entry.path().extension()) {
                wstring strFileName = entry.path().filename().stem().wstring();
                wstring strSaveName = strFileName + TEXT(".dat");


                wstring strSavePath2 = MakeSavePath(entry.path().wstring(), entry.path().filename().wstring());

                strSavePath2 += strSaveName;
                /*애님 논애님 결정*/
                string strParentName = entry.path().parent_path().filename().string();
                _uint iTypeIdx = CheckModelType(entry.path().string());

                /*저장 경로 결정*/
                CAssimpModel::Create(entry.path().string(), iTypeIdx, strSavePath2, strFileName);

                /*추출한 파일은 자동으로 삭제*/
                filesystem::remove(entry.path());

                string strParentPath = entry.path().parent_path().filename().string();


                if (entry.path().parent_path().empty() && (strParentPath != "Anim" && strParentPath != "NonAnim")) {

                    filesystem::remove(entry.path().parent_path());

                }
            }
        }
        /*if. Directory : 계속 다음 폴더를 순회*/
        else if (entry.is_directory()) {
            FindFBXFile(entry.path());
        }
    }


}

_uint CMainConverter::CheckModelType(string _strPath)
{

    size_t iPos = _strPath.find("NonAnim");

    _uint iTypeIdx = 1;

    /*Anim*/
    if (iPos == -1) {
        iTypeIdx = 0;
    }

    return iTypeIdx;
}

wstring CMainConverter::MakeSavePath(wstring _strPath, wstring _strFileName)
{   
    /*파일명과 기본 BasePath를 잘라서 Client 리소스 폴더에서 저장해야하는 경로를 지정해준다*/
    size_t pos = _strPath.find(m_strBasePath);
    _strPath.erase(pos, m_strBasePath.length());
    size_t pos2 = _strPath.find(_strFileName);
    _strPath.erase(pos2, _strFileName.length());

    wstring strCompletePath{};
    if (wstring::npos !=_strPath.find(L"Level_MapTool"))
        strCompletePath = strMapToolBase + _strPath;
    else
        strCompletePath = strClientBase + _strPath;

    /*폴더가 없다면 미리 생성*/
    auto ret = filesystem::create_directories(strCompletePath);

    return strCompletePath;
}

void CMainConverter::Free()
{
   // CGameInstance::DestroyInstance();
}
