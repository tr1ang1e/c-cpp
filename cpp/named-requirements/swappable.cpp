#include <iostream>


/* Swappable type
 *
 * Provides functionality which makes possible std::swap(_1, _2) call 
 * where _1 and _2 are this type objects references. Implementation:
 *      swap() method inside target class
 *      swap() overloading of std::swap()       // better if it is in the enclosing namespace
 * */


namespace me
{

    class S
    {
    private:
    
        int member;
    
    public:

        S(int i) : member{i} 
        {
        }

        void swap(S& other)
        {
            int temp = this->member;
            this->member = other.member;
            other.member = temp;
        }

        int get()
        {
            return member;
        }
    };

    void swap(S& _1, S& _2)
    {
        _1.swap(_2);
    }
}

int main(int argc, char** argv)
{
    me::S _1(1);
    me::S _2(2);
    std::cout << _1.get() << _2.get() << std::endl;     // 12

    std::swap(_1, _2);
    std::cout << _1.get() << _2.get() << std::endl;     // 21

    return 0;
}