# 🎯 C++ Interview Questions for Telecom

## 🎯 Why C++ in Telecom?

- **Performance:** Real-time requirements
- **Memory control:** Embedded systems
- **OOP:** Complex protocol stacks
- **Standards:** STL, modern C++ features

---

## 📝 Most Asked C++ Questions

### **Q1: Pointers vs References**

**Answer:**
"Pointers and References dono indirect access dete hain:

**Pointers:**
- Can be NULL
- Reassignable
- Pointer arithmetic
- Need dereferencing (*p)
- Can be uninitialized

**References:**
- Cannot be NULL
- NOT reassignable
- No arithmetic
- Direct access (no *)
- Must be initialized

```cpp
int x = 10;

// Pointer
int *p = &x;
*p = 20;  // Modifies x
p = nullptr;  // OK

// Reference
int &r = x;
r = 30;  // Modifies x
// r = nullptr;  // ERROR

// Function parameters
void modify_ptr(int *p) {
    if (p != nullptr) *p = 100;
}

void modify_ref(int &r) {
    r = 100;  // Always safe
}
```

**When to use what?**
- References: When NULL not valid, simpler syntax
- Pointers: When NULL valid, dynamic memory, arrays"

### **Q2: Stack vs Heap**

**Answer:**
"Memory allocation areas:

**Stack:**
- LIFO order
- Automatic allocation
- Function frames
- Limited size (~1-8 MB)
- Fast allocation
- Auto cleanup

**Heap:**
- Random access
- Manual allocation (new/delete)
- Large size (GBs)
- Slower allocation
- Manual cleanup (or smart pointers)

```cpp
void function() {
    int local_var = 10;          // Stack
    int *heap_var = new int(20); // Heap
    
    int arr1[100];               // Stack array
    int *arr2 = new int[100];    // Heap array
    
    delete heap_var;             // Manual cleanup
    delete[] arr2;
    
    // local_var, arr1 auto cleaned
}
```

**When to use Heap?**
- Large data
- Unknown size at compile time
- Returning from function
- Object lifetime > scope"

### **Q3: Virtual Function & Polymorphism**

**Answer:**
"Virtual functions enable runtime polymorphism:

**Without virtual:**
```cpp
class Base {
public:
    void print() { cout << "Base"; }
};

class Derived : public Base {
public:
    void print() { cout << "Derived"; }
};

Base *p = new Derived();
p->print();  // "Base" (compile-time binding)
```

**With virtual:**
```cpp
class Base {
public:
    virtual void print() { cout << "Base"; }
    virtual ~Base() = default;  // ALWAYS!
};

class Derived : public Base {
public:
    void print() override { cout << "Derived"; }
};

Base *p = new Derived();
p->print();  // "Derived" (runtime binding)
```

**vtable mechanism:**
- Each class with virtual has vtable
- Object has vptr
- Function call: vptr → vtable → function

**Pure virtual (Abstract):**
```cpp
class Shape {
public:
    virtual double area() = 0;  // Pure virtual
};

class Circle : public Shape {
public:
    double area() override { return 3.14 * r * r; }
};
```

**Always virtual destructor in base class!**"

### **Q4: Smart Pointers**

**Answer:**
"Smart pointers manage memory automatically:

**unique_ptr:**
```cpp
#include <memory>

unique_ptr<int> p1 = make_unique<int>(10);
// Auto delete when out of scope

// unique_ptr<int> p2 = p1;  // ERROR - no copy
unique_ptr<int> p2 = std::move(p1);  // OK - transfer

if (p1) {  // p1 is now nullptr
    cout << "Has value";
}
```

**shared_ptr:**
```cpp
shared_ptr<int> sp1 = make_shared<int>(20);
shared_ptr<int> sp2 = sp1;  // Copy OK

// Reference count: 2
cout << sp1.use_count();

// When all shared_ptrs go out of scope, deleted
```

