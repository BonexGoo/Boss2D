// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.
//
// Copyright (C) 2018 Intel Corporation


#include "../test_precomp.hpp"
#include <opencv2/gapi/util/variant.hpp>
#include <cstddef> //std::max_align_t

namespace opencv_test
{

namespace
{
    typedef util::variant<int, std::string> TestVar;
    typedef util::variant<int, float>       TestVar2;
}

TEST(Variant, EmptyCTor)
{
    util::variant<int> vi;
    EXPECT_EQ(0,  util::get<int>(vi));

    util::variant<int, std::string> vis;
    EXPECT_EQ(0,  util::get<int>(vis));

    util::variant<std::string> vs;
    EXPECT_EQ("", util::get<std::string>(vs));

    util::variant<std::string, int> vsi;
    EXPECT_EQ("", util::get<std::string>(vsi));
}

TEST(Variant, ValueMoveCTor)
{
    util::variant<int> vi(42);
    EXPECT_EQ(0u,     vi.index());
    EXPECT_EQ(42,     util::get<int>(vi));

    util::variant<int, std::string> vis(2017);
    EXPECT_EQ(0u,     vis.index());
    EXPECT_EQ(2017,   util::get<int>(vis));

    util::variant<int, std::string> vis2(std::string("2017"));
    EXPECT_EQ(1u,     vis2.index());
    EXPECT_EQ("2017", util::get<std::string>(vis2));

    util::variant<std::string> vs(std::string("2017"));
    EXPECT_EQ(0u,     vs.index());
    EXPECT_EQ("2017", util::get<std::string>(vs));

    util::variant<std::string, int> vsi(std::string("2017"));
    EXPECT_EQ(0u,     vsi.index());
    EXPECT_EQ("2017", util::get<std::string>(vsi));

    util::variant<std::string, int> vsi2(42);
    EXPECT_EQ(1u,     vsi2.index());
    EXPECT_EQ(42,     util::get<int>(vsi2));
}

TEST(Variant, ValueCopyCTor)
{
    const int i42         = 42;
    const int i17         = 2017;
    const std::string s17 = "2017";

    util::variant<int> vi(i42);
    EXPECT_EQ(0u,     vi.index());
    EXPECT_EQ(i42,    util::get<int>(vi));

    util::variant<int, std::string> vis(i17);
    EXPECT_EQ(0u,     vis.index());
    EXPECT_EQ(i17,    util::get<int>(vis));

    util::variant<int, std::string> vis2(s17);
    EXPECT_EQ(1u,     vis2.index());
    EXPECT_EQ(s17,    util::get<std::string>(vis2));

    util::variant<std::string> vs(s17);
    EXPECT_EQ(0u,     vs.index());
    EXPECT_EQ(s17,    util::get<std::string>(vs));

    util::variant<std::string, int> vsi(s17);
    EXPECT_EQ(0u,     vsi.index());
    EXPECT_EQ(s17, util::get<std::string>(vsi));

    util::variant<std::string, int> vsi2(i42);
    EXPECT_EQ(1u,     vsi2.index());
    EXPECT_EQ(i42,    util::get<int>(vsi2));
}

TEST(Variant, CopyMoveCTor)
{
    const TestVar tvconst(std::string("42"));

    TestVar tv = tvconst;
    EXPECT_EQ( 1u,  tv.index());
    EXPECT_EQ("42", util::get<std::string>(tv));

    TestVar tv2(TestVar(40+2));
    EXPECT_EQ( 0u,  tv2.index());
    EXPECT_EQ( 42,  util::get<int>(tv2));
}

TEST(Variant, Assign_Basic)
{
    TestVar vis;
    EXPECT_EQ(0u, vis.index());
    EXPECT_EQ(0,  util::get<int>(vis));

    vis = 42;
    EXPECT_EQ(0u, vis.index());
    EXPECT_EQ(42, util::get<int>(vis));
}

TEST(Variant, Assign_ValueUpdate_SameType)
{
    TestVar vis(42);

    EXPECT_EQ(0u, vis.index());
    EXPECT_EQ(42, util::get<int>(vis));

    vis = 43;
    EXPECT_EQ(0u, vis.index());
    EXPECT_EQ(43, util::get<int>(vis));
}

TEST(Variant, Assign_ValueUpdate_DiffType)
{
    TestVar vis(42);

    EXPECT_EQ(0u, vis.index());
    EXPECT_EQ(42, util::get<int>(vis));

    vis = std::string("42");
    EXPECT_EQ(1u, vis.index());
    EXPECT_EQ("42", util::get<std::string>(vis));
}

TEST(Variant, Assign_ValueUpdate_Const)
{
    TestVar va(42);
    const TestVar vb(43);

    EXPECT_EQ(0u, va.index());
    EXPECT_EQ(42, util::get<int>(va));

    EXPECT_EQ(0u, vb.index());
    EXPECT_EQ(43, util::get<int>(vb));

    va = vb;

    EXPECT_EQ(0u, va.index());
    EXPECT_EQ(43, util::get<int>(va));
}

TEST(Variant, Assign_ValueUpdate_Const_DiffType)
{
    TestVar va(42);
    const TestVar vb(std::string("42"));

    EXPECT_EQ(0u, va.index());
    EXPECT_EQ(42, util::get<int>(va));

    EXPECT_EQ(1u, vb.index());
    EXPECT_EQ("42", util::get<std::string>(vb));

    va = vb;

    EXPECT_EQ(1u,   va.index());
    EXPECT_EQ("42", util::get<std::string>(va));
}

TEST(Variant, Assign_Move)
{
    TestVar va(42);
    TestVar vb(std::string("42"));
    TestVar vc(43);

    EXPECT_EQ(0u, va.index());
    EXPECT_EQ(42, util::get<int>(va));

    EXPECT_EQ(1u, vb.index());
    EXPECT_EQ("42", util::get<std::string>(vb));

    EXPECT_EQ(0u, vc.index());
    EXPECT_EQ(43, util::get<int>(vc));

    va = std::move(vb);
    EXPECT_EQ(1u, va.index());
    EXPECT_EQ("42", util::get<std::string>(va));

    va = std::move(vc);
    EXPECT_EQ(0u, va.index());
    EXPECT_EQ(43, util::get<int>(va));
}

TEST(Variant, Swap_SameIndex)
{
    TestVar tv1(42);
    TestVar tv2(43);

    EXPECT_EQ(0u, tv1.index());
    EXPECT_EQ(42, util::get<int>(tv1));

    EXPECT_EQ(0u, tv2.index());
    EXPECT_EQ(43, util::get<int>(tv2));

    tv1.swap(tv2);

    EXPECT_EQ(0u, tv1.index());
    EXPECT_EQ(43, util::get<int>(tv1));

    EXPECT_EQ(0u, tv2.index());
    EXPECT_EQ(42, util::get<int>(tv2));
}

TEST(Variant, Swap_DiffIndex)
{
    TestVar2 tv1(42);
    TestVar2 tv2(3.14f);

    EXPECT_EQ(0u, tv1.index());
    EXPECT_EQ(42, util::get<int>(tv1));

    EXPECT_EQ(1u, tv2.index());
    EXPECT_EQ(3.14f, util::get<float>(tv2));

    tv1.swap(tv2);

    EXPECT_EQ(0u, tv2.index());
    EXPECT_EQ(42, util::get<int>(tv2));

    EXPECT_EQ(1u, tv1.index());
    EXPECT_EQ(3.14f, util::get<float>(tv1));
}

TEST(Variant, Get)
{
    const TestVar cv(42);

    // Test const& get()
    EXPECT_EQ(42, util::get<int>(cv));
    EXPECT_THROW(util::get<std::string>(cv), util::bad_variant_access);

    // Test &get
    TestVar cv2(std::string("42"));
    EXPECT_EQ("42", util::get<std::string>(cv2));
    EXPECT_THROW(util::get<int>(cv2), util::bad_variant_access);
}

TEST(Variant, GetWrite)
{
    util::variant<int, std::string> v(42);
    EXPECT_EQ(42, util::get<int>(v));

    util::get<int>(v) = 43;
    EXPECT_EQ(43, util::get<int>(v));
}

TEST(Variant, NoDefaultCtor)
{
    struct MyType
    {
        int m_a;
        MyType() = delete;
    };

    // This code MUST compile
    util::variant<int, MyType> var;
    SUCCEED() << "Code compiled";

    // At the same time, util::variant<MyType, ...> MUST NOT.
}

TEST(Variant, MonoState)
{
    struct MyType
    {
        int m_a;
        explicit MyType(int a) : m_a(a) {}
        MyType() = delete;
    };

    util::variant<util::monostate, MyType> var;
    EXPECT_EQ(0u, var.index());

    var = MyType{42};
    EXPECT_EQ(1u, var.index());
    EXPECT_EQ(42, util::get<MyType>(var).m_a);
}


TEST(Variant, Eq)
{
    TestVar v1(42), v2(std::string("42"));
    TestVar v3(v1), v4(v2);

    EXPECT_TRUE(v1 == v3);
    EXPECT_TRUE(v2 == v4);
    EXPECT_TRUE(v1 != v2);
    EXPECT_TRUE(v3 != v4);

    EXPECT_FALSE(v1 == v2);
    EXPECT_FALSE(v3 == v4);
    EXPECT_FALSE(v1 != v3);
    EXPECT_FALSE(v2 != v4);
}

TEST(Variant, Eq_Monostate)
{
    using TestVar3 = util::variant<util::monostate, int>;
    TestVar3 v1;
    TestVar3 v2(42);

    EXPECT_NE(v1, v2);

    v2 = util::monostate{};
    EXPECT_EQ(v1, v2);
}

TEST(Variant, VectorOfVariants)
{
    std::vector<TestVar> vv1(1024);
    std::vector<TestVar> vv2(1024);

    EXPECT_TRUE(vv1 == vv2);

    std::vector<TestVar> vv3(2048, TestVar(std::string("42")));

    // Just test chat the below code compiles:
    // 1: internal copy of variants from one vector to another,
    //    with probable reallocation of 1st vector to host all elements
    std::copy(vv1.begin(), vv1.end(), std::back_inserter(vv2));
    EXPECT_EQ(2048u, vv2.size());

    // 2: truncation of vector, with probable destruction of its tail memory
    vv2.resize(1024);
    EXPECT_EQ(1024u, vv2.size());

    // 3. vector assignment, with overwriting underlying variants
    vv2 = vv3;
    EXPECT_EQ(2048u, vv2.size());
    EXPECT_TRUE(vv2 == vv3);
}

TEST(Variant, HoldsAlternative)
{
    TestVar v(42);
    EXPECT_TRUE (util::holds_alternative<int>        (v));
    EXPECT_FALSE(util::holds_alternative<std::string>(v));

    v = std::string("42");
    EXPECT_FALSE(util::holds_alternative<int>        (v));
    EXPECT_TRUE (util::holds_alternative<std::string>(v));
}

TEST(Variant, Sizeof)
{
    //variant has to store index of the contained type as well as the type itself
    EXPECT_EQ(2 * sizeof(size_t), (sizeof(util::variant<int, char>)));
#if !defined(__GNUG__) || __GNUG__ >= 5
    // GCC versions prior to 5.0 have limited C++11 support, e.g.
    // no std::max_align_t defined
    EXPECT_EQ((sizeof(std::max_align_t) + std::max(sizeof(size_t), alignof(std::max_align_t))), (sizeof(util::variant<std::max_align_t, char>)));
#endif
}

TEST(Variant, EXT_IndexOf)
{
    struct MyType{};
    class MyClass{};

    using V = util::variant<util::monostate, int, double, char, float, MyType, MyClass>;
    static_assert(0u == V::index_of<util::monostate>(), "Index is incorrect");
    static_assert(1u == V::index_of<int    >(), "Index is incorrect");
    static_assert(2u == V::index_of<double >(), "Index is incorrect");
    static_assert(3u == V::index_of<char   >(), "Index is incorrect");
    static_assert(4u == V::index_of<float  >(), "Index is incorrect");
    static_assert(5u == V::index_of<MyType >(), "Index is incorrect");
    static_assert(6u == V::index_of<MyClass>(), "Index is incorrect");
}

} // namespace opencv_test
