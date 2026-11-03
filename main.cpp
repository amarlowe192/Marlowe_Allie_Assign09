#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <string>
using namespace std;

// ================= PART 1: MIN-HEAP =================
class MinHeap {
    vector<int> a; // raw storage (0-indexed): parent(i)=(i-1)/2, children 2i+1, 2i+2
    void heapifyUp(size_t i) {
        while (i > 0) {
            size_t p = (i - 1) / 2;
            if (a[i] >= a[p]) break;
            swap(a[i], a[p]);
            i = p;
        }
    }
    void heapifyDown(size_t i) {
        size_t n = a.size();
        while (true) {
            size_t l = 2 * i + 1, r = l + 1, s = i;
            if (l < n && a[l] < a[s]) s = l;
            if (r < n && a[r] < a[s]) s = r;
            if (s == i) break;
            swap(a[i], a[s]);
            i = s;
        }
    }
public:
    void insert(int v) { a.push_back(v); heapifyUp(a.size() - 1); }
    int peekMin() const {
        if (a.empty()) throw runtime_error("heap is empty");
        return a[0];
    }
    int removeMin() {
        if (a.empty()) throw runtime_error("heap is empty");
        int m = a[0];
        a[0] = a.back();
        a.pop_back();
        if (!a.empty()) heapifyDown(0);
        return m;
    }
    // bottom-up O(n) build
    void build(const vector<int>& v) {
        a = v;
        for (int i = (int)a.size() / 2 - 1; i >= 0; --i) heapifyDown(i);
    }
    size_t size() const { return a.size(); }
    bool empty() const { return a.empty(); }
    void print(const string& label) const {
        cout << label << ": [";
        for (size_t i = 0; i < a.size(); ++i) cout << (i ? ", " : "") << a[i];
        cout << "]\n";
    }
    bool valid() const {
        for (size_t i = 1; i < a.size(); ++i) if (a[i] < a[(i - 1) / 2]) return false;
        return true;
    }
};

// ================= PART 2: TREAP =================
struct TNode {
    int key, pri;
    TNode *left = nullptr, *right = nullptr;
    TNode(int k, int p) : key(k), pri(p) {}
};

class Treap {
    TNode* root = nullptr;
    // right rotation: left child becomes parent
    TNode* rotateRight(TNode* y) {
        TNode* x = y->left;
        y->left = x->right;
        x->right = y;
        return x;
    }
    // left rotation: right child becomes parent
    TNode* rotateLeft(TNode* x) {
        TNode* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }
    TNode* insert(TNode* t, int k, int p, bool& inserted) {
        if (!t) { inserted = true; return new TNode(k, p); }
        if (k == t->key) return t;                 // duplicate keys ignored
        if (k < t->key) {
            t->left = insert(t->left, k, p, inserted);
            if (t->left->pri < t->pri) t = rotateRight(t);   // min-heap on priority
        } else {
            t->right = insert(t->right, k, p, inserted);
            if (t->right->pri < t->pri) t = rotateLeft(t);
        }
        return t;
    }
    void inorder(TNode* t) const {
        if (!t) return;
        inorder(t->left);
        cout << "(" << t->key << "," << t->pri << ") ";
        inorder(t->right);
    }
    void show(TNode* t, int depth, const string& tag) const {
        if (!t) return;
        show(t->right, depth + 1, "R");
        cout << string(depth * 6, ' ') << tag << "[k=" << t->key << ", p=" << t->pri << "]\n";
        show(t->left, depth + 1, "L");
    }
    bool check(TNode* t, long lo, long hi) const {
        if (!t) return true;
        if (t->key <= lo || t->key >= hi) return false;
        if (t->left && t->left->pri < t->pri) return false;
        if (t->right && t->right->pri < t->pri) return false;
        return check(t->left, lo, t->key) && check(t->right, t->key, hi);
    }
    int height(TNode* t) const { return t ? 1 + max(height(t->left), height(t->right)) : 0; }
    void destroy(TNode* t) { if (t) { destroy(t->left); destroy(t->right); delete t; } }
public:
    ~Treap() { destroy(root); }
    bool insert(int k, int p) { bool ins = false; root = insert(root, k, p, ins); return ins; }
    const TNode* search(int k) const {
        TNode* t = root;
        while (t) {
            if (k == t->key) return t;
            t = k < t->key ? t->left : t->right;
        }
        return nullptr;
    }
    void printInorder() const { cout << "In-order (key,priority): "; inorder(root); cout << "\n"; }
    void printTree() const { cout << "Tree (rotated 90 deg; root at left, right subtree on top):\n"; show(root, 0, "root"); }
    bool valid() const { return check(root, -1000000000L, 1000000000L); }
    int height() const { return height(root); }
};

int main() {
    cout << "===== PART 1: MIN-HEAP =====\n";
    MinHeap h;
    vector<int> init = {42, 17, 8, 93, 25, 4, 61, 30, 12, 77, 3, 50};
    cout << "Input (12 ints): 42 17 8 93 25 4 61 30 12 77 3 50\n";
    h.build(init);
    h.print("After build (bottom-up)");
    cout << "peekMin = " << h.peekMin() << "\n";
    h.insert(1);  h.print("After insert(1)");
    h.insert(35); h.print("After insert(35)");
    h.insert(10); h.print("After insert(10)");
    cout << "removeMin -> " << h.removeMin() << "\n"; h.print("After removeMin");
    cout << "removeMin -> " << h.removeMin() << "\n"; h.print("After removeMin");
    cout << "removeMin -> " << h.removeMin() << "\n"; h.print("After removeMin");
    cout << "peekMin = " << h.peekMin() << ", valid heap? " << (h.valid() ? "yes" : "NO") << "\n";
    cout << "Drain in order: ";
    while (!h.empty()) cout << h.removeMin() << " ";
    cout << "\n";
    try { h.removeMin(); } catch (const exception& e) { cout << "removeMin on empty -> exception: " << e.what() << "\n"; }

    cout << "\n===== PART 2: TREAP (min-heap on priority) =====\n";
    Treap t;
    int ks[] = {50, 30, 70, 20, 40, 60, 80, 10, 45, 65, 35, 75};
    int ps[] = {35, 20, 60, 85, 10, 45, 90, 70, 5, 55, 25, 40};
    for (int i = 0; i < 12; ++i) {
        t.insert(ks[i], ps[i]);
        cout << "insert(" << ks[i] << "," << ps[i] << ") valid=" << (t.valid() ? "yes" : "NO") << "\n";
    }
    cout << "Duplicate key insert(40,1) accepted? " << (t.insert(40, 1) ? "yes" : "no") << "\n";
    t.printInorder();
    t.printTree();
    cout << "Height = " << t.height() << "\n";
    for (int k : {45, 80, 99, 5}) {
        const TNode* n = t.search(k);
        if (n) cout << "search(" << k << ") found, priority=" << n->pri << "\n";
        else cout << "search(" << k << ") not found\n";
    }
    // sorted-order insertion: shows treap stays balanced due to priorities
    Treap s;
    srand(7);
    for (int k = 1; k <= 1000; ++k) s.insert(k, rand());
    cout << "1000 sorted keys with random priorities: valid=" << (s.valid() ? "yes" : "NO")
         << ", height=" << s.height() << " (unbalanced BST would be 1000)\n";
}
