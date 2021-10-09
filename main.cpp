#include <iostream>
#include <set>
#include <vector>
#include <random>

//make MSS, MSS+add, MSS+subtract, MSS+add+subtract - completed
//make UnionState, InterState
//make Probability test overloaded

class BaseState {
public:
    BaseState() = default;

    virtual bool contains(int) const {
        return true;
    }
};



class DiscreteState: public BaseState {
private:
    int const state;

public:
    DiscreteState(int state): state(state) { }

    bool contains(int s) const override {
        return s == state;
    }
};

class SegmentState: public BaseState {
private:
    int const beg, end;

public:
    SegmentState(): beg(0), end(-1) { }
    SegmentState(int beg, int end): beg(beg), end(end) { }

    bool contains(int s) const {
        return s >= beg && s <= end;
    }
};

class MultiSegmentState: public BaseState {
private:
    std::vector<int> const m_beg;
    std::vector<int> const m_end;

public:
    MultiSegmentState(): m_beg(), m_end() { }
    MultiSegmentState(std::vector<int> m_beg, std::vector<int> m_end): m_beg(m_beg), m_end(m_end) { }

    bool contains(int s) const {
        for(unsigned int i=0; i<m_beg.size(); i++){
            if(s>=m_beg[i] && s<= m_end[i]){
                return true;
            }
        }
        return false;
    }
};

class MSS_add: public BaseState{
private:
    std::vector<int> const mssadd_beg;
    std::vector<int> const mssadd_end;
    std::vector<int> const mssadd;

public:
    MSS_add(): mssadd_beg(), mssadd_end(), mssadd() { }
    MSS_add(std::vector<int> mssadd_beg, std::vector<int> mssadd_end, std::vector<int> mssadd): mssadd_beg(mssadd_beg), mssadd_end(mssadd_end), mssadd(mssadd) { }

    bool contains(int s) const {
        for(unsigned int i=0; i<mssadd_beg.size(); i++){
            if(s>=mssadd_beg[i] && s<= mssadd_end[i]){
                return true;
            }
        }
        for(unsigned int i=0; i<mssadd.size(); i++){
            if (s==mssadd[i]){
                return true;
            }
        }
        return false;
    }
};

class MSS_sub: public BaseState {
private:
    std::vector<int> const msssub_beg;
    std::vector<int> const msssub_end;
    std::vector<int> const msssub;

public:
    MSS_sub(): msssub_beg(), msssub_end(), msssub() { }
    MSS_sub(std::vector<int> msssub_beg, std::vector<int> msssub_end, std::vector<int> msssub): msssub_beg(msssub_beg), msssub_end(msssub_end), msssub(msssub) { }

    bool contains(int s) const {
        for (unsigned int i=0; i<msssub.size(); i++){
            if(s==msssub[i]){
                return false;
            }
        }
        for(unsigned int i=0; i<msssub_beg.size(); i++){
            if(s>=msssub_beg[i] && s<= msssub_end[i]){
                return true;
            }
        }
        return false;
    }
};

class MSS_as: public BaseState {
private:
    std::vector<int> const mssas_beg;
    std::vector<int> const mssas_end;
    std::vector<int> const msssub;
    std::vector<int> const mssadd;

public:
    MSS_as(): mssas_beg(), mssas_end(), msssub(), mssadd() { }
    MSS_as(std::vector<int> mssas_beg, std::vector<int> mssas_end, std::vector<int> msssub, std::vector<int> mssadd): mssas_beg(mssas_beg), mssas_end(mssas_end), msssub(msssub), mssadd(mssadd) { }

    bool contains(int s) const {
        for (unsigned int i=0; i<msssub.size(); i++){
            if(s==msssub[i]){
                return false;
            }
        }
        for (unsigned int i=0; i<mssadd.size(); i++){
            if(s==mssadd[i]){
                return true;
            }
        }
        for(unsigned int i=0; i<mssas_beg.size(); i++){
            if(s>=mssas_beg[i] && s<= mssas_end[i]){
                return true;
            }
        }
        return false;
    }
};


