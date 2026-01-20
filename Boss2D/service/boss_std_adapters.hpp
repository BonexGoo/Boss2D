#pragma once

#include <vector>
#include <unordered_map>
#include <variant>
#include <string>
#include <string_view>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <algorithm>

namespace BOSS
{
    struct MapPath; // forward decl for callback-based maps
    // StdArray<T>: std::vector-backed adapter for Boss2D Array API (subset).
    // Note: Boss2D uses Array<T, datatype_pod_canmemcpy> where the 2nd template
    // argument is typically a *value token* (non-type). Therefore we accept
    // a non-type template pack: auto... Ignored.
    template<typename T, auto... Ignored>
    class StdArray
    {
    public:
        StdArray() = default;

        // Boss-style API
        inline sint32 Count() const { return (sint32) m_v.size(); }
        inline void Clear() { m_v.clear(); }
        inline void Reserve(sint32 n) { if(n > 0) m_v.reserve((size_t) n); }

        inline T& At(sint32 i) { return m_v[(size_t) norm_index(i)]; }
        inline const T& At(sint32 i) const { return m_v[(size_t) norm_index(i)]; }

        // Grows to contain index i.
        inline T& AtWherever(sint32 i)
        {
            if(i < 0) i = 0;
            const size_t idx = (size_t) i;
            if(m_v.size() <= idx) m_v.resize(idx + 1);
            return m_v[idx];
        }

        // Push default element and return reference.
        inline T& AtAdding()
        {
            m_v.emplace_back();
            return m_v.back();
        }

        // Boss code frequently uses SubtractionAll/SubtractionOne.
        inline void SubtractionAll() { m_v.clear(); }
        // Boss2D Array::SubtractionOne() is frequently used as pop-back.
        inline void SubtractionOne()
        {
            if(!m_v.empty()) m_v.pop_back();
        }
        // Also used as erase-at-index.
        inline void SubtractionOne(sint32 index)
        {
            if(m_v.empty()) return;
            const sint32 ni = (sint32) norm_index(index);
            if(0 <= ni && ni < (sint32) m_v.size())
                m_v.erase(m_v.begin() + (size_t) ni);
        }

        inline T& operator[](sint32 i) { return m_v[(size_t) norm_index(i)]; }
        inline const T& operator[](sint32 i) const { return m_v[(size_t) norm_index(i)]; }

        inline auto begin() { return m_v.begin(); }
        inline auto end() { return m_v.end(); }
        inline auto begin() const { return m_v.begin(); }
        inline auto end() const { return m_v.end(); }

    private:
        inline size_t norm_index(sint32 i) const
        {
            // Boss2D supports negative indexing: -1 means last.
            const sint32 n = (sint32) m_v.size();
            if(i < 0) i = n + i;
            if(i < 0) i = 0;
            if(i >= n) i = n - 1;
            return (size_t) i;
        }
        std::vector<T> m_v;
    };

    // StdMap<V>: unordered_map-backed adapter for Boss2D Map API (subset).
    // Key supports either std::string (for chars keys) or int64_t (for integral keys).
    struct StdKey
    {
        using Variant = std::variant<std::string, int64_t, std::uintptr_t>;
        Variant v;

        StdKey() : v(std::string()) {}
        explicit StdKey(const char* s) : v(std::string(s ? s : "")) {}
        explicit StdKey(std::string s) : v(std::move(s)) {}
        explicit StdKey(int64_t i) : v(i) {}

        // pointer keys (e.g., const MapPath*)
        explicit StdKey(const void* p) : v((std::uintptr_t) p) {}

        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        explicit StdKey(I i) : v((int64_t) i) {}

        // Allow assignment from C-strings and std::string (Boss-style key-out paths)
        StdKey& operator=(const char* s_in)
        {
            v = std::string(s_in ? s_in : "");
            return *this;
        }

        StdKey& operator=(const std::string& s_in)
        {
            v = s_in;
            return *this;
        }

    };

    struct StdKeyHash
    {
        size_t operator()(const StdKey& k) const noexcept
        {
            return std::visit([](auto&& vv) -> size_t {
                using T = std::decay_t<decltype(vv)>;
                if constexpr (std::is_same_v<T, std::string>)
                    return std::hash<std::string_view>{}(std::string_view(vv));
                else if constexpr (std::is_same_v<T, int64_t>)
                    return std::hash<int64_t>{}(vv);
                else
                    return std::hash<std::uintptr_t>{}(vv);
            }, k.v);
        }
    };

    struct StdKeyEq
    {
        bool operator()(const StdKey& a, const StdKey& b) const noexcept { return a.v == b.v; }
    };

    // Small trait: does KeyOut have SubtractionAll() and AtAdding()?
    template<typename T, typename = void> struct _has_boss_array_like : std::false_type {};
    template<typename T>
    struct _has_boss_array_like<T, std::void_t<
        decltype(std::declval<T&>().SubtractionAll()),
        decltype(std::declval<T&>().AtAdding())
    >> : std::true_type {};

    template<typename V>
    class StdMap
    {
    public:
        StdMap() = default;

        inline sint32 Count() const { return (sint32) m_order.size(); }
        inline void Clear() { m_map.clear(); m_order.clear(); }
        inline void Reset() { Clear(); }

        // Insert-or-access
        inline V& operator()(const char* key) { return get_or_add(StdKey(key)); }
        inline V& operator()(const void* key) { return get_or_add(StdKey(key)); }
        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        inline V& operator()(I key) { return get_or_add(StdKey(key)); }
        inline V& operator[](const char* key) { return get_or_add(StdKey(key)); }
        inline V& operator[](const void* key) { return get_or_add(StdKey(key)); }
        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        inline V& operator[](I key) { return get_or_add(StdKey(key)); }

