#include "intervals.h"
#include <cassert>
#include <algorithm>
#include <utility>
#include <iostream>

DisjointIntervals::DisjointIntervals(std::vector<Interval> const &intervals)
{
    for(auto const &I : intervals){
        this->intervals.insert(I);
    }
    sanitize();
}

void DisjointIntervals::intersectionWith(DisjointIntervals const &other)
{
    auto it1 = this->intervals.cbegin();
    auto it2 = other.intervals.cbegin();
    auto end1 = this->intervals.cend();
    auto end2 = other.intervals.cend();

    std::set<Interval> new_intervals;
    while( it1!=end1 && it2!= end2){
        if(it1->left > it2->left){
            std::swap(it1,it2);
            std::swap(end1,end2);
        }
        // it1:   v
        //      --[----
        //      ----[--
        // it2:     ^
        if(it1->right < it2->left){
            // it1:   v
            //      --[-]---
            //      -----[--
            // it2:      ^
            it1++;
            continue;
        }else{
            // it1:   v
            //      --[------]-
            //      -----[-
            // it2:      ^
            if(it2->right < it1->right){
                // it1:   v
                //      --[------]-
                //      -----[-]---
                // it2:      ^
                new_intervals.insert(*it2);
                it2++;
                continue;
            }else{
                // it1:   v
                //      --[----]---
                //      -----[---]-
                // it2:      ^
                Interval I = {it2->left, it1->right};
                new_intervals.insert(I);
                it1++;
                continue;
            }
        }
    }
    intervals=new_intervals;
}

void DisjointIntervals::unionWith(DisjointIntervals const &other)
{
    auto it1 = this->intervals.cbegin();
    auto it2 = other.intervals.cbegin();
    auto end1 = this->intervals.cend();
    auto end2 = other.intervals.cend();

    std::set<Interval> new_intervals;

    //if either is empty
    if(it1==end1){
        intervals = other.intervals;
        return;
    }else if(it2==end2){
        return;
    }

    if(it1->left > it2->left){
        std::swap(it1,it2);
        std::swap(end1,end2);
    }
    Interval new_last = *it1;
    it1++;


    while( it1 != end1 || it2 != end2){
        if(it1 == end1 || it1->left > it2->left){
            std::swap(it1,it2);
            std::swap(end1,end2);
        }
        // it1:   v
        //      --[----
        //      ----[--
        // it2:     ^

        if(it1->left <= new_last.right){
            new_last.right = std::max(it1->right,new_last.right);
            it1++;
            continue;
        }else{
            new_intervals.insert(new_last);
            new_last = *it1;
            it1++;
            continue;
        }
    }
    new_intervals.insert(new_last);
    intervals=new_intervals;
}

void DisjointIntervals::print()
{
    for(auto I : intervals){
        std::cout<<"["<<I.left<<", "<<I.right<<"]"<<std::endl;
    }
}

bool DisjointIntervals::operator==(const DisjointIntervals &other) const
{
    return intervals==other.intervals;
}

void DisjointIntervals::sanitize()
{
    for(auto it=intervals.begin(); it!=intervals.end();){
        if(it->left > it->right){
            it=intervals.erase(it);
            continue;
        }
        it++;
    }
    auto it = this->intervals.cbegin();
    auto end = this->intervals.cend();

    std::set<Interval> new_intervals;
    //if either is empty
    if(it==end){
        return;
    }
    Interval new_last = *it;
    it++;

    while( it != end){
        if(it->left <= new_last.right){
            new_last.right = std::max(it->right,new_last.right);
            it++;
            continue;
        }else{
            new_intervals.insert(new_last);
            new_last = *it;
            it++;
            continue;
        }
    }
    new_intervals.insert(new_last);
    intervals=new_intervals;
}



bool DisjointIntervals::Interval::operator<(const DisjointIntervals::Interval &other) const
{
    return left < other.left ?
                true :
                ( left > other.left ?
                      false :
                      (right < other.right)
                      );
}

bool DisjointIntervals::Interval::operator==(const DisjointIntervals::Interval &other) const
{
    return left==other.left && right == other.right;
}
