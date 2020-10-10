// compile: g++ task.cpp -o task --std=c++17

#include <iostream>
#include <memory>

class A {
 public:
// ...
  static void * operator new(size_t size) {
    std::cout << "operator new!" << std::endl;
    return ::operator new(size);
  }
  static void operator delete(void *p, size_t size) {
    std::cout << "operator delete!" << std::endl;
    return ::operator delete(p);
  }
};


#define METHOD_CHECKER(NAME, METHOD) \
template<typename T> \
class NAME { \
 private: \
  typedef char YesType[1]; \
  typedef char NoType[2]; \
  template<typename C> static YesType &test(decltype(&C::METHOD)); \
  template<typename C> static NoType &test(...); \
 public: \
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) }; \
}

METHOD_CHECKER(HasOpNew, operator new);
METHOD_CHECKER(HasOpDel, operator delete);


template<class T>
class CustomAlloc {

  void* (*opnew)(size_t) = ::operator new;
  void (*opdel)(void*, size_t) = ::operator delete;

  template<class C> void set_operators() {
    if constexpr (HasOpNew<C>::value) {
      opnew = C::operator new;
    }
    if constexpr (HasOpDel<C>::value) {
      opdel = C::operator delete;
    }
  }

 public:

  CustomAlloc() {
    set_operators<T>();
  }
  template <class C> CustomAlloc(const CustomAlloc<C> &) {
    set_operators<C>();
  }

  T *allocate(size_t s) {
    auto p = static_cast<T*>(opnew(s));
    if (!p) {
      throw std::bad_alloc();
    }
    return p;
  }

  void deallocate(T *p, size_t s) {
    opdel(p, s);
  }

  typedef T value_type;

};


int main() {
  auto sp = std::allocate_shared<A>(CustomAlloc<A>());
}

