# Level 7: C/C++ Technical Questions

> JD: "C/C++— Expert (build, configure, debug OCUDU/srsRAN/Open5GS)". Both roles need strong C/C++.

---

## SECTION A: C Programming Basics (1-15)

### Q1. What is the difference between C and C++?
**Answer:**
- **C:** Procedural, no OOP, no STL, faster compile.
- **C++:** OOP, templates, STL, exceptions, references.

Open5GS is C. srsRAN Project is C++.

### Q2. What is the difference between `malloc` and `calloc`?
**Answer:**
- `malloc(size)`: Allocates memory, uninitialized.
- `calloc(n, size)`: Allocates and zeroes memory.

### Q3. What is a memory leak? How to find it?
**Answer:** Memory allocated but never freed. Tools:
- **Valgrind:** `valgrind --leak-check=full ./app`
- **AddressSanitizer:** Compile with `-fsanitize=address`
- **Static analysis:** clang-tidy, cppcheck.

### Q4. What is a dangling pointer?
**Answer:** Pointer pointing to freed memory. Bad - causes crash or undefined behavior.
```c
int *p = malloc(4);
free(p);
*p = 10;  // BUG! dangling pointer
```
Fix: `p = NULL;` after `free`.

### Q5. What is a null pointer vs uninitialized pointer?
**Answer:**
- **Null pointer:** `p = NULL`, intentionally points to nothing.
- **Uninitialized pointer:** Has garbage value. Reading/writing crashes.

### Q6. What is the difference between pass by value, reference, pointer?
**Answer:**
- **Value:** Copy is made. Changes don't affect original.
- **Pointer:** Address passed. Changes affect original.
- **Reference (C++):** Alias for original. Like pointer but cleaner syntax.

### Q7. What is the difference between `struct` and `union`?
**Answer:**
- **struct:** All members have own memory. Size = sum.
- **union:** All members share same memory. Size = largest.

Union useful for tagged data, protocol headers with variants.

### Q8. What is endianness?
**Answer:**
- **Little-endian:** LSB first (x86). 0x12345678 stored as 78 56 34 12.
- **Big-endian:** MSB first (network order). 0x12345678 as 12 34 56 78.

Use `htonl()`, `ntohl()` to convert.

### Q9. What is bit field?
**Answer:** Struct member using specific bits:
```c
struct GtpHdr {
  uint8_t version:3;
  uint8_t PT:1;
  uint8_t reserved:1;
  uint8_t E:1;
  uint8_t S:1;
  uint8_t PN:1;
};
```
Useful for protocol headers. But compiler-dependent layout!

### Q10. How to pack a struct (no padding)?
**Answer:**
```c
struct __attribute__((packed)) GtpHdr {
  uint8_t flags;
  uint8_t msg_type;
  uint16_t length;
  uint32_t teid;
};
```
Or `#pragma pack(1)`. Important for network protocol parsing.

### Q11. What is `volatile`?
**Answer:** Tells compiler "this variable can change unexpectedly" (e.g., hardware register, signal handler). Disables optimization.

### Q12. What is `static` variable?
**Answer:**
- **Inside function:** Keeps value between calls. Initialized once.
- **At file scope:** Internal linkage (not visible outside file).

### Q13. What is `extern`?
**Answer:** Declares variable defined in another file. Used in headers.
```c
// header.h
extern int counter;
// file.c
int counter = 0;
```

### Q14. What is the difference between `#define` and `const`?
**Answer:**
- `#define MAX 100`: Preprocessor text replacement. No type. No scope.
- `const int MAX = 100;`: Real variable. Type-checked. Has scope.

### Q15. What is `inline` function?
**Answer:** Hint to compiler to insert function body at call site (no call overhead). Compiler may ignore. Used for small, hot functions.

---

## SECTION B: C++ Specific (16-30)

### Q16. What are the four pillars of OOP?
**Answer:**
1. **Encapsulation:** Bundle data + methods, hide internals.
2. **Inheritance:** Class inherits from parent.
3. **Polymorphism:** Same interface, different behavior (virtual functions).
4. **Abstraction:** Hide complex details behind simple interface.

### Q17. What is a virtual function?
**Answer:** Function in base class that can be overridden in derived. Resolved at runtime via vtable.
```cpp
class Base {
  virtual void hello() { cout << "Base"; }
};
class Derived : public Base {
  void hello() override { cout << "Derived"; }
};
Base *p = new Derived();
p->hello();  // Prints "Derived"
```

