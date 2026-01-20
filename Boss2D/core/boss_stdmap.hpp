#pragma once
#include <boss_stdarray.hpp>

#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <variant>
#include <type_traits>
#include <cstdint>
#include <utility>

namespace BOSS
{
    class StdMapPath
    {
    public:
        const StdMapPath* parent;
        const sint32 value;
    public:
        StdMapPath(const StdMapPath* parent, const sint32 value)
            : parent(parent), value(value) {}
        ~StdMapPath() {}
        std_chararray GetPath() const
        {
            std_chararray collector;
            const StdMapPath* path = this;
            while(path && path->parent)
            {
                char& onechar = collector.AtAdding();
                onechar = (char) path->value;
                path = path->parent;
                onechar |= (char) (path->value << 4);
                path = path->parent;
            }
            std_chararray result;
            for(sint32 i = collector.Count() - 1; 0 <= i; --i)
                result.AtAdding() = collector[i];
            result.AtAdding() = '\0';
            return result;
        }
    public:
        static sint64 ToInt(const std_chararray& path)
        {
            sint64 Result = 0;
            for(sint32 i = 0, iend = path.Count(); i < iend && path[i] != '\0'; ++i)
                Result = (Result << 8) | (path[i] & 0xFF);
            return Result;
        }
    };

    namespace _stdmap_detail
    {
        struct StdKey
        {
            using Variant = std::variant<std::string, int64_t>;
            Variant v;

            StdKey() : v(std::string()) {}
            explicit StdKey(const char* s) : v(std::string(s ? s : "")) {}
            explicit StdKey(std::string s) : v(std::move(s)) {}
            explicit StdKey(int64_t i) : v(i) {}

            template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
            explicit StdKey(I i) : v((int64_t) i) {}
        };

        struct StdKeyHash
        {
            size_t operator()(const StdKey& k) const noexcept
            {
                return std::visit([](auto&& vv) -> size_t {
                    using T = std::decay_t<decltype(vv)>;
                    if constexpr (std::is_same_v<T, std::string>)
                        return std::hash<std::string_view>{}(std::string_view(vv));
                    else
                        return std::hash<int64_t>{}(vv);
                }, k.v);
            }
        };

        struct StdKeyEq
        {
            bool operator()(const StdKey& a, const StdKey& b) const noexcept
            {
                return a.v == b.v;
            }
        };

        inline void write_path_from_bytes(std::vector<StdMapPath>& nodes, const unsigned char* bytes, size_t len)
        {
            nodes.clear();
            nodes.reserve(1 + len * 2);
            nodes.emplace_back(nullptr, 0);
            const StdMapPath* cur = &nodes.back();
            for(size_t i = 0; i < len; ++i)
            {
                const unsigned char c = bytes[i];
                nodes.emplace_back(cur, (sint32) ((c & 0xF0) >> 4));
                cur = &nodes.back();
                nodes.emplace_back(cur, (sint32) (c & 0x0F));
                cur = &nodes.back();
            }
        }

        inline void write_path_from_cstr(std::vector<StdMapPath>& nodes, const char* s, sint32 length, sint32 endmark)
        {
            std::string tmp;
            if(!s) s = "";
            if(length == -1)
            {
                while(*s && *s != (char) endmark)
                {
                    tmp.push_back(*s);
                    ++s;
                }
            }
            else
            {
                for(sint32 i = 0; i < length; ++i)
                    tmp.push_back(s[i]);
            }
            write_path_from_bytes(nodes, (const unsigned char*) tmp.data(), tmp.size());
        }

        inline void write_path_from_wstr(std::vector<StdMapPath>& nodes, const wchar_t* s, sint32 length, sint32 endmark)
        {
            // Boss2D 원본은 UTF16을 nibble 4개로 분해하지만,
            // StdMap은 호환 목적상 wchar_t 값을 byte 스트림(LE)로 단순 인코딩합니다.
            // AccessByOrder/Callback에서 MapPath::GetPath() -> ToInt 형태만 유지하면 됩니다.
            std::vector<unsigned char> bytes;
            if(!s) s = L"";
            if(length == -1)
            {
                while(*s && *s != (wchar_t) endmark)
                {
                    const wchar_t ch = *s++;
                    for(size_t b = 0; b < sizeof(wchar_t); ++b)
                        bytes.push_back((unsigned char) ((ch >> (8 * (sizeof(wchar_t) - 1 - b))) & 0xFF));
                }
            }
            else
            {
                for(sint32 i = 0; i < length; ++i)
                {
                    const wchar_t ch = s[i];
                    for(size_t b = 0; b < sizeof(wchar_t); ++b)
                        bytes.push_back((unsigned char) ((ch >> (8 * (sizeof(wchar_t) - 1 - b))) & 0xFF));
                }
            }
            write_path_from_bytes(nodes, bytes.data(), bytes.size());
        }

