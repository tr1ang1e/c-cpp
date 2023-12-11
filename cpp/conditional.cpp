#include <iostream>


template <bool Condition, typename F, typename S>
struct conditional
{
    using type = F;
};

template <typename F, typename S>
struct conditional<false, F, S>
{
    using type = S;
};

template <bool Condition, typename F, typename S>
using conditional_t = typename conditional<Condition, F, S>::type;


int main()
{
    std::cout << typeid(conditional_t<true, int, double>).name() << std::endl;
    std::cout << typeid(conditional_t<false, int, double>).name() << std::endl;

    return 0;
}
