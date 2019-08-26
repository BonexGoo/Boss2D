#pragma once
#include <boss_view.hpp>

namespace BOSS
{
    BOSS_DECLARE_ID(id_acc);
    BOSS_DECLARE_ID(id_alpr);
    BOSS_DECLARE_ID(id_curl);
    BOSS_DECLARE_ID(id_dlib);
    BOSS_DECLARE_ID(id_freetype);
    BOSS_DECLARE_ID(id_git);
    BOSS_DECLARE_ID(id_h264);
    BOSS_DECLARE_ID(id_jpg);
    BOSS_DECLARE_ID(id_md5);
    BOSS_DECLARE_ID(id_opencv);
    BOSS_DECLARE_ID(id_tesseract);
    BOSS_DECLARE_ID(id_tif);
    BOSS_DECLARE_ID(id_webrtc);
    BOSS_DECLARE_ID(id_websocket);
    BOSS_DECLARE_ID(id_zip);

    //! \brief 애드온지원
    class AddOn
    {
    public:
        //! \brief AAC연동
        class Aac
        {
        public:
            static id_acc Create(sint32 bitrate, sint32 channel, sint32 samplerate);
            static void Release(id_acc acc);
            static void EncodeTo(id_acc acc, bytes pcm, sint32 length, id_flash flash, uint64 timems);
            static void SilenceTo(id_acc acc, id_flash flash, uint64 timems);
        };

        //! \brief ALPR연동
        class Alpr
        {
        public:
            static id_alpr Create(chars country = "", h_view listener = h_view::null());
            static void Release(id_alpr alpr);
            static chars Recognize(id_alpr alpr, id_bitmap_read bitmap);
            static chars Summary(chars recognizedtext);
        };

        //! \brief CURL연동
        class Curl
        {
        public:
            typedef size_t (*CurlReadCB)(void* ptr, size_t size, size_t nitems, payload data);
            typedef void (*SearchCB)(payload data, chars name, sint32 size, id_clock clock);
            enum SendType {ST_NoSend, ST_Put, ST_Post};

        public:
            static id_curl Create(sint32 timeout = -1);
            static id_curl CreateForUser(chars username, chars password, sint32 timeout = -1);
            static id_curl Clone(id_curl curl);
            static void Release(id_curl curl);
            static chars GetString(id_curl curl, chars url,
                chars headerdata = nullptr, SendType sendtype = ST_NoSend, chars senddata = nullptr, sint32 datalen = -1);
            static bytes GetBytes(id_curl curl, chars url, sint32* getsize,
                chars headerdata = nullptr, SendType sendtype = ST_NoSend, chars senddata = nullptr, sint32 datalen = -1);
            static chars GetRedirectUrl(id_curl curl, chars url, sint32 successcode,
                chars headerdata = nullptr, SendType sendtype = ST_NoSend, chars senddata = nullptr, sint32 datalen = -1);
            static void SendStream(id_curl curl, chars url, CurlReadCB cb, payload data);
            static bool FtpUpload(id_curl curl, chars url, chars filename, buffer data);
            static buffer FtpDownload(id_curl curl, chars url, chars filename);
            static bool FtpDelete(id_curl curl, chars url, chars filename);
            static bool FtpCreateFolder(id_curl curl, chars url, chars dirname);
            static bool FtpDeleteFolder(id_curl curl, chars url, chars dirname);
            static sint32 FtpSearch(id_curl curl, chars url, chars dirname, SearchCB cb, payload data);
        };

        //! \brief DLIB연동
        class Dlib
        {
        public:
            enum FaceLandmark68Type {
                FLT_LeftEye_6_Dots, FLT_RightEye_6_Dots,
                FLT_LeftEyeBrow_5_Dots, FLT_RightEyeBrow_5_Dots,
                FLT_Nose_9_Dots, FLT_Lips_20_Dots, FLT_Jaw_17_Dots};

        public:
            static id_dlib Create(void);
            static void Release(id_dlib dlib);
            static void Update(id_dlib dlib, id_bitmap_read bmp);
            static const point64* GetFaceLandmark(id_dlib dlib, FaceLandmark68Type type);
        };

        //! \brief FREETYPE연동
        class FreeType
        {
        public:
            static id_freetype Create(buffer ttf, chars nickname);
            static id_freetype_read Get(chars nickname);
            static void Release(id_freetype freetype);
            static id_bitmap ToBmp(id_freetype_read freetype, sint32 height, uint32 code);
            static void GetInfo(id_freetype_read freetype, sint32 height, uint32 code, sint32* width = nullptr, sint32* ascent = nullptr);
        };

        //! \brief GIT연동
        class Git
        {
        public:
            enum GitProgressType {GPT_DownloadPack, GPT_DownloadTip, GPT_InflateFile};
            typedef void (*ProgressCB)(payload, GitProgressType, chars);

        public:
            static id_git Create(chars rootpath, chars sshname, chars id, chars password);
            static void Release(id_git git);
            static void Update(id_git git, ProgressCB cb = nullptr, payload data = nullptr);
        };

