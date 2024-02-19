#pragma once

namespace util{

template <class... fs> struct overload : fs... { using fs::operator()...; };
template <class... fs> overload(fs...) -> overload<fs...>;

};