**weak_ptr:**
```cpp
shared_ptr<int> sp = make_shared<int>(30);
weak_ptr<int> wp = sp;

// To use, lock first
if (auto locked = wp.lock()) {
    cout << *locked;
}

// Doesn't increment ref count
// Avoids circular references
```

**When to use?**
- unique_ptr: Single owner (most common)
- shared_ptr: Multiple owners
- weak_ptr: Break circular references

**Advantages:**
- No memory leaks
- Exception safe
- Clear ownership"

### **Q5: const correctness**

**Answer:**
"const ke 4 main uses:

**1. const variable:**
```cpp
const int x = 10;
// x = 20;  // ERROR
```

**2. const pointer:**
```cpp
int x = 10, y = 20;

const int *p1 = &x;     // pointer to const
*p1 = 100;              // ERROR
p1 = &y;                // OK

int *const p2 = &x;     // const pointer
*p2 = 100;              // OK
p2 = &y;                // ERROR

const int *const p3 = &x; // const pointer to const
*p3 = 100;              // ERROR
p3 = &y;                // ERROR
```

**3. const member function:**
```cpp
class MyClass {
    int x;
public:
    int get() const { return x; }  // doesn't modify
    void set(int v) { x = v; }     // modifies
};

const MyClass obj;
obj.get();  // OK
obj.set(5); // ERROR - obj is const
```

**4. const parameter:**
```cpp
void print(const string& s) {
    // Can read s, not modify
    cout << s;
}
```

**Why important?**
- Prevents bugs
- Compiler optimization
- Self-documenting
- Thread safety"

### **Q6: STL Containers**

**Answer:**
"Common STL containers and complexity:

**vector:**
```cpp
vector<int> v;
v.push_back(10);     // O(1) amortized
v.pop_back();        // O(1)
v[i];                // O(1)
v.insert(pos, val);  // O(n)
```

**list (doubly linked):**
```cpp
list<int> l;
l.push_front(10);    // O(1)
l.push_back(20);     // O(1)
l.insert(pos, val);  // O(1) given iterator
// No random access
```

**map (Red-Black Tree):**
```cpp
map<string, int> m;
m["key"] = 1;        // O(log n)
m.find("key");       // O(log n)
m.erase("key");      // O(log n)
```

**unordered_map (Hash Table):**
```cpp
unordered_map<string, int> um;
um["key"] = 1;       // O(1) avg, O(n) worst
um.find("key");      // O(1) avg
```

**queue:**
```cpp
queue<int> q;
q.push(10);          // O(1)
q.pop();             // O(1)
q.front();           // O(1)
```

**deque:**
```cpp
deque<int> dq;
dq.push_front(10);   // O(1)
dq.push_back(20);    // O(1)
dq[i];               // O(1)
```

**Telecom Use:**
- vector: Resource blocks list
- map: UE state by ID
- queue: Pending HARQ
- list: Logical channel buffers"

### **Q7: Move Semantics (C++11)**

**Answer:**
"Move semantics avoid expensive copies:

**Copy vs Move:**
```cpp
class String {
    char *data;
    size_t size;
public:
    // Copy constructor (deep copy)
    String(const String& other) {
        size = other.size;
        data = new char[size];
        memcpy(data, other.data, size);  // EXPENSIVE
    }
    
    // Move constructor (steal)
    String(String&& other) noexcept {
        data = other.data;        // Take pointer
        size = other.size;
        other.data = nullptr;     // Source empty
        other.size = 0;
    }
};

String getString();              // Returns by value
String s = getString();          // Move (no copy!)
```

**std::move:**
```cpp
String s1("Hello");
String s2 = std::move(s1);  // Move s1 into s2
// s1 is empty now
```

**rvalue references (&&):**
```cpp
void func(int& x);          // lvalue ref
void func(int&& x);         // rvalue ref

func(10);                   // calls rvalue version
int y = 20;
func(y);                    // calls lvalue version
func(std::move(y));         // calls rvalue version
```