        // Access (returns pointer to value or nullptr)
        inline V* Access(const char* key) { return access_ptr(StdKey(key)); }
        inline const V* Access(const char* key) const { return access_ptr(StdKey(key)); }
        inline V* Access(const void* key) { return access_ptr(StdKey(key)); }
        inline const V* Access(const void* key) const { return access_ptr(StdKey(key)); }
        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        inline V* Access(I key) { return access_ptr(StdKey(key)); }
        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        inline const V* Access(I key) const { return access_ptr(StdKey(key)); }

        // Access by insertion order
        // NOTE: Do not pass a plain nullptr here; MSVC cannot deduce KeyOut from std::nullptr_t.
        // We explicitly cast to StdKey* (unused) to select the intended template overload.
        inline V* AccessByOrder(sint32 index) { return access_by_order_impl(index, (StdKey*) nullptr); }
        inline const V* AccessByOrder(sint32 index) const { return access_by_order_impl(index, (StdKey*) nullptr); }

        template<typename KeyOut>
        inline V* AccessByOrder(sint32 index, KeyOut* key_out) { return access_by_order_impl(index, key_out); }
        template<typename KeyOut>
        inline const V* AccessByOrder(sint32 index, KeyOut* key_out) const { return access_by_order_impl(index, key_out); }

        // Remove
        inline void Remove(const char* key) { remove_key(StdKey(key)); }
        inline void Remove(const void* key) { remove_key(StdKey(key)); }
        template<typename I, typename = std::enable_if_t<std::is_integral_v<I>>>
        inline void Remove(I key) { remove_key(StdKey(key)); }

        // Iterate callback.
        // Boss2D patterns:
        //  - cb(const char* key, V* value, payload param)
        //  - cb(const MapPath* key, V* value, payload param)
        template<typename CB>
        inline void AccessByCallback(CB&& cb, payload param)
        {
            for(const auto& k : m_order)
            {
                auto it = m_map.find(k);
                if(it == m_map.end()) continue;

                if constexpr (std::is_invocable_v<CB, const char*, V*, payload>)
                {
                    if(auto ps = std::get_if<std::string>(&k.v))
                        cb(ps->c_str(), &it->second, param);
                }
                else if constexpr (std::is_invocable_v<CB, const MapPath*, V*, payload>)
                {
                    if(auto pp = std::get_if<std::uintptr_t>(&k.v))
                        cb((const MapPath*) (const void*) (*pp), &it->second, param);
                }
                else if constexpr (std::is_invocable_v<CB, const void*, V*, payload>)
                {
                    if(auto pp = std::get_if<std::uintptr_t>(&k.v))
                        cb((const void*) (*pp), &it->second, param);
                }
                else
                {
                    // Unsupported callable signature for this adapter.
                }
            }
        }

    private:
        V& get_or_add(const StdKey& k)
        {
            auto it = m_map.find(k);
            if(it != m_map.end()) return it->second;
            m_order.push_back(k);
            auto [it2, _] = m_map.emplace(k, V());
            return it2->second;
        }

        V* access_ptr(const StdKey& k)
        {
            auto it = m_map.find(k);
            return (it == m_map.end()) ? nullptr : &it->second;
        }
        const V* access_ptr(const StdKey& k) const
        {
            auto it = m_map.find(k);
            return (it == m_map.end()) ? nullptr : &it->second;
        }

        template<typename KeyOut>
        void write_key_out(KeyOut* out, const StdKey& k) const
        {
            if(!out) return;
			// If caller expects our key type, just copy it.
			if constexpr (std::is_same_v<std::remove_cv_t<KeyOut>, StdKey>)
			{
				*out = k;
				return;
			}
            // If caller wants a pointer key, pass it through.
            if constexpr (std::is_pointer_v<KeyOut>)
            {
                if(auto pp = std::get_if<std::uintptr_t>(&k.v))
                {
                    *out = (KeyOut) (const void*) (*pp);
                    return;
                }
            }

            std::string s;
            if(auto ps = std::get_if<std::string>(&k.v)) s = *ps;
            else if(auto pi = std::get_if<int64_t>(&k.v)) s = std::to_string(*pi);
            else s = "";

            if constexpr (_has_boss_array_like<KeyOut>::value)
            {
                out->SubtractionAll();
                for(char c : s) out->AtAdding() = c;
                out->AtAdding() = '\0';
            }
            else
            {
                // Fallback: try assignment from const char*
                *out = s.c_str();
            }
        }

        template<typename KeyOut>
        V* access_by_order_impl(sint32 index, KeyOut* key_out)
        {
            if(index < 0 || (size_t) index >= m_order.size()) return nullptr;
            const StdKey& k = m_order[(size_t) index];
            write_key_out(key_out, k);
            auto it = m_map.find(k);
            return (it == m_map.end()) ? nullptr : &it->second;
        }
        template<typename KeyOut>
        const V* access_by_order_impl(sint32 index, KeyOut* key_out) const
        {
            if(index < 0 || (size_t) index >= m_order.size()) return nullptr;
            const StdKey& k = m_order[(size_t) index];
            write_key_out(key_out, k);
            auto it = m_map.find(k);
            return (it == m_map.end()) ? nullptr : &it->second;
        }

        void remove_key(const StdKey& k)
        {
            m_map.erase(k);
            for(size_t i = 0; i < m_order.size(); )
            {
                if(m_order[i].v == k.v) m_order.erase(m_order.begin() + (long) i);
                else ++i;
            }
        }

    private:
        std::unordered_map<StdKey, V, StdKeyHash, StdKeyEq> m_map;
        std::vector<StdKey> m_order;
    };
}
