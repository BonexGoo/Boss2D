#pragma once
#include <platform/boss_platform.hpp>
#include <element/boss_point.hpp>
#include <element/boss_rect.hpp>
#include <functional>

namespace BOSS
{
    /// @brief 길찾기
    class TryWorld
    {
        ////////////////////////////////////////////////////////////////////////////////
        // Dot
        public: class Dot
        {
            public: Dot() {x = 0; y = 0; Payload = -1;}
            public: Dot(const Point& point) {x = point.x; y = point.y; Payload = -1;}
            public: Dot(float _x, float _y, int _payload = -1) {x = _x; y = _y; Payload = _payload;}
            public: ~Dot() {}
            public: float x;
            public: float y;
            public: int Payload;
        };
        public: typedef Array<Dot, datatype_class_canmemcpy, 4> DotList;

        ////////////////////////////////////////////////////////////////////////////////
        // Line
        public: enum linetype {linetype_bound, linetype_space, linetype_wall};
        private: struct Line
        {
            linetype Type;
            int DotA;
            int DotB;
            void Set(linetype type, int dotA, int dotB)
            {
                Type = type;
                DotA = dotA;
                DotB = dotB;
            }
        };
        private: typedef Array<Line, datatype_pod_canmemcpy, 8> LineList;

        ////////////////////////////////////////////////////////////////////////////////
        // Polygon
        private: class Polygon
        {
            public: Polygon() {Enable = false;}
            public: ~Polygon() {}
            public: bool Enable;
            public: DotList DotArray;
        };
        private: typedef Array<Polygon, datatype_class_nomemcpy, 8> PolygonList;

        public: class Util;
        public: class Path;
        public: class Map;
        public: class Hurdle;
        public: class GetPosition;

        ////////////////////////////////////////////////////////////////////////////////
        // Util
        public: class Util
        {
            public: static inline const int GetClockwiseValue(const Dot& Start, const Dot& End, const Dot& Src)
            {return (int) (((Start.x - End.x) * (Src.y - End.y) - (Start.y - End.y) * (Src.x - End.x)) * 256);}
            public: static const Dot* GetDotByLineCross(const Dot& DstB, const Dot& DstE, const Dot& SrcB, const Dot& SrcE);
            public: static bool PtInPolygon(const DotList& Polygon, const Dot& Pt);
            public: static bool PtInPolyLine(const DotList& Polygon, const Dot& Pt);
        };

        ////////////////////////////////////////////////////////////////////////////////
        // Path
        public: class Path
        {
            friend class Map;
            friend class GetPosition;
            private: const int Step;
            public: int DotFocus;
            public: DotList Dots;

            private: Path(int step) : Step(step)
            {
                DotFocus = 0;
            }
            public: ~Path()
            {
            }

            public: static void Release(Path*& path)
            {
                delete path;
                path = nullptr;
            }
        };