**Use cases:**
- Return large objects from functions
- Transfer ownership
- Performance optimization
- Move-only types (unique_ptr)"

### **Q8: Multithreading**

**Answer:**
"C++11 threading basics:

**std::thread:**
```cpp
#include <thread>

void worker(int id) {
    cout << "Thread " << id;
}

int main() {
    thread t1(worker, 1);
    thread t2(worker, 2);
    
    t1.join();  // Wait for t1
    t2.join();
}
```

**std::mutex:**
```cpp
#include <mutex>

mutex mtx;
int shared_data = 0;

void increment() {
    lock_guard<mutex> lock(mtx);  // RAII
    shared_data++;
}  // Auto unlock
```

**std::atomic:**
```cpp
#include <atomic>

atomic<int> counter(0);

void thread_func() {
    counter++;  // Atomic increment
    counter.fetch_add(1);  // Same
}
```

**std::condition_variable:**
```cpp
mutex mtx;
condition_variable cv;
bool ready = false;

void wait_thread() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, []{ return ready; });
    // Process when ready
}

void signal_thread() {
    {
        lock_guard<mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one();
}
```

**Telecom Use:**
- HARQ processes (parallel)
- Multiple UE handling
- Background tasks"

### **Q9: RAII (Resource Acquisition Is Initialization)**

**Answer:**
"RAII = constructor acquires, destructor releases.

**Concept:**
- Objects manage resources
- Auto cleanup when out of scope
- Exception safe

**Example:**
```cpp
class FileHandle {
    FILE *fp;
public:
    FileHandle(const char *name) {
        fp = fopen(name, "r");
        if (!fp) throw runtime_error("Failed");
    }
    
    ~FileHandle() {
        if (fp) fclose(fp);  // Auto cleanup
    }
};

void read_file() {
    FileHandle f("data.txt");
    // Use f
    // Auto fclose when f goes out of scope
    // Even if exception thrown!
}
```

**Common RAII classes:**
- unique_ptr / shared_ptr (memory)
- lock_guard / unique_lock (mutex)
- ifstream / ofstream (files)

**Why important?**
- No resource leaks
- Exception safety
- Self-cleaning code
- Modern C++ idiom"

### **Q10: Memory Management Best Practices**

**Answer:**
"Memory management in modern C++:

**1. Prefer Stack over Heap:**
```cpp
// BAD
int *arr = new int[100];
// ... use ...
delete[] arr;

// GOOD
array<int, 100> arr;  // Stack
// or
vector<int> arr(100);  // Heap, auto manage
```

**2. RAII Always:**
```cpp
// BAD
int *p = new int(10);
function_might_throw();
delete p;  // Leaked if throw!

// GOOD
unique_ptr<int> p = make_unique<int>(10);
function_might_throw();
// Auto cleanup
```

**3. Rule of Five (or Zero):**
```cpp
class MyClass {
public:
    // If you define one, define all five:
    ~MyClass();
    MyClass(const MyClass&);
    MyClass& operator=(const MyClass&);
    MyClass(MyClass&&);
    MyClass& operator=(MyClass&&);
    
    // OR define none (Rule of Zero)
    // Use default + smart pointers
};
```

**4. Avoid Raw new/delete:**
```cpp
// BAD
MyClass *obj = new MyClass();
delete obj;

// GOOD
auto obj = make_unique<MyClass>();
// Auto cleanup
```

**5. Memory Profiling:**
- Valgrind: `valgrind --leak-check=full ./prog`
- AddressSanitizer: `g++ -fsanitize=address`
- Static analysis: clang-tidy"

---

## 🎯 Telecom-Specific Coding

### **Q11: Implement Linked List**

