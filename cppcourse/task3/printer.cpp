// compile: g++ printer.cpp -o printer --std=c++17 -g

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <tuple>
#include <utility>
#include <sstream>

struct Printer {

 private:

  std::stringstream stream;

 public:

  std::string str() const
  {
    return stream.str();
  }

  template<typename T>
  Printer& format(const T &t)
  {
    stream << t;
    return *this;
  }

 private:

  template<template <typename...> typename Container>
  std::pair<std::string,std::string> brace();
  
 public:

  template<template <typename...> typename Container, typename T,
    class = typename std::enable_if<!std::is_same<Container<T>,std::basic_string<T>>::value>::type >
  Printer& format(const Container<T> &c)
  {
    format(brace<Container>().first + " ");
    auto it = c.begin();
    while (true) {
      if (it == c.end()) {
        break;
      }
      if (it != c.begin()) {
        format(", ");
      }
      format(*it);
      ++it;
    }
    format(" " + brace<Container>().second);
    return *this;
  }

  template<typename T1, typename T2>
  Printer& format(const std::pair<T1,T2> &v)
  {
    format("( ").format(v.first).format(",. ").format(v.second).format(" )");
    return *this;
  }

private:

  template<size_t I, typename... Ts>
  void format_tuple(const std::tuple<Ts...> &t)
  {
    if constexpr (I < std::tuple_size<std::tuple<Ts...>>::value) {
      if (I > 0) {
        format(", ");
      }
      format(std::get<I>(t));
      format_tuple<I+1>(t);
    }
  }

public:

  template<typename... Ts>
  Printer& format(const std::tuple<Ts...> &t)
  {
    format("( ");
    format_tuple<0>(t);
    format(" )");
    return *this;
  }

};

template<template <typename...> typename Container>
std::pair<std::string,std::string> Printer::brace()
{
  return {"(", ")"};
}

template<>
std::pair<std::string,std::string> Printer::brace<std::vector>()
{
  return {"[", "]"};
}

template<>
std::pair<std::string,std::string> Printer::brace<std::set>()
{
  return {"{", "}"};
}

template<typename T>
std::string format(const T& t) {
  return Printer().format(t).str();
}

int main() {
  std::tuple<std::string, int, int> t = {"xyz", 1, 2};
  std::vector<std::pair<int, int> > v = {{1, 4}, {5, 6}};
  std::string s1 = Printer().format(" vector: ").format(v).str();
  std::cout << s1 << std::endl;
  // " vector: [ (1, 4), (5, 6) ]"
  std::string s2 = Printer().format(t).format(" ! ").format(0).str();
  std::cout << s2 << std::endl;
  // "( xyz, 1, 2 ) ! 0"
}

