#ifndef VIEWER_SAVEFILE_IO
#define VIEWER_SAVEFILE_IO
#include "VizEngineAPIs.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#undef GetObject

// sequence images 관리
// 좋은 방법은 아니지만 일단 savefileio에서 갖고 있게하기 -> import, export 편의를 위해서..
// 1. mi의 파라미터 - 시퀀스 이미지의 연결 관계
// mi의 파라미터는.. VID + _ +파라미터 이름
extern std::map<std::string, int> sequenceIndexByMIParam;

// 2. 해당 시퀀스 이미지 인덱스의 텍스처들 관계
const int SEQ_COUNT = 6;
extern std::vector<vzm::VzTexture*> sequenceTextures[SEQ_COUNT];

namespace savefileIO {

std::string getRelativePath(std::string absolute_path);

void setResPath(std::string assetPath);

void setIBLPath(std::string absIBLPath);

void importSettings(VID root, std::string filePath, vzm::VzRenderer* renderer,
                    vzm::VzScene* scene, vzm::VzLight* sunLight);

void exportSettings(VID root, vzm::VzRenderer* renderer, vzm::VzScene* scene,
                    vzm::VzLight* sunLight);

}  // namespace savefileIO
#endif
