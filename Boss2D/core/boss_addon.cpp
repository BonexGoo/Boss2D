#include <boss.hpp>
#include "boss_addon.hpp"

#include <element/boss_rect.hpp>
#include <element/boss_vector.hpp>

// 링크옵션 /OPT:NOREF가 안되서 임시코드
bool __LINK_ADDON_AAC__();
bool __LINK_ADDON_ALPR__();
bool __LINK_ADDON_CURL__();
bool __LINK_ADDON_DLIB__();
bool __LINK_ADDON_FREETYPE__();
bool __LINK_ADDON_GIT__();
bool __LINK_ADDON_H264__();
bool __LINK_ADDON_JPG__();
bool __LINK_ADDON_OGG__();
bool __LINK_ADDON_OPENCV__();
bool __LINK_ADDON_SSL__();
bool __LINK_ADDON_TESSERACT__();
bool __LINK_ADDON_TIF__();
bool __LINK_ADDON_WEBRTC__();
bool __LINK_ADDON_WEBSOCKET__();
bool __LINK_ADDON_ZIP__();
static bool _ =
    __LINK_ADDON_AAC__() |
    __LINK_ADDON_ALPR__() |
    __LINK_ADDON_CURL__() |
    __LINK_ADDON_DLIB__() |
    __LINK_ADDON_FREETYPE__() |
    __LINK_ADDON_GIT__() |
    __LINK_ADDON_H264__() |
    __LINK_ADDON_JPG__() |
    __LINK_ADDON_OGG__() |
    __LINK_ADDON_OPENCV__() |
    __LINK_ADDON_SSL__() |
    __LINK_ADDON_TESSERACT__() |
    __LINK_ADDON_TIF__() |
    __LINK_ADDON_WEBRTC__() |
    __LINK_ADDON_WEBSOCKET__() |
    __LINK_ADDON_ZIP__();