### Q18. What is pure virtual function?
**Answer:** Virtual function with no body. Makes class abstract (can't instantiate).
```cpp
class Shape {
  virtual void draw() = 0;  // pure virtual
};
```

### Q19. What is the difference between `new` and `malloc`?
**Answer:**
- `malloc`: C. Allocates raw memory. Returns void*. No constructor.
- `new`: C++. Allocates + calls constructor. Returns typed pointer. Throws on failure (or returns nullptr with std::nothrow).

### Q20. What is RAII?
**Answer:** Resource Acquisition Is Initialization. Tie resource to object lifetime. Resource acquired in constructor, released in destructor. Example: `std::lock_guard`, `std::unique_ptr`.

### Q21. What is smart pointer? Types?
**Answer:** C++ wrapper that auto-frees memory.
- **`unique_ptr`:** Exclusive ownership. Cheap.
- **`shared_ptr`:** Shared ownership with ref count.
- **`weak_ptr`:** Non-owning, used to break cycles.

### Q22. When to use unique_ptr vs shared_ptr?
**Answer:**
- **unique_ptr:** When ownership is clear. Most cases.
- **shared_ptr:** When multiple objects need ownership. Ref counting has cost.

### Q23. What is `std::move`?
**Answer:** Casts to rvalue, enabling move semantics. Transfers ownership without copy.
```cpp
std::unique_ptr<Foo> a = std::make_unique<Foo>();
std::unique_ptr<Foo> b = std::move(a);  // a is now empty
```

### Q24. What is lambda?
**Answer:** Anonymous function in C++:
```cpp
auto add = [](int a, int b) { return a + b; };
auto x = add(2, 3);
```
Capture: `[=]` by value, `[&]` by reference, `[this]` capture this.

### Q25. What is the difference between `const T&` and `T&&`?
**Answer:**
- `const T&`: lvalue reference, can bind to anything.
- `T&&`: rvalue reference, only binds to rvalues (temporaries). Used for move semantics.

### Q26. What is template?
**Answer:** Generic programming. Compile-time polymorphism.
```cpp
template<typename T>
T max(T a, T b) { return a > b ? a : b; }
```
Used heavily in STL.

### Q27. What is the difference between vector and list?
**Answer:**
- **vector:** Dynamic array. Random access O(1). Insert/delete middle O(n).
- **list:** Doubly linked list. Random access O(n). Insert/delete O(1) if iterator.

### Q28. What is the difference between map and unordered_map?
**Answer:**
- **map:** Red-black tree. Sorted. Lookup O(log n).
- **unordered_map:** Hash table. Unordered. Lookup O(1) average.

### Q29. What is exception handling?
**Answer:**
```cpp
try {
  throw std::runtime_error("error");
} catch (const std::exception& e) {
  std::cerr << e.what();
}
```

In high-perf telecom code, exceptions often avoided (use error codes).

### Q30. What is `noexcept`?
**Answer:** Says function doesn't throw exceptions. Helps optimizer. Move constructors should be noexcept for vector to use them.

---

## SECTION C: Multi-threading and Concurrency (31-40)

### Q31. What is a thread?
**Answer:** Lightweight unit of execution within a process. Shares memory with other threads.

### Q32. What is the difference between thread and process?
**Answer:**
- **Process:** Own memory space. Heavyweight. Communication via IPC.
- **Thread:** Shares memory with siblings. Lightweight. Easy data sharing.

### Q33. How to create a thread in C++?
**Answer:**
```cpp
#include <thread>
void func() { ... }
std::thread t(func);
t.join();        // Wait for completion
// Or t.detach();
```

### Q34. What is a mutex?
**Answer:** Mutual exclusion. One thread at a time can lock it.
```cpp
std::mutex mtx;
mtx.lock();
// critical section
mtx.unlock();
// Better: std::lock_guard<std::mutex> g(mtx);
```

### Q35. What is a race condition?
**Answer:** Two threads access shared data concurrently, at least one writes. Result depends on timing. Bug-prone.

### Q36. What is a deadlock?
**Answer:** Two threads waiting for each other's lock. Both stuck forever.

Prevention:
- Always lock in same order.
- Use `std::lock(m1, m2)` for multi-lock.
- Timeouts.

### Q37. What is condition variable?
**Answer:** Thread synchronization. Wait until condition is true.
```cpp
std::condition_variable cv;
std::mutex mtx;
std::unique_lock<std::mutex> lock(mtx);
cv.wait(lock, []{ return ready; });
```

### Q38. What is atomic?
**Answer:** `std::atomic<T>` provides thread-safe variable access without explicit lock. CPU does atomic instructions.
```cpp
std::atomic<int> counter(0);
counter++;  // thread-safe
```

### Q39. What is producer-consumer pattern?
**Answer:** Producer adds items to queue, consumer removes. Used heavily in telecom (e.g., RLC TX queue).
- Use mutex + condition variable.
- Or lock-free queue for low latency.

### Q40. What is lock-free programming?
**Answer:** Multi-threaded code without locks. Uses atomic operations (CAS - Compare-And-Swap). Very fast but very hard to write correctly. Used in DPDK, ring buffers.

---

## SECTION D: Build Tools and Debugging (41-50)

### Q41. What is CMake?
**Answer:** Cross-platform build system. Generates Makefiles (or Ninja, VS projects, Xcode).
```cmake
cmake_minimum_required(VERSION 3.10)
project(myapp)
add_executable(myapp main.cpp helper.cpp)
target_link_libraries(myapp pthread)
```

### Q42. CMake build commands?
**Answer:**
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
# Or
cmake --build .
```

### Q43. What is Meson and Ninja?
**Answer:**
- **Meson:** Build system (like CMake), generates Ninja files.
- **Ninja:** Fast build tool (replaces make).

Open5GS uses Meson + Ninja:
```bash
meson build
ninja -C build
```

### Q44. What is gdb?
**Answer:** GNU Debugger. Step through C/C++ code, inspect variables, set breakpoints.
```bash
gdb ./myapp
(gdb) run
(gdb) break main
(gdb) next     # step over
(gdb) step     # step into
(gdb) print x
(gdb) bt       # backtrace
(gdb) attach <pid>
```

### Q45. How to debug a core dump?
**Answer:**
```bash
ulimit -c unlimited                    # Enable core dumps
./myapp                                # Crash creates core file
gdb ./myapp core
(gdb) bt                               # See where it crashed
```

### Q46. What is Valgrind?
**Answer:** Memory error detector and profiler.
```bash
valgrind --leak-check=full ./myapp
valgrind --tool=callgrind ./myapp   # Profile
```

### Q47. What is AddressSanitizer (ASAN)?
**Answer:** Compile-time memory checker. Faster than Valgrind.
```bash
g++ -fsanitize=address -g main.cpp -o app
./app   # Crashes with detailed report on bug
```

### Q48. How to profile a C++ program?
**Answer:**
- `perf` (Linux): `perf record ./app && perf report`.
- `gprof`: compile with `-pg`.
- `callgrind` (Valgrind).
- `strace -c ./app` for syscalls.

### Q49. What is Git? Basic commands?
**Answer:**
```bash
git clone https://...
git status
git diff
git add file.c
git commit -m "fix bug"
git push origin main
git pull
git branch feature-x
git checkout feature-x
git merge main
git log --oneline
```

### Q50. What is the difference between `git merge` and `git rebase`?
**Answer:**
- **merge:** Creates merge commit. Preserves history.
- **rebase:** Replays commits on top of base. Linear history. Can rewrite history (don't rebase shared branches!).

---

## SECTION E: Common Coding Questions (51-60) — BONUS

### Q51. Write a function to swap two integers without temp.
**Answer:**
```c
void swap(int *a, int *b) {
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}
```

### Q52. Write a function to reverse a string in place.
**Answer:**
```c
void reverse(char *s) {
    int n = strlen(s);
    for (int i = 0, j = n-1; i < j; i++, j--) {
        char t = s[i]; s[i] = s[j]; s[j] = t;
    }
}
```

### Q53. Detect endianness.
**Answer:**
```c
int is_little_endian() {
    uint16_t x = 1;
    return *(uint8_t*)&x == 1;
}
```

### Q54. Reverse bits of a byte.
**Answer:**
```c
uint8_t reverse_byte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}
```

### Q55. Count set bits in integer.
**Answer:**
```c
int popcount(uint32_t n) {
    int count = 0;
    while (n) { count++; n &= n - 1; }
    return count;
}
// Or use __builtin_popcount(n) (GCC)
```

### Q56. Parse a GTP-U header.
**Answer:**
```c
struct __attribute__((packed)) gtpu_hdr {
    uint8_t flags;
    uint8_t msg_type;
    uint16_t length;
    uint32_t teid;
};