        ////////////////////////////////////////////////////////////////////////////////
        // Map
        public: class Map
        {
            friend class Hurdle;
            public: DotList Dots;
            public: LineList Lines;
            private: class Triangle
            {
                friend class Map;
                private: linetype TypeAB;
                private: linetype TypeAC;
                private: linetype TypeBC;
                private: int DotA;
                private: int DotB;
                private: int DotC;
                private: Triangle* LinkAB;
                private: Triangle* LinkAC;
                private: Triangle* LinkBC;
                private: Dot WayDot;
                private: Triangle* WayBack;
                private: int ObjectScore;
                private: int DistanceSum;
                private: Triangle* Next;

                private: Triangle()
                {
                    TypeAB = linetype_bound;
                    TypeAC = linetype_bound;
                    TypeBC = linetype_bound;
                    DotA = -1;
                    DotB = -1;
                    DotC = -1;
                    LinkAB = nullptr;
                    LinkAC = nullptr;
                    LinkBC = nullptr;
                    WayDot = Dot(0, 0);
                    WayBack = nullptr;
                    ObjectScore = -1;
                    DistanceSum = 0;
                    Next = nullptr;
                }
                public: ~Triangle()
                {
                    Triangle* DeleteNode = Next;
                    while(DeleteNode)
                    {
                        Triangle* DeleteNodeNext = DeleteNode->Next;
                        DeleteNode->Next = nullptr;
                        delete DeleteNode;
                        DeleteNode = DeleteNodeNext;
                    }
                }

                private: Triangle* INSERT_FIRST()
                {
                    Triangle* Result = new Triangle();
                    Result->Next = Next;
                    return Next = Result;
                }
                private: Triangle* DELETE_FIRST()
                {
                    if(Next)
                    {
                        Triangle* OldTriangle = Next;
                        Next = Next->Next;
                        OldTriangle->Next = nullptr;
                        delete OldTriangle;
                    }
                    return nullptr;
                }
            } Top;

            private: Map();
            public: ~Map();
            public: static void Release(Map*& map);
            public: typedef std::function<int(const Dot&, const Dot&, const Dot&)> ScoreCB;
            public: Path* CreatePath(const int step);
            public: Path* BuildPath(const Dot& beginPos, const Dot& endPos, const int step = 0, int* score = nullptr, ScoreCB cb = nullptr);
            private: bool MAPPING(PolygonList& list);
            private: bool CREATE_TRIANGLES(Triangle* focus, Triangle* parent, linetype type, int dotA, int dotB);
            private: bool IS_INCLUDE_ANY_DOT_BY(int dotA, int dotB, int dotC) const;
            private: bool IS_CROSSING_ANY_LINE_BY(int dotA, int dotB) const;
            private: int FIND_LINE_ID(int dotA, int dotB) const;
            private: Triangle* FIND_PICK_TRIANGLE(const Dot& pos);
            private: Triangle* FIND_SAME_TRIANGLE(int dotA, int dotB, Triangle* parent);
            private: bool PATH_FIND(Triangle* focus, const Triangle* target, const Dot& endPos, ScoreCB cb) const;

            private: inline const int R_SIDE1(const int dotA, const int dotB, const int dotC) const
            {return ((Dots[dotA].x - Dots[dotB].x) * (Dots[dotC].y - Dots[dotB].y) - (Dots[dotA].y - Dots[dotB].y) * (Dots[dotC].x - Dots[dotB].x));}
            private: inline const int R_SIDE2(const int dotA, const int dotB, const Dot& C) const
            {return ((Dots[dotA].x - Dots[dotB].x) * (C.y - Dots[dotB].y) - (Dots[dotA].y - Dots[dotB].y) * (C.x - Dots[dotB].x));}
            private: inline const int DISTANCE(const Dot& A, const Dot& B) const
            {return Math::Sqrt(Math::Pow(A.x - B.x) + Math::Pow(A.y - B.y));}
            private: inline const Dot DOT_AB_SIDE_A(const Triangle& t) const
            {return Dot((Dots[t.DotA].x * 2 + Dots[t.DotB].x) / 3, (Dots[t.DotA].y * 2 + Dots[t.DotB].y) / 3);}
            private: inline const Dot DOT_AC_SIDE_A(const Triangle& t) const
            {return Dot((Dots[t.DotA].x * 2 + Dots[t.DotC].x) / 3, (Dots[t.DotA].y * 2 + Dots[t.DotC].y) / 3);}
            private: inline const Dot DOT_BC_SIDE_B(const Triangle& t) const
            {return Dot((Dots[t.DotB].x * 2 + Dots[t.DotC].x) / 3, (Dots[t.DotB].y * 2 + Dots[t.DotC].y) / 3);}
            private: inline const Dot DOT_AB_SIDE_B(const Triangle& t) const
            {return Dot((Dots[t.DotA].x + Dots[t.DotB].x * 2) / 3, (Dots[t.DotA].y + Dots[t.DotB].y * 2) / 3);}
            private: inline const Dot DOT_AC_SIDE_C(const Triangle& t) const
            {return Dot((Dots[t.DotA].x + Dots[t.DotC].x * 2) / 3, (Dots[t.DotA].y + Dots[t.DotC].y * 2) / 3);}
            private: inline const Dot DOT_BC_SIDE_C(const Triangle& t) const
            {return Dot((Dots[t.DotB].x + Dots[t.DotC].x * 2) / 3, (Dots[t.DotB].y + Dots[t.DotC].y * 2) / 3);}
        };

        ////////////////////////////////////////////////////////////////////////////////
        // Hurdle
        public: class Hurdle
        {
            friend class GetPosition;
            private: const sint32 DiffBorder;
            private: bool BuildFlag;
            private: PolygonList List;

            private: Hurdle(sint32 diffBorder = 10);
            public: ~Hurdle();
            public: static Hurdle* Create(Hurdle* hurdle = nullptr, sint32 diffBorder = 10);
            public: static void Release(Hurdle*& hurdle);
            public: void Add(DotList& polygon, bool add_or_sub);
            public: void AddWithoutMerging(const DotList& polygon);
            public: void SetPayload(const Rect& testRect, int payload);
            public: Map* BuildMap(const Rect& boundBox);
        };

        ////////////////////////////////////////////////////////////////////////////////
        // GetPosition
        public: class GetPosition
        {
            private: GetPosition();
            private: ~GetPosition();
            public: static bool SubTarget(const Hurdle* hurdle, Path* path, const Point& curPos, Point& targetPos);
            public: static const Dot** GetValidNext(const Hurdle* hurdle, const Point& curPos, const Point& nextPos,
                Point& resultPos, Point& reflectPos, float distanceMin = -1);
        };
    };
}