namespace BOSS
{
    ////////////////////////////////////////////////////////////////////////////////
    static void Aac_Error() {BOSS_ASSERT("Aac애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Aac, Create, id_acc, return nullptr, sint32, sint32, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Aac, Release, void, return, id_acc)
    BOSS_DEFINE_ADDON_FUNCTION(Aac, EncodeTo, void, return, id_acc, bytes, sint32, id_flash, uint64)
    BOSS_DEFINE_ADDON_FUNCTION(Aac, SilenceTo, void, return, id_acc, id_flash, uint64)

    id_acc AddOn::Aac::Create(sint32 bitrate, sint32 channel, sint32 samplerate)
    {return Core_AddOn_Aac_Create()(bitrate, channel, samplerate);}

    void AddOn::Aac::Release(id_acc acc)
    {Core_AddOn_Aac_Release()(acc);}

    void AddOn::Aac::EncodeTo(id_acc acc, bytes pcm, sint32 length, id_flash flash, uint64 timems)
    {Core_AddOn_Aac_EncodeTo()(acc, pcm, length, flash, timems);}

    void AddOn::Aac::SilenceTo(id_acc acc, id_flash flash, uint64 timems)
    {Core_AddOn_Aac_SilenceTo()(acc, flash, timems);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Alpr_Error() {BOSS_ASSERT("Alpr애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Alpr, Create, id_alpr, return nullptr, chars, h_view)
    BOSS_DEFINE_ADDON_FUNCTION(Alpr, Release, void, return, id_alpr)
    BOSS_DEFINE_ADDON_FUNCTION(Alpr, Recognize, chars, return "", id_alpr, id_bitmap_read)
    BOSS_DEFINE_ADDON_FUNCTION(Alpr, Summary, chars, return "", chars)

    id_alpr AddOn::Alpr::Create(chars country, h_view listener)
    {return Core_AddOn_Alpr_Create()(country, listener);}

    void AddOn::Alpr::Release(id_alpr alpr)
    {Core_AddOn_Alpr_Release()(alpr);}

    chars AddOn::Alpr::Recognize(id_alpr alpr, id_bitmap_read bitmap)
    {return Core_AddOn_Alpr_Recognize()(alpr, bitmap);}

    chars AddOn::Alpr::Summary(chars recognizedtext)
    {return Core_AddOn_Alpr_Summary()(recognizedtext);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Curl_Error() {BOSS_ASSERT("Curl애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Curl, Create, id_curl, return nullptr, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, CreateForUser, id_curl, return nullptr, chars, chars, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, Clone, id_curl, return nullptr, id_curl)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, Release, void, return, id_curl)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, GetString, chars, return "", id_curl, chars, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, GetBytes, bytes, return nullptr, id_curl, chars, sint32*, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, GetRedirectUrl, chars, return "", id_curl, chars, sint32, chars, AddOn::Curl::SendType, chars, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, SendStream, void, return, id_curl, chars, AddOn::Curl::CurlReadCB, payload)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpUpload, bool, return false, id_curl, chars, chars, buffer)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpDownload, buffer, return nullptr, id_curl, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpDelete, bool, return false, id_curl, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpCreateFolder, bool, return false, id_curl, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpDeleteFolder, bool, return false, id_curl, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Curl, FtpSearch, sint32, return -1, id_curl, chars, chars, AddOn::Curl::SearchCB, payload)

    id_curl AddOn::Curl::Create(sint32 timeout)
    {return Core_AddOn_Curl_Create()(timeout);}

    id_curl AddOn::Curl::CreateForUser(chars username, chars password, sint32 timeout)
    {return Core_AddOn_Curl_CreateForUser()(username, password, timeout);}

    id_curl AddOn::Curl::Clone(id_curl curl)
    {return Core_AddOn_Curl_Clone()(curl);}

    void AddOn::Curl::Release(id_curl curl)
    {Core_AddOn_Curl_Release()(curl);}

    chars AddOn::Curl::GetString(id_curl curl, chars url, chars headerdata, SendType sendtype, chars senddata, sint32 datalen)
    {return Core_AddOn_Curl_GetString()(curl, url, headerdata, sendtype, senddata, datalen);}

    bytes AddOn::Curl::GetBytes(id_curl curl, chars url, sint32* getsize, chars headerdata, SendType sendtype, chars senddata, sint32 datalen)
    {return Core_AddOn_Curl_GetBytes()(curl, url, getsize, headerdata, sendtype, senddata, datalen);}

    chars AddOn::Curl::GetRedirectUrl(id_curl curl, chars url, sint32 successcode, chars headerdata, SendType sendtype, chars senddata, sint32 datalen)
    {return Core_AddOn_Curl_GetRedirectUrl()(curl, url, successcode, headerdata, sendtype, senddata, datalen);}

    void AddOn::Curl::SendStream(id_curl curl, chars url, CurlReadCB cb, payload data)
    {Core_AddOn_Curl_SendStream()(curl, url, cb, data);}

    bool AddOn::Curl::FtpUpload(id_curl curl, chars url, chars filename, buffer data)
    {return Core_AddOn_Curl_FtpUpload()(curl, url, filename, data);}

    buffer AddOn::Curl::FtpDownload(id_curl curl, chars url, chars filename)
    {return Core_AddOn_Curl_FtpDownload()(curl, url, filename);}

    bool AddOn::Curl::FtpDelete(id_curl curl, chars url, chars filename)
    {return Core_AddOn_Curl_FtpDelete()(curl, url, filename);}

    bool AddOn::Curl::FtpCreateFolder(id_curl curl, chars url, chars dirname)
    {return Core_AddOn_Curl_FtpCreateFolder()(curl, url, dirname);}

    bool AddOn::Curl::FtpDeleteFolder(id_curl curl, chars url, chars dirname)
    {return Core_AddOn_Curl_FtpDeleteFolder()(curl, url, dirname);}

    sint32 AddOn::Curl::FtpSearch(id_curl curl, chars url, chars dirname, SearchCB cb, payload data)
    {return Core_AddOn_Curl_FtpSearch()(curl, url, dirname, cb, data);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Dlib_Error() {BOSS_ASSERT("Dlib애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Dlib, Create, id_dlib, return nullptr, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Dlib, Release, void, return, id_dlib)
    BOSS_DEFINE_ADDON_FUNCTION(Dlib, Update, sint32, return 0, id_dlib, id_bitmap_read)
    BOSS_DEFINE_ADDON_FUNCTION(Dlib, GetFaceLandmark, const point64*, return nullptr, id_dlib, sint32, AddOn::Dlib::FaceLandmark68Type)

    id_dlib AddOn::Dlib::Create(chars trainpath)
    {return Core_AddOn_Dlib_Create()(trainpath);}

    void AddOn::Dlib::Release(id_dlib dlib)
    {Core_AddOn_Dlib_Release()(dlib);}

    sint32 AddOn::Dlib::Update(id_dlib dlib, id_bitmap_read bmp24)
    {return Core_AddOn_Dlib_Update()(dlib, bmp24);}

    const point64* AddOn::Dlib::GetFaceLandmark(id_dlib dlib, sint32 index, FaceLandmark68Type type)
    {return Core_AddOn_Dlib_GetFaceLandmark()(dlib, index, type);}

    ////////////////////////////////////////////////////////////////////////////////
    static void FreeType_Error() {BOSS_ASSERT("FreeType애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(FreeType, Create, id_freetype, return nullptr, buffer, chars)
    BOSS_DEFINE_ADDON_FUNCTION(FreeType, Get, id_freetype_read, return nullptr, chars)
    BOSS_DEFINE_ADDON_FUNCTION(FreeType, Release, void, return, id_freetype)
    BOSS_DEFINE_ADDON_FUNCTION(FreeType, ToBmp, id_bitmap, return nullptr, id_freetype_read, sint32, uint32)
    BOSS_DEFINE_ADDON_FUNCTION(FreeType, GetInfo, void, return, id_freetype_read, sint32, uint32, sint32*, sint32*)

    id_freetype AddOn::FreeType::Create(buffer ttf, chars nickname)
    {return Core_AddOn_FreeType_Create()(ttf, nickname);}

    id_freetype_read AddOn::FreeType::Get(chars nickname)
    {return Core_AddOn_FreeType_Get()(nickname);}

    void AddOn::FreeType::Release(id_freetype freetype)
    {Core_AddOn_FreeType_Release()(freetype);}

    id_bitmap AddOn::FreeType::ToBmp(id_freetype_read freetype, sint32 height, uint32 code)
    {return Core_AddOn_FreeType_ToBmp()(freetype, height, code);}

    void AddOn::FreeType::GetInfo(id_freetype_read freetype, sint32 height, uint32 code, sint32* width, sint32* ascent)
    {Core_AddOn_FreeType_GetInfo()(freetype, height, code, width, ascent);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Git_Error() {BOSS_ASSERT("Git애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Git, Create, id_git, return nullptr, chars, chars, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Git, Release, void, return, id_git)
    BOSS_DEFINE_ADDON_FUNCTION(Git, Update, void, return, id_git, AddOn::Git::ProgressCB, payload)

    id_git AddOn::Git::Create(chars rootpath, chars sshname, chars id, chars password)
    {return Core_AddOn_Git_Create()(rootpath, sshname, id, password);}

    void AddOn::Git::Release(id_git git)
    {Core_AddOn_Git_Release()(git);}

    void AddOn::Git::Update(id_git git, ProgressCB cb, payload data)
    {Core_AddOn_Git_Update()(git, cb, data);}

    ////////////////////////////////////////////////////////////////////////////////
    static void H264_Error() {BOSS_ASSERT("H264애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(H264, CreateEncoder, id_h264, return nullptr, sint32, sint32, bool)
    BOSS_DEFINE_ADDON_FUNCTION(H264, CreateDecoder, id_h264, return nullptr, void)
    BOSS_DEFINE_ADDON_FUNCTION(H264, Release, void, return, id_h264)
    BOSS_DEFINE_ADDON_FUNCTION(H264, EncodeOnce, void, return, id_h264, const uint32*, id_flash, uint64)
    BOSS_DEFINE_ADDON_FUNCTION(H264, DecodeBitmapOnce, id_bitmap, return nullptr, id_h264, id_flash, uint64, uint64*)
    BOSS_DEFINE_ADDON_FUNCTION(H264, DecodeTextureOnce, id_texture, return nullptr, id_h264, id_flash, uint64, uint64*)
    BOSS_DEFINE_ADDON_FUNCTION(H264, DecodeSeek, void, return, id_h264, id_flash, uint64)

    id_h264 AddOn::H264::CreateEncoder(sint32 width, sint32 height, bool fastmode)
    {return Core_AddOn_H264_CreateEncoder()(width, height, fastmode);}

    id_h264 AddOn::H264::CreateDecoder(void)
    {return Core_AddOn_H264_CreateDecoder()();}

    void AddOn::H264::Release(id_h264 h264)
    {Core_AddOn_H264_Release()(h264);}

    void AddOn::H264::EncodeOnce(id_h264 h264, const uint32* rgba, id_flash flash, uint64 timems)
    {Core_AddOn_H264_EncodeOnce()(h264, rgba, flash, timems);}

    id_bitmap AddOn::H264::DecodeBitmapOnce(id_h264 h264, id_flash flash, uint64 settimems, uint64* gettimems)
    {return Core_AddOn_H264_DecodeBitmapOnce()(h264, flash, settimems, gettimems);}

    id_texture AddOn::H264::DecodeTextureOnce(id_h264 h264, id_flash flash, uint64 settimems, uint64* gettimems)
    {return Core_AddOn_H264_DecodeTextureOnce()(h264, flash, settimems, gettimems);}

    void AddOn::H264::DecodeSeek(id_h264 h264, id_flash flash, uint64 timems)
    {Core_AddOn_H264_DecodeSeek()(h264, flash, timems);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Jpg_Error() {BOSS_ASSERT("Jpg애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Jpg, Create, id_jpg, return nullptr, id_bitmap_read, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Jpg, Release, void, return, id_jpg)
    BOSS_DEFINE_ADDON_FUNCTION(Jpg, GetLength, sint32, return 0, id_jpg)
    BOSS_DEFINE_ADDON_FUNCTION(Jpg, GetBits, bytes, return nullptr, id_jpg)
    BOSS_DEFINE_ADDON_FUNCTION(Jpg, ToBmp, id_bitmap, return nullptr, bytes, sint32)

    id_jpg AddOn::Jpg::Create(id_bitmap_read bmp, sint32 quality)
    {return Core_AddOn_Jpg_Create()(bmp, quality);}

    void AddOn::Jpg::Release(id_jpg jpg)
    {Core_AddOn_Jpg_Release()(jpg);}

    sint32 AddOn::Jpg::GetLength(id_jpg jpg)
    {return Core_AddOn_Jpg_GetLength()(jpg);}

    bytes AddOn::Jpg::GetBits(id_jpg jpg)
    {return Core_AddOn_Jpg_GetBits()(jpg);}

    id_bitmap AddOn::Jpg::ToBmp(bytes jpg, sint32 length)
    {return Core_AddOn_Jpg_ToBmp()(jpg, length);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Ogg_Error() {BOSS_ASSERT("Ogg애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Ogg, ToPcmStream, void, return, id_file_read, AddOn::Ogg::PcmCreateCB, AddOn::Ogg::PcmWriteCB, payload)

    void AddOn::Ogg::ToPcmStream(id_file_read oggfile, PcmCreateCB ccb, PcmWriteCB wcb, payload data)
    {return Core_AddOn_Ogg_ToPcmStream()(oggfile, ccb, wcb, data);}

    ////////////////////////////////////////////////////////////////////////////////
    static void OpenCV_Error() {BOSS_ASSERT("OpenCV애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, Create, id_opencv, return nullptr, void)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, Release, void, return, id_opencv)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, SetMOG2, void, return, id_opencv, bool, sint32, double, bool)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, SetCanny, void, return, id_opencv, bool, double, double, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, Update, void, return, id_opencv, id_bitmap_read)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, GetUpdatedImage, id_bitmap, return nullptr, id_opencv)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, GetFindContours, void, return, id_opencv, AddOn::OpenCV::FindContoursCB, payload)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, GetHoughLines, void, return, id_opencv, AddOn::OpenCV::HoughLinesCB, payload)
    BOSS_DEFINE_ADDON_FUNCTION(OpenCV, GetHoughCircles, void, return, id_opencv, AddOn::OpenCV::HoughCirclesCB, payload)

    id_opencv AddOn::OpenCV::Create(void)
    {return Core_AddOn_OpenCV_Create()();}

    void AddOn::OpenCV::Release(id_opencv opencv)
    {Core_AddOn_OpenCV_Release()(opencv);}

    void AddOn::OpenCV::SetMOG2(id_opencv opencv, bool enable, sint32 history, double threshold, bool shadows)
    {Core_AddOn_OpenCV_SetMOG2()(opencv, enable, history, threshold, shadows);}

    void AddOn::OpenCV::SetCanny(id_opencv opencv, bool enable, double low, double high, sint32 aperture)
    {Core_AddOn_OpenCV_SetCanny()(opencv, enable, low, high, aperture);}

    void AddOn::OpenCV::Update(id_opencv opencv, id_bitmap_read bmp)
    {Core_AddOn_OpenCV_Update()(opencv, bmp);}

    id_bitmap AddOn::OpenCV::GetUpdatedImage(id_opencv opencv)
    {return Core_AddOn_OpenCV_GetUpdatedImage()(opencv);}

    void AddOn::OpenCV::GetFindContours(id_opencv opencv, FindContoursCB cb, payload data)
    {Core_AddOn_OpenCV_GetFindContours()(opencv, cb, data);}

    void AddOn::OpenCV::GetHoughLines(id_opencv opencv, HoughLinesCB cb, payload data)
    {Core_AddOn_OpenCV_GetHoughLines()(opencv, cb, data);}

    void AddOn::OpenCV::GetHoughCircles(id_opencv opencv, HoughCirclesCB cb, payload data)
    {Core_AddOn_OpenCV_GetHoughCircles()(opencv, cb, data);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Ssl_Error() {BOSS_ASSERT("Ssl애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, CreateMD5, id_md5, return nullptr, void)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, ReleaseMD5, chars, return "", id_md5)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, UpdateMD5, void, return, id_md5, bytes, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, ToMD5, chars, return "", bytes, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, ToSHA256, chars, return "", bytes, sint32, bool)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, ToBASE64, chars, return "", bytes, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Ssl, FromBASE64, buffer, return nullptr, chars)

    id_md5 AddOn::Ssl::CreateMD5(void)
    {return Core_AddOn_Ssl_CreateMD5()();}

    chars AddOn::Ssl::ReleaseMD5(id_md5 md5)
    {return Core_AddOn_Ssl_ReleaseMD5()(md5);}

    void AddOn::Ssl::UpdateMD5(id_md5 md5, bytes binary, sint32 length)
    {Core_AddOn_Ssl_UpdateMD5()(md5, binary, length);}

    chars AddOn::Ssl::ToMD5(bytes binary, sint32 length)
    {return Core_AddOn_Ssl_ToMD5()(binary, length);}

    chars AddOn::Ssl::ToSHA256(bytes binary, sint32 length, bool base64)
    {return Core_AddOn_Ssl_ToSHA256()(binary, length, base64);}

    chars AddOn::Ssl::ToBASE64(bytes binary, sint32 length)
    {return Core_AddOn_Ssl_ToBASE64()(binary, length);}

    buffer AddOn::Ssl::FromBASE64(chars base64)
    {return Core_AddOn_Ssl_FromBASE64()(base64);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Tesseract_Error() {BOSS_ASSERT("Tesseract애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Tesseract, Create, id_tesseract, return nullptr, chars, chars, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Tesseract, Release, void, return, id_tesseract)
    BOSS_DEFINE_ADDON_FUNCTION(Tesseract, Training, void, return, id_tesseract, chars)

    id_tesseract AddOn::Tesseract::Create(chars tifpath, chars otherpath, chars filepath)
    {return Core_AddOn_Tesseract_Create()(tifpath, otherpath, filepath);}

    void AddOn::Tesseract::Release(id_tesseract tesseract)
    {Core_AddOn_Tesseract_Release()(tesseract);}

    void AddOn::Tesseract::Training(id_tesseract tesseract, chars workname)
    {Core_AddOn_Tesseract_Training()(tesseract, workname);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Tif_Error() {BOSS_ASSERT("Tif애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Tif, Create, id_tif, return nullptr, id_bitmap_read)
    BOSS_DEFINE_ADDON_FUNCTION(Tif, Release, void, return, id_tif)
    BOSS_DEFINE_ADDON_FUNCTION(Tif, ToBmp, id_bitmap, return nullptr, bytes, sint32)

    id_tif AddOn::Tif::Create(id_bitmap_read bmp)
    {return Core_AddOn_Tif_Create()(bmp);}

    void AddOn::Tif::Release(id_tif tif)
    {Core_AddOn_Tif_Release()(tif);}

    id_bitmap AddOn::Tif::ToBmp(bytes tif, sint32 length)
    {return Core_AddOn_Tif_ToBmp()(tif, length);}

    ////////////////////////////////////////////////////////////////////////////////
    static void WebRtc_Error() {BOSS_ASSERT("WebRtc애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, OpenForOffer, id_webrtc, return nullptr, bool, bool)
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, OpenForAnswer, id_webrtc, return nullptr, chars)
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, BindWithAnswer, bool, return false, id_webrtc, chars)
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, Close, void, return, id_webrtc)
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, SetMute, void, return, id_webrtc, bool)
    BOSS_DEFINE_ADDON_FUNCTION(WebRtc, SendData, void, return, id_webrtc, bytes, sint32)

    id_webrtc AddOn::WebRtc::OpenForOffer(bool audio, bool data)
    {return Core_AddOn_WebRtc_OpenForOffer()(audio, data);}

    id_webrtc AddOn::WebRtc::OpenForAnswer(chars offer_sdp)
    {return Core_AddOn_WebRtc_OpenForAnswer()(offer_sdp);}

    bool AddOn::WebRtc::BindWithAnswer(id_webrtc offer_webrtc, chars answer_sdp)
    {return Core_AddOn_WebRtc_BindWithAnswer()(offer_webrtc, answer_sdp);}

    void AddOn::WebRtc::Close(id_webrtc webrtc)
    {Core_AddOn_WebRtc_Close()(webrtc);}

    void AddOn::WebRtc::SetMute(id_webrtc answer_webrtc, bool on)
    {Core_AddOn_WebRtc_SetMute()(answer_webrtc, on);}

    void AddOn::WebRtc::SendData(id_webrtc webrtc, bytes data, sint32 len)
    {Core_AddOn_WebRtc_SendData()(webrtc, data, len);}

    ////////////////////////////////////////////////////////////////////////////////
    static void WebSocket_Error() {BOSS_ASSERT("WebSocket애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, Create, id_websocket, return nullptr, chars)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, Release, void, return, id_websocket)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, IsConnected, bool, return false, id_websocket)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, SendString, void, return, id_websocket, chars)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, SendBinary, void, return, id_websocket, bytes, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, GetRecvCount, sint32, return 0, id_websocket)
    BOSS_DEFINE_ADDON_FUNCTION(WebSocket, RecvStringOnce, chars, return nullptr, id_websocket)

    id_websocket AddOn::WebSocket::Create(chars url)
    {return Core_AddOn_WebSocket_Create()(url);}

    void AddOn::WebSocket::Release(id_websocket websocket)
    {Core_AddOn_WebSocket_Release()(websocket);}

    bool AddOn::WebSocket::IsConnected(id_websocket websocket)
    {return Core_AddOn_WebSocket_IsConnected()(websocket);}

    void AddOn::WebSocket::SendString(id_websocket websocket, chars text)
    {Core_AddOn_WebSocket_SendString()(websocket, text);}

    void AddOn::WebSocket::SendBinary(id_websocket websocket, bytes data, sint32 len)
    {Core_AddOn_WebSocket_SendBinary()(websocket, data, len);}

    sint32 AddOn::WebSocket::GetRecvCount(id_websocket websocket)
    {return Core_AddOn_WebSocket_GetRecvCount()(websocket);}

    chars AddOn::WebSocket::RecvStringOnce(id_websocket websocket)
    {return Core_AddOn_WebSocket_RecvStringOnce()(websocket);}

    ////////////////////////////////////////////////////////////////////////////////
    static void Zip_Error() {BOSS_ASSERT("Zip애드온이 준비되지 않았습니다", false);}
    BOSS_DEFINE_ADDON_FUNCTION(Zip, Create, id_zip, return nullptr, bytes, sint32, sint32*, chars)
    BOSS_DEFINE_ADDON_FUNCTION(Zip, Release, void, return, id_zip)
    BOSS_DEFINE_ADDON_FUNCTION(Zip, ToFile, buffer, return nullptr, id_zip, sint32)
    BOSS_DEFINE_ADDON_FUNCTION(Zip, GetFileInfo, chars, return "", id_zip, sint32,
        bool*, uint64*, uint64*, uint64*, bool*, bool*, bool*, bool*)

    id_zip AddOn::Zip::Create(bytes zip, sint32 length, sint32* filecount, chars password)
    {return Core_AddOn_Zip_Create()(zip, length, filecount, password);}

    void AddOn::Zip::Release(id_zip zip)
    {Core_AddOn_Zip_Release()(zip);}

    buffer AddOn::Zip::ToFile(id_zip zip, sint32 fileindex)
    {return Core_AddOn_Zip_ToFile()(zip, fileindex);}

    chars AddOn::Zip::GetFileInfo(id_zip zip, sint32 fileindex,
        bool* isdir, uint64* ctime, uint64* mtime, uint64* atime,
        bool* archive, bool* hidden, bool* readonly, bool* system)
    {return Core_AddOn_Zip_GetFileInfo()(zip, fileindex,
        isdir, ctime, mtime, atime, archive, hidden, readonly, system);}
}
