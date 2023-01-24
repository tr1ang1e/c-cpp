#include <iostream>
#include <cstring>
#include <vector>


/* ITERATORS
 * 
 * Итераторы обеспечивают доступ к элементам контейнера. Компилятор
 * использует принцип утиной типизации, чтобы определить 
 *      - сам итератор
 *      - тип итератора
 *
 * Простейший интерфейс для некоторого шаблонного класса S<T> с итератором ITER:
 *      ITER S::begin(...);
 *      ITER S::end(...);
 *      bool S::ITER::operator!=(S::ITER&);
 *      void S::ITER::operator++( );
 *        T& S::ITER::operator*( );  
 *
 * 
 * __1__ : iter and citer
 *
 * iter   =  итератор, при разыменовании которого возвращается не-константная ссылка на подлежащий объект
 * сiter  =  итератор, при разыменовании которого возвращается    константная ссылка на подлежащий объект
 * [!] константность относится именно к ссылке на подлежащий объект, а не к самому итератору. Сам по себе
 *     итератор не должен быть константным т.к. это противоречит его сути итерации по подлежащим объектам
 * 
 * Реализуеся с использованием std::conditional из библиотеки <type_traits>
 * см. полный пример реализации ниже
 * 
 *      using iter = ITER<false>;           using citer = ITER<true>;
 *      T& operator*();                     const T& operator*();
 *      iter begin();                       citer cbegin();
 *      ...                                 ...
 * 
 *
 * __2__ : std::default_sentinel_t
 *       = " ... an empty class type used to denote the end of a range"
 *       https://en.cppreference.com/w/cpp/iterator/default_sentinel_t
 *
 * если оператор != позволяет определить окончание итараций 
 * без использования второго итератора то метод end() может 
 * также быть определен через возврат std::default_sentinel_t
 *      
 *  <...>
 *      bool S::ITER::operator!= (std::default_sentinel_t) { return this->_string[this->_index] != '\0'; }
 *      std::default_sentinel_t S::end( ) { return std::default_sentinel; }
 *  <...>
 *
 *
 * __3__ : iterator categories
 *         https://en.cppreference.com/w/cpp/iterator         
 *
 * Любой итератор соответствует одной категории. Категории определяются
 * не принадлежностью конкретному классу (иерархии наследования для
 * итераторов не существует), а набором операций, предоставляемых
 * интерфейсом конкретного итератора. Каждый следующий в "иерархии" 
 * итераторов расширяет функциональность предыдущего
 *
 *      input           =   один проход  |  operator++  |  operator==  |  operator*  read-only  |  swappable
 *      output          =   один проход  |  operator++  |  operator==  |  operator* write-only  |  swappable
 *      forward         =   неограниченное число проходов                                                           std::forward_list,unordered_map,unordered_set
 *      bidirectional   =   operator--                                                                              std::list,map,set
 *      random access   =   operator+=   |  operator-=  |  сравнения   |  разность итераторов                       std::deque
 *      contiguous      =   гарантия, что объекты под итератором последовательно в памяти                           std::vector,array + [!] POINTER  
 *
 * Для определения категории:  
 * https://en.cppreference.com/w/cpp/iterator/iterator_tags (пример для обоих вариантов)
 *      
 *      > можно использовать Iterator concepts из <concept>
 *      > можно использовать struct Iterator_traits из <iterator_traits>      
 *              информация о других доступных - помимо категории - данных: 
 *              std::iterator_traits<Iterator>::*information*
 *              https://en.cppreference.com/w/cpp/iterator/iterator_traits
 *  
 * */


// iterator implementation example = see main() for usage example
template <typename T, size_t Size>
class Array
{
    T array[Size];

    template <bool IsConst>
    class Iter
    {
        T* current;

    public:

        Iter(T* element) : current{element}
        {
        }
        
        std::conditional_t<IsConst, const T&, T&> operator*()
        {
            return *current;
        }

        bool operator!=(const Iter& other)
        {
            return this->current != other.current;
        }

        void operator++()
        {
            ++current;
        }
    };

public:

    Array (std::initializer_list<T> values) 
    {
        if (values.size() > Size)
        {
            throw std::invalid_argument("Wrong initializer list size");
        }

        size_t idx = 0;
        for (const auto& iter : values)
        {
            this->array[idx] = iter;
            ++idx;
        }
    }

    using iter = Iter<false>;
    using citer = Iter<true>;

    iter begin()
    {
        return iter(array);
    }

    iter end()
    {
        return iter(&array[Size]);
    }

    citer cbegin()
    {
        return citer(array);
    }

    citer cend()
    {
        return citer(&array[Size]);
    }
};