```cpp
template<typename T>
class LinkedList {
    struct Node {
        T data;
        Node *next;
        Node(T val) : data(val), next(nullptr) {}
    };
    
    Node *head;
    
public:
    LinkedList() : head(nullptr) {}
    
    ~LinkedList() {
        while (head) {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
    }
    
    void push_front(T val) {
        Node *node = new Node(val);
        node->next = head;
        head = node;
    }
    
    bool find(T val) {
        Node *curr = head;
        while (curr) {
            if (curr->data == val) return true;
            curr = curr->next;
        }
        return false;
    }
};
```

### **Q12: Implement Circular Buffer (HARQ Soft Buffer)**

```cpp
template<typename T, size_t N>
class CircularBuffer {
    T buffer[N];
    size_t head;  // Read position
    size_t tail;  // Write position
    size_t count;
    
public:
    CircularBuffer() : head(0), tail(0), count(0) {}
    
    bool push(T val) {
        if (count == N) return false;  // Full
        buffer[tail] = val;
        tail = (tail + 1) % N;
        count++;
        return true;
    }
    
    bool pop(T& val) {
        if (count == 0) return false;  // Empty
        val = buffer[head];
        head = (head + 1) % N;
        count--;
        return true;
    }
    
    bool full() { return count == N; }
    bool empty() { return count == 0; }
};

// Usage in HARQ:
CircularBuffer<HarqProcess, 8> harq_processes;
```

### **Q13: State Machine (RACH)**

```cpp
class RachStateMachine {
public:
    enum State {
        INIT,
        PREAMBLE_TX,
        WAIT_RAR,
        MSG3_TX,
        WAIT_MSG4,
        SUCCESS,
        FAILED
    };
    
private:
    State current_state;
    int retry_count;
    int max_retries;
    
public:
    RachStateMachine() : current_state(INIT), retry_count(0), max_retries(10) {}
    
    void process_event(int event) {
        switch (current_state) {
            case INIT:
                if (event == TRIGGER_RACH) {
                    send_preamble();
                    current_state = PREAMBLE_TX;
                }
                break;
                
            case PREAMBLE_TX:
                current_state = WAIT_RAR;
                start_rar_timer();
                break;
                
            case WAIT_RAR:
                if (event == RAR_RECEIVED) {
                    send_msg3();
                    current_state = MSG3_TX;
                } else if (event == TIMER_EXPIRED) {
                    if (++retry_count > max_retries) {
                        current_state = FAILED;
                    } else {
                        send_preamble();
                        current_state = PREAMBLE_TX;
                    }
                }
                break;
            
            // ... more states
        }
    }
};
```

---

## 🎯 Real Interview Coding Questions

### **Q: Reverse a Linked List (in C++)**

```cpp
struct Node {
    int data;
    Node *next;
};

Node* reverse(Node *head) {
    Node *prev = nullptr;
    Node *curr = head;
    
    while (curr) {
        Node *next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    
    return prev;
}
```

### **Q: Find Cycle in Linked List**

```cpp
bool has_cycle(Node *head) {
    Node *slow = head;
    Node *fast = head;
    
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) return true;
    }
    
    return false;
}
```

### **Q: Implement Queue using Two Stacks**

```cpp
class Queue {
    stack<int> in_stack;
    stack<int> out_stack;
    
public:
    void push(int val) {
        in_stack.push(val);
    }
    
    int pop() {
        if (out_stack.empty()) {
            while (!in_stack.empty()) {
                out_stack.push(in_stack.top());
                in_stack.pop();
            }
        }
        
        int val = out_stack.top();
        out_stack.pop();
        return val;
    }
};
```

---

## ✅ Self-Test

1. ✅ Pointer vs Reference?
2. ✅ Stack vs Heap?
3. ✅ Virtual function mechanism?
4. ✅ unique_ptr vs shared_ptr?
5. ✅ const correctness uses?
6. ✅ STL container complexities?
7. ✅ Move semantics?
8. ✅ Mutex vs atomic?
9. ✅ RAII concept?
10. ✅ Memory leak prevention?

---

## 🚀 Next File:
**Read:** `10_BEHAVIORAL_HR.md`
