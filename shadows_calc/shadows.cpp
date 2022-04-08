#include <iostream>
//#include <vector>

#include "csv.hpp"
#include "solar.hpp"
#include "triangulation.hpp"
#include "grid.hpp"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <set>

/**
 * @see https://cp-algorithms.com/geometry/point-location.html
 */
namespace p_loc {

    typedef long long ll;

    bool ge(const ll &a, const ll &b) { return a >= b; }
    bool le(const ll &a, const ll &b) { return a <= b; }
    bool eq(const ll &a, const ll &b) { return a == b; }
    bool gt(const ll &a, const ll &b) { return a > b; }
    bool lt(const ll &a, const ll &b) { return a < b; }
    int sgn(const ll &x) { return le(x, 0) ? eq(x, 0) ? 0 : -1 : 1; }

    struct pt {
        ll x, y;
        pt() {}
        pt(ll _x, ll _y) : x(_x), y(_y) {}
        pt operator-(const pt &a) const { return pt(x - a.x, y - a.y); }
        ll dot(const pt &a) const { return x * a.x + y * a.y; }
        ll dot(const pt &a, const pt &b) const { return (a - *this).dot(b - *this); }
        ll cross(const pt &a) const { return x * a.y - y * a.x; }
        ll cross(const pt &a, const pt &b) const { return (a - *this).cross(b - *this); }
        bool operator==(const pt &a) const { return a.x == x && a.y == y; }
    };

    struct Edge {
        pt l, r;
    };

    bool edge_cmp(Edge *edge1, Edge *edge2) {
        const pt a = edge1->l, b = edge1->r;
        const pt c = edge2->l, d = edge2->r;
        int val = sgn(a.cross(b, c)) + sgn(a.cross(b, d));
        if (val != 0)
            return val > 0;
        val = sgn(c.cross(d, a)) + sgn(c.cross(d, b));
        return val < 0;
    }

    enum EventType {
        DEL = 2, ADD = 3, GET = 1, VERT = 0
    };

    struct Event {
        EventType type;
        int pos;

        bool operator<(const Event &event) const { return type < event.type; }
    };

    std::vector<Edge *> sweepline(std::vector<Edge *> planar, std::vector<pt> queries) {
        using pt_type = decltype(pt::x);

        // collect all x-coordinates
        auto s = std::set<pt_type, std::function<bool(const pt_type &, const pt_type &)>>(lt);
        for (pt p: queries)
            s.insert(p.x);
        for (Edge *e: planar) {
            s.insert(e->l.x);
            s.insert(e->r.x);
        }

        // map all x-coordinates to ids
        int cid = 0;
        auto id = std::map<pt_type, int, std::function<bool(const pt_type &, const pt_type &)>>(lt);
        for (auto x: s)
            id[x] = cid++;

        // create events
        auto t = std::set<Edge *, decltype(*edge_cmp)>(edge_cmp);
        auto vert_cmp = [](const std::pair<pt_type, int> &l, const std::pair<pt_type, int> &r) {
            if (!eq(l.first, r.first))
                return lt(l.first, r.first);
            return l.second < r.second;
        };
        auto vert = std::set<std::pair<pt_type, int>, decltype(vert_cmp)>(vert_cmp);
        std::vector<std::vector<Event>> events(cid);
        for (int i = 0; i < (int) queries.size(); i++) {
            int x = id[queries[i].x];
            events[x].push_back(Event{GET, i});
        }
        for (int i = 0; i < (int) planar.size(); i++) {
            int lx = id[planar[i]->l.x], rx = id[planar[i]->r.x];
            if (lx > rx) {
                std::swap(lx, rx);
                std::swap(planar[i]->l, planar[i]->r);
            }
            if (lx == rx) {
                events[lx].push_back(Event{VERT, i});
            } else {
                events[lx].push_back(Event{ADD, i});
                events[rx].push_back(Event{DEL, i});
            }
        }

        // perform sweep line algorithm
        std::vector<Edge *> ans(queries.size(), nullptr);
        for (int x = 0; x < cid; x++) {
            sort(events[x].begin(), events[x].end());
            vert.clear();
            for (Event event: events[x]) {
                if (event.type == DEL) {
                    t.erase(planar[event.pos]);
                }
                if (event.type == VERT) {
                    vert.insert(std::make_pair(std::min(planar[event.pos]->l.y, planar[event.pos]->r.y), event.pos));
                }
                if (event.type == ADD) {
                    t.insert(planar[event.pos]);
                }
                if (event.type == GET) {
                    auto jt = vert.upper_bound(std::make_pair(queries[event.pos].y, planar.size()));
                    if (jt != vert.begin()) {
                        --jt;
                        int i = jt->second;
                        if (ge(std::max(planar[i]->l.y, planar[i]->r.y), queries[event.pos].y)) {
                            ans[event.pos] = planar[i];
                            continue;
                        }
                    }
                    Edge *e = new Edge;
                    e->l = e->r = queries[event.pos];
                    auto it = t.upper_bound(e);
                    if (it != t.begin())
                        ans[event.pos] = *(--it);
                    delete e;
                }
            }

            for (Event event: events[x]) {
                if (event.type != GET)
                    continue;
                if (ans[event.pos] != nullptr &&
                    eq(ans[event.pos]->l.x, ans[event.pos]->r.x))
                    continue;

                Edge *e = new Edge;
                e->l = e->r = queries[event.pos];
                auto it = t.upper_bound(e);
                delete e;
                if (it == t.begin())
                    e = nullptr;
                else
                    e = *(--it);
                if (ans[event.pos] == nullptr) {
                    ans[event.pos] = e;
                    continue;
                }
                if (e == nullptr)
                    continue;
                if (e == ans[event.pos])
                    continue;
                if (id[ans[event.pos]->r.x] == x) {
                    if (id[e->l.x] == x) {
                        if (gt(e->l.y, ans[event.pos]->r.y))
                            ans[event.pos] = e;
                    }
                } else {
                    ans[event.pos] = e;
                }
            }
        }
        return ans;
    }

