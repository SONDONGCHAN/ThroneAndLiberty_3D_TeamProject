#include "JsonParsingMgr.h"

#include <fstream>

CJsonParsingMgr::CJsonParsingMgr()
{
}

void CJsonParsingMgr::WriteJson(Json::Value _value, const wstring& _strSavePath)
{
    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

    std::ofstream ofStream;
    ofStream.open(_strSavePath, ios_base::out);

    if (ofStream.is_open()) {
        writer->write(_value, &ofStream);
    }
    else {
        return;
    }

    ofStream.close();


}

Json::Value CJsonParsingMgr::ReadJson(const wstring& _strReadPath)
{
    Json::Value root;

    if (TEXT("") == _strReadPath)
        return root;

    ifstream fin(_strReadPath);

    if(fin.is_open()) {
        fin >> root;
    }

    return root;
}

shared_ptr<CJsonParsingMgr> CJsonParsingMgr::Create()
{
    return make_shared<CJsonParsingMgr>();
}