class SetState: public BaseState {
private:
    std::set<int> const states;

public:
    SetState(): states() { }
    SetState(std::set<int> const &src): states(src) { }

    bool contains(int s) const {
        return states.count(s) > 0;
    }
};

class ProbabilityTest {
private:
    unsigned seed;
    int test_min, test_max;
    unsigned test_count;

public:
    ProbabilityTest(unsigned seed, int test_min, int test_max, unsigned test_count): seed(seed), test_min(test_min),test_max(test_max), test_count(test_count) { }

    float operator()(BaseState const &s) const {
        std::default_random_engine rng(seed);
        std::uniform_int_distribution<int> dstr(test_min,test_max);
        unsigned good = 0;
        for (unsigned cnt = 0; cnt != test_count; ++cnt)
            if (s.contains(dstr(rng))) ++good;

        return static_cast<float>(good)/static_cast<float>(test_count);
    }
};


class IntersectionState: public BaseState {
private:
    BaseState* s1;
    BaseState* s2;

public:
    IntersectionState() = default;
    IntersectionState(BaseState& s1, BaseState& s2) : s1(&s1), s2(&s2){}

    bool contains(int elem) const override {
        return s1->contains(elem) && s2->contains(elem);
    }
};

class UnionState: public BaseState {
private:
    BaseState* s1;
    BaseState* s2;

public:
    UnionState() = default;
    UnionState(BaseState& s1, BaseState& s2) : s1(&s1), s2(&s2){}

    bool contains(int elem) const override {
        return s1->contains(elem) || s2->contains(elem);
    }
};


int main(int argc, const char * argv[]) {
    /*DiscreteState d(1);
    SegmentState s(0,10);
    MultiSegmentState ms({1, 5}, {2, 9});
    MSS_add m_add({1, 5}, {2, 9}, {11, 13});
    MSS_sub m_sub({1, 5}, {2, 9}, {2, 6, 8});
    MSS_as m_as({1, 5}, {2, 9}, {2, 6, 8}, {11, 13});
    SetState ss({1, 3, 5, 7, 23, 48, 57, 60, 90, 99});
    ProbabilityTest pt(10,0,100,100000);

    IntersectionState inter_state(m_as, ss);
    std::cout << "---" << std::endl;
    std::cout << pt(inter_state) << std::endl;
    std::cout << "---" << std::endl;

    UnionState un_state(m_as, ss);
    std::cout << "---" << std::endl;
    std::cout << pt(un_state) << std::endl;
    std::cout << "---" << std::endl;

    std::cout << pt(d) << std::endl;
    std::cout << pt(s) << std::endl;
    std::cout << pt(ms) << std::endl;
    std::cout << pt(m_add) << std::endl;
    std::cout << pt(m_sub) << std::endl;
    std::cout << pt(m_as) << std::endl;
    std::cout << pt(ss) << std::endl;*/
    SegmentState g(1, 10);
    SetState t({1, 3, 5, 7, 9, 11, 13, 15, 17, 19});
    ProbabilityTest pt(10,1,20,100000);
    bool hit_seg, hit_set;
    std::vector<float> c_seg, c_set;
    float temp_1, temp_2;
    for (int i=1; i<16; i++){
        hit_seg = false;
        hit_set = false;
        temp_1 = 1.0;
        temp_2 = 1.0;
        for (int j=0; j<i; j++){
            temp_1 *= pt(g);
            temp_2 *= pt(t);
        }
        c_seg.push_back(temp_1);
        c_set.push_back(temp_2);
    }

    for (int i=0; i<16; i++){
        std::cout << c_seg[i] << " ";
    }
    std::cout << std::endl;
    for (int i=0; i<16; i++){
        std::cout << c_seg[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