        inline void write_path_from_int(std::vector<StdMapPath>& nodes, uint64 v)
        {
            unsigned char bytes[8];
            for(int i = 0; i < 8; ++i)
                bytes[i] = (unsigned char) ((v >> (8 * (7 - i))) & 0xFF);
            size_t first = 0;
            while(first < 7 && bytes[first] == 0) first++;
            write_path_from_bytes(nodes, bytes + first, 8 - first);
        }

        template<typename ARR>
        inline void write_out_path(ARR* out, const std::vector<StdMapPath>& nodes)
        {
            if(!out) return;
            *out = nodes.back().GetPath();
        }
    }

    /// @brief Std맵지원
    template<typename TYPE, sint32 ENDMARK = 0>
    class StdMap
    {
    public:
        typedef void (*AccessCB)(const StdMapPath*, TYPE*, payload);

    public:
        /// @brief 정수식 접근(생성가능)
        /// @param key : 검색키
        /// @return 해당 객체
        TYPE& operator[](uint08 key) { return CreativeByIndexable((uint64) key); }
        TYPE& operator[](uint16 key) { return CreativeByIndexable((uint64) key); }
        TYPE& operator[](uint32 key) { return CreativeByIndexable((uint64) key); }
        TYPE& operator[](uint64 key) { return CreativeByIndexable((uint64) key); }
        TYPE& operator[](sint08 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint64) (uint08) key); }
        TYPE& operator[](sint16 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint64) (uint16) key); }
        TYPE& operator[](sint32 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint64) (uint32) key); }
        TYPE& operator[](sint64 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return CreativeByIndexable((uint64) (uint64) key); }

        /// @brief 정수식 마지막 추가후 접근
        /// @return 해당 객체
        TYPE& AtAdding() { return CreativeByIndexable((uint64) Info.Count); }

        /// @brief 문자열(UTF8/CP949)식 접근(생성가능)
        /// @param key : 검색키
        /// @param length : 검색키의 길이(-1이면 자동설정)
        /// @return 해당 객체
        TYPE& operator()(chars key, sint32 length = -1)
        {
            Info.LastTipStr.clear();
            if(!key) key = "";
            if(length == -1)
            {
                while(*key && *key != (char) ENDMARK)
                {
                    Info.LastTipStr.push_back(*key);
                    ++key;
                }
            }
            else if(length == 0)
            {
                // 원본과 동일하게 "\0" 1개를 의미하도록 0길이 키는 빈 스트링
            }
            else
            {
                for(sint32 i = 0; i < length; ++i)
                    Info.LastTipStr.push_back(key[i]);
                key += length;
            }
            Info.LastTipPtr = Info.LastTipStr.c_str();

            return get_or_add(_stdmap_detail::StdKey(Info.LastTipStr));
        }

        /// @brief 문자열(UTF16)식 접근(생성가능)
        /// @param key : 검색키
        /// @param length : 검색키의 길이(-1이면 자동설정)
        /// @return 해당 객체
        TYPE& operator()(wchars key, sint32 length = -1)
        {
            Info.LastTipWStr.clear();
            if(!key) key = L"";
            if(length == -1)
            {
                while(*key && *key != (wchar_t) ENDMARK)
                {
                    Info.LastTipWStr.push_back(*key);
                    ++key;
                }
            }
            else if(length == 0)
            {
                // empty
            }
            else
            {
                for(sint32 i = 0; i < length; ++i)
                    Info.LastTipWStr.push_back(key[i]);
                key += length;
            }
            Info.LastTipPtr = Info.LastTipWStr.data();

            // wchar 키는 std::wstring을 UTF-8로 변환하지 않고 그대로 보관(내부용)
            // key 충돌 방지를 위해 앞에 0x01 태그를 붙여 string으로 저장
            std::string tagged;
            tagged.reserve(1 + Info.LastTipWStr.size() * sizeof(wchar_t));
            tagged.push_back('\x01');
            for(wchar_t ch : Info.LastTipWStr)
            {
                for(size_t b = 0; b < sizeof(wchar_t); ++b)
                    tagged.push_back((char) ((ch >> (8 * (sizeof(wchar_t) - 1 - b))) & 0xFF));
            }
            return get_or_add(_stdmap_detail::StdKey(std::move(tagged)));
        }

        /// @brief 정수식 접근(생성불가)
        /// @param key : 검색키
        /// @return 해당 객체의 포인터
        TYPE* Access(uint08 key) const { return AccessByIndexable((uint64) key); }
        TYPE* Access(uint16 key) const { return AccessByIndexable((uint64) key); }
        TYPE* Access(uint32 key) const { return AccessByIndexable((uint64) key); }
        TYPE* Access(uint64 key) const { return AccessByIndexable((uint64) key); }
        TYPE* Access(sint08 key) const { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint64) (uint08) key); }
        TYPE* Access(sint16 key) const { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint64) (uint16) key); }
        TYPE* Access(sint32 key) const { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint64) (uint32) key); }
        TYPE* Access(sint64 key) const { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return AccessByIndexable((uint64) (uint64) key); }

        /// @brief 문자열(UTF8/CP949)식 접근(생성불가)
        /// @param key : 검색키
        /// @param length : 검색키의 길이(-1이면 자동설정)
        /// @return 해당 객체의 포인터
        TYPE* Access(chars key, sint32 length = -1) const
        {
            if(!key) key = "";
            std::string s;
            if(length == -1)
            {
                while(*key && *key != (char) ENDMARK) { s.push_back(*key); ++key; }
            }
            else if(length == 0)
            {
            }
            else
            {
                s.assign(key, key + length);
                key += length;
            }
            Info.LastTipStr = s;
            Info.LastTipPtr = Info.LastTipStr.c_str();
            return access_ptr(_stdmap_detail::StdKey(s));
        }

        /// @brief 문자열(UTF16)식 접근(생성불가)
        /// @param key : 검색키
        /// @param length : 검색키의 길이(-1이면 자동설정)
        /// @return 해당 객체의 포인터
        TYPE* Access(wchars key, sint32 length = -1) const
        {
            if(!key) key = L"";
            std::wstring ws;
            if(length == -1)
            {
                while(*key && *key != (wchar_t) ENDMARK) { ws.push_back(*key); ++key; }
            }
            else if(length == 0)
            {
            }
            else
            {
                ws.assign(key, key + length);
                key += length;
            }
            Info.LastTipWStr = ws;
            Info.LastTipPtr = Info.LastTipWStr.data();

            std::string tagged;
            tagged.reserve(1 + ws.size() * sizeof(wchar_t));
            tagged.push_back('\x01');
            for(wchar_t ch : ws)
            {
                for(size_t b = 0; b < sizeof(wchar_t); ++b)
                    tagged.push_back((char) ((ch >> (8 * (sizeof(wchar_t) - 1 - b))) & 0xFF));
            }
            return access_ptr(_stdmap_detail::StdKey(tagged));
        }

        /// @brief 순번식 접근(생성불가)
        /// @param order : 순번
        /// @param getpath : 경로얻기
        /// @return 해당 객체의 포인터
        TYPE* AccessByOrder(sint32 order, std_chararray* getpath = nullptr) const
        {
            return AccessByOrderCore(order, getpath);
        }

        /// @brief 콜백식 접근(생성불가)
        /// @param cb : 콜백함수
        /// @param param : 전달인수
        void AccessByCallback(AccessCB cb, payload param) const
        {
            if(!cb) return;
            std::vector<StdMapPath> nodes;
            for(const auto& k : m_order)
            {
                auto it = m_map.find(k);
                if(it == m_map.end()) continue;

                if(auto ps = std::get_if<std::string>(&k.v))
                {
                    // tagged wchar key?
                    if(!ps->empty() && (unsigned char)(*ps)[0] == 0x01)
                    {
                        // strip tag
                        const unsigned char* bytes = (const unsigned char*) ps->data() + 1;
                        const size_t len = ps->size() - 1;
                        _stdmap_detail::write_path_from_bytes(nodes, bytes, len);
                        cb(&nodes.back(), (TYPE*) &it->second, param);
                    }
                    else
                    {
                        _stdmap_detail::write_path_from_cstr(nodes, ps->c_str(), (sint32) ps->size(), ENDMARK);
                        cb(&nodes.back(), (TYPE*) &it->second, param);
                    }
                }
                else if(auto pi = std::get_if<int64_t>(&k.v))
                {
                    _stdmap_detail::write_path_from_int(nodes, (uint64) *pi);
                    cb(&nodes.back(), (TYPE*) &it->second, param);
                }
            }
        }

        /// @brief 정수식 데이터제거
        /// @param key : 검색키
        /// @return 성공여부
        bool Remove(uint08 key) { return RemoveByIndexable((uint64) key); }
        bool Remove(uint16 key) { return RemoveByIndexable((uint64) key); }
        bool Remove(uint32 key) { return RemoveByIndexable((uint64) key); }
        bool Remove(uint64 key) { return RemoveByIndexable((uint64) key); }
        bool Remove(sint08 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint64) (uint08) key); }
        bool Remove(sint16 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint64) (uint16) key); }
        bool Remove(sint32 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint64) (uint32) key); }
        bool Remove(sint64 key) { BOSS_ASSERT("key가 음수값입니다", 0 <= key); return RemoveByIndexable((uint64) (uint64) key); }

        /// @brief 문자열(UTF8/CP949)식 데이터제거
        /// @param key : 검색키
        /// @return 성공여부
        bool Remove(chars key)
        {
            if(!key) key = "";
            std::string s;
            while(*key && *key != (char) ENDMARK) { s.push_back(*key); ++key; }
            if(remove_key(_stdmap_detail::StdKey(s)))
            {
                Info.Count--;
                return true;
            }
            return false;
        }

        /// @brief 문자열(UTF16)식 데이터제거
        /// @param key : 검색키
        /// @return 성공여부
        bool Remove(wchars key)
        {
            if(!key) key = L"";
            std::wstring ws;
            while(*key && *key != (wchar_t) ENDMARK) { ws.push_back(*key); ++key; }

            std::string tagged;
            tagged.reserve(1 + ws.size() * sizeof(wchar_t));
            tagged.push_back('\x01');
            for(wchar_t ch : ws)
            {
                for(size_t b = 0; b < sizeof(wchar_t); ++b)
                    tagged.push_back((char) ((ch >> (8 * (sizeof(wchar_t) - 1 - b))) & 0xFF));
            }

            if(remove_key(_stdmap_detail::StdKey(tagged)))
            {
                Info.Count--;
                return true;
            }
            return false;
        }

        /// @brief 전체초기화
        void Reset()
        {
            Info.Count = 0;
            Info.LastTipPtr = nullptr;
            Info.LastTipStr.clear();
            Info.LastTipWStr.clear();
            m_map.clear();
            m_order.clear();
        }

        /// @brief 데이터수량 얻기
        /// @return 데이터수량
        const sint32 Count() const { return (sint32) Info.Count; }

        /// @brief 팁정보 얻기
        /// @return operator[]/operator()와 Access등에 의해 발생된 팁정보
        const void* GetLastTip() const { return Info.LastTipPtr; }

        /// @brief 생성자
        StdMap()
        {
            Info.Count = 0;
            Info.LastTipPtr = nullptr;
        }

        /// @brief 복사생성자
        /// @param rhs : 복사할 인스턴스
        StdMap(const StdMap& rhs)
        {
            Reset();
            operator=(rhs);
        }

        /// @brief 이동생성자
        /// @param rhs : 이동할 인스턴스
        StdMap(StdMap&& rhs) noexcept
        {
            Reset();
            operator=(ToReference(rhs));
        }

        /// @brief 소멸자
        ~StdMap()
        {
            Reset();
        }

        /// @brief 복사
        /// @param rhs : 복사할 인스턴스
        /// @return 자기 객체
        StdMap& operator=(const StdMap& rhs)
        {
            Reset();
            Info = rhs.Info;
            m_map = rhs.m_map;
            m_order = rhs.m_order;
            return *this;
        }

        /// @brief 이동
        /// @param rhs : 이동할 인스턴스
        /// @return 자기 객체
        StdMap& operator=(StdMap&& rhs) noexcept
        {
            Reset();
            Info = rhs.Info;
            m_map = std::move(rhs.m_map);
            m_order = std::move(rhs.m_order);
            rhs.Reset();
            return *this;
        }

    protected:
        /// @brief 생성자(내부호출용)
        StdMap(sint32)
        {
            Info.Count = 0;
            Info.LastTipPtr = nullptr;
        }

    private:
        struct VarInfo
        {
            sint32 Count {0};
            mutable const void* LastTipPtr {nullptr};
            mutable std::string LastTipStr;
            mutable std::wstring LastTipWStr;
        };

        TYPE& get_or_add(const _stdmap_detail::StdKey& k)
        {
            auto it = m_map.find(k);
            if(it != m_map.end()) return it->second;
            m_order.push_back(k);
            auto [it2, _] = m_map.emplace(k, TYPE());
            Info.Count++;
            if(IsTypePointer<TYPE>())
                *((void**) &it2->second) = nullptr;
            return it2->second;
        }

        TYPE* access_ptr(const _stdmap_detail::StdKey& k) const
        {
            auto it = m_map.find(k);
            if(it == m_map.end()) return nullptr;
            return (TYPE*) &it->second;
        }

        TYPE& CreativeByIndexable(uint64 key)
        {
            Info.LastTipInt = key;
            Info.LastTipPtr = &Info.LastTipInt;
            return get_or_add(_stdmap_detail::StdKey((int64_t) key));
        }

        TYPE* AccessByIndexable(uint64 key) const
        {
            Info.LastTipInt = key;
            Info.LastTipPtr = &Info.LastTipInt;
            return access_ptr(_stdmap_detail::StdKey((int64_t) key));
        }

        TYPE* AccessByOrderCore(sint32 order, std_chararray* getpath) const
        {
            if(order < 0 || (size_t) order >= m_order.size()) return nullptr;
            const auto& k = m_order[(size_t) order];
            auto it = m_map.find(k);
            if(it == m_map.end()) return nullptr;

            std::vector<StdMapPath> nodes;
            if(auto ps = std::get_if<std::string>(&k.v))
            {
                if(!ps->empty() && (unsigned char)(*ps)[0] == 0x01)
                {
                    const unsigned char* bytes = (const unsigned char*) ps->data() + 1;
                    const size_t len = ps->size() - 1;
                    _stdmap_detail::write_path_from_bytes(nodes, bytes, len);
                }
                else
                {
                    _stdmap_detail::write_path_from_bytes(nodes, (const unsigned char*) ps->data(), ps->size());
                }
            }
            else if(auto pi = std::get_if<int64_t>(&k.v))
            {
                _stdmap_detail::write_path_from_int(nodes, (uint64) *pi);
            }

            if(getpath && !nodes.empty())
                *getpath = nodes.back().GetPath();
            return (TYPE*) &it->second;
        }

        bool RemoveByIndexable(uint64 key)
        {
            if(remove_key(_stdmap_detail::StdKey((int64_t) key)))
            {
                Info.Count--;
                return true;
            }
            return false;
        }

        bool remove_key(const _stdmap_detail::StdKey& k)
        {
            auto it = m_map.find(k);
            if(it == m_map.end()) return false;
            m_map.erase(it);
            for(size_t i = 0; i < m_order.size(); )
            {
                if(m_order[i].v == k.v) m_order.erase(m_order.begin() + (long) i);
                else ++i;
            }
            return true;
        }

    private:
        // NOTE: 원본 Map은 trie 기반이었고, 여기서는 unordered_map + insertion-order로 교체합니다.
        mutable struct VarInfoEx : VarInfo
        {
            mutable uint64 LastTipInt {0};
        } Info;

        std::unordered_map<_stdmap_detail::StdKey, TYPE, _stdmap_detail::StdKeyHash, _stdmap_detail::StdKeyEq> m_map;
        std::vector<_stdmap_detail::StdKey> m_order;
    };
}
