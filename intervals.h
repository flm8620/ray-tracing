#ifndef INTERVALS_H
#define INTERVALS_H

#include <vector>
#include <set>
#include <limits>
#include <Eigen/Dense>
class DisjointIntervals
{
public:
    struct Interval{
        float left, right;
        Eigen::Vector3f normalLeft, normalRight;
        bool operator<(Interval const & other) const;
        bool operator==(Interval const & other) const;
    };
    constexpr static float NEG_INF = -std::numeric_limits<float>::infinity();
    constexpr static float POS_INF = std::numeric_limits<float>::infinity();
    DisjointIntervals(std::vector<Interval> const &intervals);
    DisjointIntervals();
    DisjointIntervals& intersectionWith(DisjointIntervals const &other);
    DisjointIntervals& unionWith(DisjointIntervals const &other);
    DisjointIntervals& inverse();
    void print();
    bool operator==(DisjointIntervals const &other) const;
    std::set<Interval>::const_iterator begin()const{return intervals.cbegin();}
    std::set<Interval>::const_iterator end()const{return intervals.cend();}

    static DisjointIntervals empty();
    static DisjointIntervals all();
private:
    std::set<Interval> intervals;
    void sanitize();
};

#endif // INTERVALS_H
