#ifndef INTERVALS_H
#define INTERVALS_H

#include <vector>
#include <set>

class DisjointIntervals
{
public:
    struct Interval{
        float left, right;
        bool operator<(Interval const & other) const;
        bool operator==(Interval const & other) const;
    };
    //Intervals(std::set<Interval> const &intervals);
    DisjointIntervals(std::vector<Interval> const &intervals);
    void intersectionWith(DisjointIntervals const &other);
    void unionWith(DisjointIntervals const &other);
    void print();
    bool operator==(DisjointIntervals const &other) const;
private:
    std::set<Interval> intervals;
    void sanitize();
};

#endif // INTERVALS_H