void parse(uint8_t *buf) {
    struct gtpu_hdr *h = (struct gtpu_hdr*)buf;
    uint8_t version = (h->flags >> 5) & 0x07;
    uint8_t pt = (h->flags >> 4) & 0x01;
    uint8_t e = (h->flags >> 2) & 0x01;
    uint16_t len = ntohs(h->length);
    uint32_t teid = ntohl(h->teid);
    printf("Ver=%u PT=%u E=%u Type=0x%02x Len=%u TEID=0x%08x\n",
           version, pt, e, h->msg_type, len, teid);
}
```

### Q57. Simple TCP echo server in C.
**Answer:**
```c
int s = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in addr = {.sin_family=AF_INET, .sin_port=htons(9000),
                           .sin_addr.s_addr=INADDR_ANY};
bind(s, (struct sockaddr*)&addr, sizeof(addr));
listen(s, 5);
while (1) {
    int c = accept(s, NULL, NULL);
    char buf[1024];
    int n = read(c, buf, sizeof(buf));
    write(c, buf, n);
    close(c);
}
```

### Q58. Simple SCTP listener.
**Answer:**
```c
int s = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
struct sockaddr_in addr = {.sin_family=AF_INET, .sin_port=htons(38412),
                           .sin_addr.s_addr=INADDR_ANY};
bind(s, (struct sockaddr*)&addr, sizeof(addr));
listen(s, 5);
int c = accept(s, NULL, NULL);
// Use sctp_recvmsg() for proper SCTP semantics
```

### Q59. Compute Internet checksum.
**Answer:**
```c
uint16_t checksum(uint16_t *data, int len) {
    uint32_t sum = 0;
    while (len > 1) { sum += *data++; len -= 2; }
    if (len) sum += *(uint8_t*)data;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~sum;
}
```

### Q60. Linked list reverse.
**Answer:**
```c
struct node { int val; struct node *next; };

struct node* reverse(struct node *head) {
    struct node *prev = NULL, *curr = head, *next;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
```

---

## END OF LEVEL 7 (60 questions)

> Move to **08_QOS_DRB_SLICING.md**.