int main()
{
    using ExampleArray = Array<int, 4>;
    ExampleArray arr = {0, 1, 2, 3};

    // before changes
    ExampleArray::iter begin = arr.begin();
    ExampleArray::iter end = arr.end();
    std::cout << "non-const iterator, iterate and read: ";
    while (begin != end)
    {
        std::cout << *begin;
        ++begin;
    }
    std::cout << std::endl;

    // change values using non-const iterator
    begin = arr.begin();
    int newValue = 4;
    std::cout << "non-const interator, iterate and write... ";
    while (begin != end)
    {
        *begin = newValue;
        ++begin;
        ++newValue;
    }
    std::cout << std::endl;

    // after changes
    auto cbegin = arr.cbegin();
    auto cend = arr.cend();
    std::cout << "const iterator, iterate and read: ";
    while (cbegin != cend)
    {
        std::cout << *cbegin;
        ++cbegin;
    }
    std::cout << std::endl;

    // try to change value using const interator = uncomment to get compile error
    // cbegin = arr.cbegin();
    // *cbegin = 0;


    /* __4__ : iterator invalidation
     *
     * При работе с итераторами из-за внутренней реализации 
     * контейнеров возможно возникновение инвалидации итератора: 
     *
     *      1) инвалидация элемента = адрес под итератором валиден, однако по этому адресу уже другой элемент
     *      
     *              iterator => element I
     *              *do_something_with_container*
     *              iterator => element X
     *
     *      2) инвалидация адреса = сам адрес под итератором становится невалидным
     *
     *              iterator => element I
     *              *do_something_with_container*
     *              iterator => invalid address in memory
     *
     *
     *                          Инвалидация элемента        Инвалидация адреса
     *
     * std::vector                  possible                    possible                           
     * std::deque                   possible                    never
     * std::list                    never                       never
     * std::map                     never                       never
     * std::unordered_map           possible                    never
     * 
     * */

    std::vector<int> v;
    
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    const auto iterBefore = v.begin();
    std::cout << "Iterator invalidation example\nsize=" << v.size() 
              << "\ncapacity=" << v.capacity() 
              << "\nbefore [0]=" << *iterBefore << " << correct" << std::endl;
    
    v.push_back(3);
    v.push_back(4);
    const auto iterAfter = v.begin();
    std::cout << "pushing back...\nsize=" << v.size() 
              << "\ncapacity=" << v.capacity() 
              << "\nbefore [0]=" << *iterBefore << " << invalid iterator, UB" 
              << "\nafter  [0]=" << *iterAfter << " << correct" << std::endl;


    /* __5__ : insert_iterator
     * https://en.cppreference.com/w/cpp/iterator/insert_iterator
     * https://en.cppreference.com/w/cpp/iterator/inserter
     * 
     * 
     * ситуация = 
     *      <...>
     *          std::vector<int> v1 = { 0, 1, 2 };
     *          std::vector<int> v2 = { 0 };
     *          std::copy(v1.begin(), v1.end, v2.begin());      // error, v2.size() < v1.size() 
     *      <...>
     *
     *
     * проблема = 
     *      копирование элементов одного контейнера в другой
     *      std::copy() выполняет в целевом контейнере:
     *          сдвиг итератора
     *          разыменование итератора = получение доступа к элементу
     *          присвоение элементу нового значения   
     *      ошибка, так как размер исходного контейнера больше, чем целевого
     *
     *
     * решение = 
     *      вспомогательный класс, выполняющий операцию v2.insert() вместо копирования
     *      https://en.cppreference.com/w/cpp/iterator/insert_iterator
     *      примерная реализация
     *
     *          template<typename CONTAINER>
     *          struct insert_operator
     *          {
     *              // псевдонимы для удобства
     *              using ii = insert_iterator<CONTAINER>;
     *              using ct = typename CONTAINER::value_type;
     *              using ci = typename CONTAINER::iterator;
     *          
     *              // необходимые поля
     *              CONTAINER& container;
     *              ci iter;
     *
     *              // ничего никуда не смещается, ничего не разыменовывается
     *              ii& operator++ ( ) { return this*; }
     *              ii& operator*  ( ) { return this*; }
     *     
     *              // вся логика здесь
     *              ct& operator= (ct& value, ci& iter)
     *              {
     *                  container.insert(value, iter);      // вместо присвоения – вставка нового элемента
     *                  ++iter;                             // сдвиг итератора здесь вместо operator++
     *                  return *this;
     *              }
     *          };
     *
     *          std::copy(v_1.begin( ), v_1.end( ), std::insert_operator<vector<int>>(v_2, v_2.begin( )));      // ok
     *
     * */

    return 0;
}
