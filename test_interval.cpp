#include "gtest/gtest.h"
#include "intervals.h"
TEST(intervals,union_intersect){
    DisjointIntervals A;
    DisjointIntervals B;

    A.unionWith(B);
    EXPECT_EQ(A,DisjointIntervals());
    A.intersectionWith(B);
    EXPECT_EQ(A,DisjointIntervals());

    DisjointIntervals C({{0,10}});
    A.intersectionWith(C);
    EXPECT_EQ(A,DisjointIntervals());
    A.unionWith(C);
    EXPECT_EQ(A,C);

    DisjointIntervals D({{0,3}});
    A.unionWith(D);
    EXPECT_EQ(A,C);
    A.intersectionWith(D);
    EXPECT_EQ(A,D);

    DisjointIntervals E({{1,5}});
    A.intersectionWith(E);
    EXPECT_EQ(A,DisjointIntervals({{1,3}}));

    DisjointIntervals F({{2,4}});
    A.unionWith(F);
    EXPECT_EQ(A,DisjointIntervals({{1,4}}));

    DisjointIntervals G({{5,6}});
    A.intersectionWith(G);
    EXPECT_EQ(A,DisjointIntervals());

    DisjointIntervals H({{7,8}});
    G.unionWith(H);
    EXPECT_EQ(G,DisjointIntervals({{5,6},{7,8}}));

    DisjointIntervals I({{5.5,7.5}});
    G.intersectionWith(I);
    EXPECT_EQ(G,DisjointIntervals({{5.5,6},{7,7.5}}));

    DisjointIntervals J({{6,7}});
    G.unionWith(J);
    EXPECT_EQ(G,DisjointIntervals({{5.5,7.5}}));

    EXPECT_EQ(A,A.inverse().inverse());
    EXPECT_EQ(B,B.inverse().inverse());
    EXPECT_EQ(C,C.inverse().inverse());
    EXPECT_EQ(D,D.inverse().inverse());
    EXPECT_EQ(E,E.inverse().inverse());
    EXPECT_EQ(F,F.inverse().inverse());
    EXPECT_EQ(G,G.inverse().inverse());
}