        //! \brief H264연동
        class H264
        {
        public:
            static id_h264 CreateEncoder(sint32 width, sint32 height, bool fastmode);
            static id_h264 CreateDecoder(void);
            static void Release(id_h264 h264);
            static void EncodeOnce(id_h264 h264, const uint32* rgba, id_flash flash, uint64 timems);
            static id_bitmap DecodeBitmapOnce(id_h264 h264, id_flash flash, uint64 settimems = 0, uint64* gettimems = nullptr);
            static id_texture DecodeTextureOnce(id_h264 h264, id_flash flash, uint64 settimems = 0, uint64* gettimems = nullptr);
            static void DecodeSeek(id_h264 h264, id_flash flash, uint64 timems);
        };

        //! \brief JPG연동
        class Jpg
        {
        public:
            static id_jpg Create(id_bitmap_read bmp, sint32 quality = 100);
            static void Release(id_jpg jpg);
            static sint32 GetLength(id_jpg jpg);
            static bytes GetBits(id_jpg jpg);
            static id_bitmap ToBmp(bytes jpg, sint32 length);
        };

        //! \brief OGG연동
        class Ogg
        {
        public:
            typedef void (*PcmCreateCB)(sint32 channel, sint32 sample_rate, sint32 sample_size, payload data);
            typedef sint32 (*PcmWriteCB)(void* ptr, size_t size, payload data);

        public:
            static void ToPcmStream(id_file_read oggfile, PcmCreateCB ccb, PcmWriteCB wcb, payload data);
        };

        //! \brief OpenCV연동
        class OpenCV
        {
        public:
            typedef void (*FindContoursCB)(sint32 count, const point64* dots, payload data);
            typedef void (*HoughLinesCB)(const point64f dot1, const point64f dot2, payload data);
            typedef void (*HoughCirclesCB)(const point64f dot, const float radius, payload data);

        public:
            static id_opencv Create(void);
            static void Release(id_opencv opencv);
            static void SetMOG2(id_opencv opencv, bool enable, sint32 history = 500, double threshold = 16, bool shadows = true);
            static void SetCanny(id_opencv opencv, bool enable, double low = 125, double high = 125 * 3, sint32 aperture = 3);
            static void Update(id_opencv opencv, id_bitmap_read bmp);
            static id_bitmap GetUpdatedImage(id_opencv opencv);
            static void GetFindContours(id_opencv opencv, FindContoursCB cb, payload data = nullptr);
            static void GetHoughLines(id_opencv opencv, HoughLinesCB cb, payload data = nullptr);
            static void GetHoughCircles(id_opencv opencv, HoughCirclesCB cb, payload data = nullptr);
        };

        //! \brief SSL연동
        class Ssl
        {
        public:
            static id_md5 CreateMD5(void);
            static chars ReleaseMD5(id_md5 md5);
            static void UpdateMD5(id_md5 md5, bytes binary, sint32 length);
            static chars ToMD5(bytes binary, sint32 length);
            static chars ToSHA256(bytes binary, sint32 length, bool base64);
            static chars ToBASE64(bytes binary, sint32 length);
            static buffer FromBASE64(chars base64);
        };

        //! \brief TESSERACT연동
        class Tesseract
        {
        public:
            static id_tesseract Create(chars tifpath, chars otherpath, chars filepath);
            static void Release(id_tesseract tesseract);
            static void Training(id_tesseract tesseract, chars workname);
        };

        //! \brief TIF연동
        class Tif
        {
        public:
            static id_tif Create(id_bitmap_read bmp);
            static void Release(id_tif tif);
            static id_bitmap ToBmp(bytes tif, sint32 length);
        };

        //! \brief WEBRTC연동
        class WebRtc
        {
        public:
            static id_webrtc OpenForOffer(bool audio, bool data);
            static id_webrtc OpenForAnswer(chars offer_sdp);
            static bool BindWithAnswer(id_webrtc offer_webrtc, chars answer_sdp);
            static void Close(id_webrtc webrtc);
            static void SetMute(id_webrtc answer_webrtc, bool on);
            static void SendData(id_webrtc webrtc, bytes data, sint32 len);
        };

        //! \brief WEBSOCKET연동
        class WebSocket
        {
        public:
            static id_websocket Create(chars url);
            static void Release(id_websocket websocket);
            static bool IsConnected(id_websocket websocket);
            static void SendString(id_websocket websocket, chars text);
            static void SendBinary(id_websocket websocket, bytes data, sint32 len);
            static sint32 GetRecvCount(id_websocket websocket);
            static chars RecvStringOnce(id_websocket websocket);
        };

        //! \brief ZIP연동
        class Zip
        {
        public:
            static id_zip Create(bytes zip, sint32 length, sint32* filecount = nullptr, chars password = nullptr);
            static void Release(id_zip zip);
            static buffer ToFile(id_zip zip, sint32 fileindex);
            static chars GetFileInfo(id_zip zip, sint32 fileindex,
                bool* isdir = nullptr, uint64* ctime = nullptr, uint64* mtime = nullptr, uint64* atime = nullptr,
                bool* archive = nullptr, bool* hidden = nullptr, bool* readonly = nullptr, bool* system = nullptr);
        };
    };
}
