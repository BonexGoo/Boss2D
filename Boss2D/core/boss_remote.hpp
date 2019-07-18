#pragma once
#include <boss_array.hpp>
#include <boss_string.hpp>
#include <boss_wstring.hpp>

class RemoteClass;

namespace BOSS
{
    BOSS_DECLARE_ID(id_remote);

    //! \brief 원격함수(RMI)지원
    class Remote
    {
    public:
        class Method;
        typedef Array<const Method*, datatype_pod_canmemcpy_zeroset> MethodPtrs;

    public:
        /*!
        \brief 접속하기(서버)
        \param port : 포트번호
        \param methods : 원격함수들
        \return 생성된 원격ID
        \see Disconnect
        */
        static id_remote ConnectForServer(uint16 port, MethodPtrs methods);

        /*!
        \brief 접속하기(클라이언트)
        \param domain : 도메인명(IP가능)
        \param port : 포트번호
        \param methods : 원격함수들
        \return 생성된 원격ID
        \see Disconnect
        */
        static id_remote ConnectForClient(chars domain, uint16 port, MethodPtrs methods);

        /*!
        \brief 접속끊기
        \param remote : 원격ID
        \see ConnectForServer, ConnectForClient
        */
        static void Disconnect(id_remote_read remote);

        /*!
        \brief 연결 및 송수신과정을 진행
        \param remote : 원격ID
        \return 상대방과 연결되었는지 여부
        */
        static bool CommunicateOnce(id_remote_read remote);

        /*!
        \brief 마지막 접속시도시간 확인
        \param remote : 원격ID
        \return 마지막 접속이후 연결시도한 시간(현재 접속되어 있다면 0)
        */
        static uint64 GetLastConnectingTime(id_remote_read remote);

        /*!
        \brief 새로운 상대방과 연결되었는지 확인
        \param remote : 원격ID
        \return 연결여부(1회성, 함수호출후에는 새로운 상대방이 아니게 됨)
        */
        static bool CheckNewPartner(id_remote remote);

    public:
        class Param
        {
        public:
            void Store(id_cloned_share rhs);
            id_cloned_share Drain() const;
            bool WhatTypeInt() const;
            bool WhatTypeDec() const;
            bool WhatTypeHex() const;
            bool WhatTypeStr() const;

        public:
            Param();
            ~Param();
            Param& operator=(const Param&);

        private:
            mutable id_cloned_share m_share;
        };
        typedef Array<Param, datatype_pod_canmemcpy, 0> Params;
        typedef Object<sint64, datatype_pod_canmemcpy> IntParam;
        typedef Object<double, datatype_pod_canmemcpy> DecParam;
        typedef Array<uint08, datatype_pod_canmemcpy> HexParam;
        typedef String StrParam;

    public:
        class Method
        {
        public:
            typedef void (*GlueCB)
            (
                payload data,
                const Remote::Params& params,
                id_cloned_share* ret,
                bool localcall
            );
            typedef void (*GlueSubCB)
            (
                payload data,
                const Remote::Param* params,
                sint32 length,
                id_cloned_share* ret
            );

        public:
            Method& operator=(const Method& rhs);
            const Method operator()() const;
            const Method operator()(sint32 value) const;
            const Method operator()(sint64 value) const;
            const Method operator()(float value) const;
            const Method operator()(double value) const;
            const Method operator()(bytes value, sint32 length) const;
            const Method operator()(chars value, sint32 length = -1) const;
            const Method operator()(wchars value, sint32 length = -1) const;
            const Method operator()(id_cloned_share value) const;
            operator id_cloned_share() const;

        public:
            Method();
            Method(const Method& rhs);
            Method(chars name, GlueCB cb = nullptr, payload data = nullptr);
            ~Method();

        private:
            Method(const Method* parent, id_cloned_share value);

        public:
            void SetRemote(id_remote_read remote) const;
            void AddParam(id_cloned_share value) const;
            void CallFlush(id_cloned_share* out) const;
            void LockForResult() const;
            void SendResult(id_cloned_share value) const;
            id_cloned_share UnlockForResult() const;
            void DirectCallOnce() const;

        public:
            void LocalCallOnce() const;

        private:
            enum CallType {CT_NeedRemote, CT_NeedLocal, CT_DirectForRemote};

        public:
            const bool m_calling;
            union
            {
                struct
                {
                    String* m_name;
                    mutable id_remote_read m_remote;
                    GlueCB m_cb;
                    payload m_data;
                    mutable id_cloned_share m_result;
                    mutable id_mutex m_resultMutex;
                } func;
                struct
                {
                    const Method* m_parent;
                    mutable bool m_needCall;
                    mutable CallType m_callType;
                    mutable Params* m_params;
                } call;
            };
        };
    };
}
