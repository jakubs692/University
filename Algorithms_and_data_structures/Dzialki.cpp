#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct SegmentTree {
private:
    vector<int> tree;
    int size;

    void build(const std::vector<int>& data, int node, int start, int end) {
        if (start == end) {
            tree[node] = data[start];
        } else {
            int mid = (start + end) / 2;
            build(data, 2 * node + 1, start, mid);
            build(data, 2 * node + 2, mid + 1, end);
            tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
        }
    }

    void add(int node, int start, int end, int idx, int val) {
        if (start == end) {
            tree[node] += val;
        } else {
            int mid = (start + end) / 2;
            if (start <= idx && idx <= mid) {
                add(2 * node + 1, start, mid, idx, val);
            } else {
                add(2 * node + 2, mid + 1, end, idx, val);
            }
            tree[node] = tree[2 * node + 1] + tree[2 * node + 2];
        }
    }

    int sum(int node, int start, int end, int L, int R) {
        if (R < start || end < L) {
            return 0; 
        }
        if (L <= start && end <= R) {
            return tree[node]; 
        }
        int mid = (start + end) / 2;
        int left_sum = sum(2 * node + 1, start, mid, L, R);
        int right_sum = sum(2 * node + 2, mid + 1, end, L, R);
        return left_sum + right_sum;
    }

public:
    SegmentTree(int n) {
        size = 1;
        while (size <= n) size *= 2;
        tree.assign(2 * size, 0);
    }

    void add(int idx, int val) {
        add(0, 0, size - 1, idx, val);
    }

    int sum(int L, int R) {
        return sum(0, 0, size - 1, L, R);
    }
};


enum EventType {
    Add_point,
    Begin, 
    End
};

struct Event {
    int y_coordinate;
    EventType t;
    int x_coordinate;

    bool operator<(const Event &other) const {
        if (y_coordinate < other.y_coordinate) return true;                                               
        if (y_coordinate > other.y_coordinate) return false;                                              
        return t < other.t;
    }
};

struct ParcelData {
    int count;
    int left_edge;
    int right_edge;
};

int main(void) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int n, m;
    cin >> n >> m;

    vector<Event> event_list; 
    vector<ParcelData> parcels;
    vector<int> coords;
    coords.reserve(n + 2 * m);
    
    int x, y;
    for (int i = 0; i < n; i++) {
        cin >> x >> y;
        coords.push_back(x);
        event_list.push_back({y, Add_point, x});
    }

    int x_l, x_r, y_b, y_t;
    for (int i = 0; i < m; i++) {
        cin >> x_l >> x_r >> y_b >> y_t;
        coords.push_back(x_l);
        coords.push_back(x_r);
        
        parcels.push_back({0, x_l, x_r});

        event_list.push_back({y_b - 1, Begin, i});
        event_list.push_back({y_t, End, i});
    }

    sort(coords.begin(), coords.end());
    coords.erase(unique(coords.begin(), coords.end()), coords.end());

    auto get_id = [&](int val) {
        return lower_bound(coords.begin(), coords.end(), val) - coords.begin();
    };

    sort(event_list.begin(), event_list.end());
    SegmentTree tree(coords.size());
    
    for (int i = 0; i < event_list.size(); i++) {
        if (event_list[i].t == Add_point) {
            int mapped = get_id(event_list[i].x_coordinate);
            tree.add(mapped, 1);
        }
        else if (event_list[i].t == Begin) {
            int idx = event_list[i].x_coordinate;
            int mapped_left = get_id(parcels[idx].left_edge);
            int mapped_right = get_id(parcels[idx].right_edge);

            parcels[idx].count -= tree.sum(mapped_left, mapped_right);
        }
        else {
            int idx = event_list[i].x_coordinate;
            int mapped_left = get_id(parcels[idx].left_edge);
            int mapped_right = get_id(parcels[idx].right_edge);

            parcels[idx].count += tree.sum(mapped_left, mapped_right);
        }
    }

    for (int i = 0; i < m; i++) {
        cout << parcels[i].count << "\n";
    }

    return 0;
}