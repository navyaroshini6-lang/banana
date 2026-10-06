HYBRID BANANAS – CORRECT C++17 SOLUTION

IMPORTANT CORRECTION:
In the input, "1 3" means SPOT 3 IS ABOVE SPOT 1.
So:
    1 -> 3  = climbing UP = cost 1
    3 -> 1  = climbing DOWN = cost 0

Earlier code had these costs reversed. This version is corrected.

--------------------------------------------------
SIMPLE IDEA
--------------------------------------------------

There are several banana trees.

For every connection:
    lower spot -> upper spot  : cost 1 (climb up)
    upper spot -> lower spot  : cost 0 (climb down)

If the same hanging spot exists in two different trees:
    switching trees at that spot costs 1.

So we create a graph:
    0-cost edge = go DOWN
    1-cost edge = go UP or switch tree

Because every edge has only 0 or 1 cost, use 0-1 BFS.

--------------------------------------------------
C++17 CODE
--------------------------------------------------

#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int to;
    int cost;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<vector<pair<int,int>>> treeEdges;
    vector<vector<int>> treeNodes;

    vector<pair<int,int>> edges;
    vector<int> nodes;

    for (int i = 0; i < N; i++) {
        string line;
        getline(cin, line);

        // Remove \r if input is Windows-style.
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line == "break") {
            treeEdges.push_back(edges);
            treeNodes.push_back(nodes);
            edges.clear();
            nodes.clear();
            continue;
        }

        stringstream ss(line);
        vector<int> a;
        int x;

        while (ss >> x)
            a.push_back(x);

        if (a.empty()) {
            continue;
        }

        // Example: 3 2 4
        // means 2 and 4 are ABOVE 3.
        int lower = a[0];

        nodes.push_back(lower);

        for (int j = 1; j < (int)a.size(); j++) {
            int upper = a[j];

            edges.push_back({lower, upper});
            nodes.push_back(upper);
        }
    }

    // Last tree may not have "break".
    if (!edges.empty() || !nodes.empty()) {
        treeEdges.push_back(edges);
        treeNodes.push_back(nodes);
    }

    int current, destination;
    cin >> current >> destination;

    int T = treeEdges.size();

    // Remove duplicate spots inside each tree.
    for (auto &v : treeNodes) {
        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
    }

    // Give every (tree, spot) a state number.
    map<pair<int,int>, int> id;
    vector<pair<int,int>> state;

    for (int t = 0; t < T; t++) {
        for (int spot : treeNodes[t]) {
            int newId = state.size();
            id[{t, spot}] = newId;
            state.push_back({t, spot});
        }
    }

    int V = state.size();

    vector<vector<Edge>> graph(V);

    // Build movement edges.
    for (int t = 0; t < T; t++) {
        for (auto [lower, upper] : treeEdges[t]) {

            int u = id[{t, lower}];
            int v = id[{t, upper}];

            // lower -> upper : CLIMB UP, cost 1
            graph[u].push_back({v, 1});

            // upper -> lower : CLIMB DOWN, cost 0
            graph[v].push_back({u, 0});
        }
    }

    // Same hanging spot may belong to multiple trees.
    // Switching trees costs 1.
    unordered_map<int, vector<int>> sameSpot;

    for (int i = 0; i < V; i++) {
        int spot = state[i].second;
        sameSpot[spot].push_back(i);
    }

    for (auto &entry : sameSpot) {
        vector<int> &states = entry.second;

        for (int i = 0; i < (int)states.size(); i++) {
            for (int j = i + 1; j < (int)states.size(); j++) {

                int u = states[i];
                int v = states[j];

                graph[u].push_back({v, 1});
                graph[v].push_back({u, 1});
            }
        }
    }

    // Find the starting state.
    // Problem says current spot is present on only one tree.
    int startState = -1;

    for (int t = 0; t < T; t++) {
        auto it = id.find({t, current});

        if (it != id.end()) {
            startState = it->second;
            break;
        }
    }

    if (startState == -1) {
        cout << -1 << '\n';
        return 0;
    }

    const int INF = 1e9;

    vector<int> dist(V, INF);
    deque<int> dq;

    dist[startState] = 0;
    dq.push_front(startState);

    // 0-1 BFS
    while (!dq.empty()) {

        int u = dq.front();
        dq.pop_front();

        for (Edge e : graph[u]) {

            if (dist[u] + e.cost < dist[e.to]) {

                dist[e.to] = dist[u] + e.cost;

                if (e.cost == 0)
                    dq.push_front(e.to);
                else
                    dq.push_back(e.to);
            }
        }
    }

    // Destination may exist in more than one tree.
    int answer = INF;

    for (int t = 0; t < T; t++) {

        auto it = id.find({t, destination});

        if (it != id.end()) {
            answer = min(answer, dist[it->second]);
        }
    }

    cout << answer << '\n';

    return 0;
}

--------------------------------------------------
WHY THIS IS CORRECT
--------------------------------------------------

Example:

1 3
3 2 4
4 5 6

This means:

        5     6
         \   /
          4
          |
          3
        /   \
       1     2

If monkey goes:

1 -> 3
3 -> 4
4 -> 6

all are UP movements.

Energy:
1 + 1 + 1 = 3

If it goes DOWN, the cost is 0.

If it changes from one tree to another at the same hanging spot,
add 1 energy.

Therefore 0-1 BFS gives the minimum possible energy.

TIME COMPLEXITY:
O(V + E) approximately for 0-1 BFS.

NOTE:
The important correction from the previous file is:
    lower -> upper = 1
    upper -> lower = 0