    struct DCEL {
        struct Edge {
            pt origin;
            Edge *nxt = nullptr;
            Edge *twin = nullptr;
            int face{};
        };
        std::vector<Edge *> body;
    };

    std::vector<std::pair<int, int>> point_location(DCEL planar, std::vector<pt> queries) {
        std::vector<std::pair<int, int>> ans(queries.size());
        std::vector<Edge *> planar2;
        std::map<intptr_t, int> pos;
        std::map<intptr_t, int> added_on;
        int n = planar.body.size();
        for (int i = 0; i < n; i++) {
            if (planar.body[i]->face > planar.body[i]->twin->face)
                continue;
            Edge *e = new Edge;
            e->l = planar.body[i]->origin;
            e->r = planar.body[i]->twin->origin;
            added_on[(intptr_t) e] = i;
            pos[(intptr_t) e] =
                    lt(planar.body[i]->origin.x, planar.body[i]->twin->origin.x)
                    ? planar.body[i]->face
                    : planar.body[i]->twin->face;
            planar2.push_back(e);
        }
        auto res = sweepline(planar2, queries);
        for (int i = 0; i < (int) queries.size(); i++) {
            if (res[i] == nullptr) {
                ans[i] = std::make_pair(1, -1);
                continue;
            }
            pt p = queries[i];
            pt l = res[i]->l, r = res[i]->r;
            if (eq(p.cross(l, r), 0) && le(p.dot(l, r), 0)) {
                ans[i] = std::make_pair(0, added_on[(intptr_t) res[i]]);
                continue;
            }
            ans[i] = std::make_pair(1, pos[(intptr_t) res[i]]);
        }
        for (auto e: planar2)
            delete e;
        return ans;
    }

} // namespace p_loc

int main() {
    auto data = read_csv("output_data_3.csv");

    int i = 1;
    while (i < data.size()) {
        auto id = data[i][1];

        std::vector<dPoint> coord;
        while (i < data.size() and id == data[i][1]) {
            coord.push_back({std::stod(data[i][3]), std::stod(data[i][4])});
            i += 1;
        }

        auto tri_data = triangulation(coord);
        std::cout << tri_data.size() << " ";
    }


//    std::vector<dPoint> coord = {
//            {1,    7},
//            {3,    1},
//            {8,    5},
//            {10,   2},
//            {13,   11},
//            {6,    10},
//            {4,    6},
//            {2.2,  5.5},
//            {4.2,  -0.5},
//            {9.2,  3.5},
//            {11.2, 0.5},
//            {14.2, 9.5},
//            {7.2,  8.5},
//            {5.2,  4.5}
//    };
//
//    auto tri_data = triangulation(coord);
}
