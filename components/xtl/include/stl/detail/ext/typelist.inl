/*
    Определение длины списка
*/

template <class Head, class Tail> struct length<type_node<Head, Tail> >
{
  enum { value = 1 + length<Tail>::value };
};

template <> struct length<null_type>
{
  enum { value = 0 };
};

template <> struct length<type_node<null_type, null_type> >
{
  enum { value = 0 };
};

/*
    Получение index-го элемента списка
*/

template <class Head, class Tail> struct at<type_node<Head, Tail>, 0>
{
  typedef Head type;
};

template <class Head, class Tail, size_t index> struct at<type_node<Head, Tail>, index>
{
  typedef typename at<Tail, index-1>::type type;
};

/*
    Получение индекса элемента
*/

template <class Head, class Tail, class T> struct indexof<type_node<Head, Tail>, T>
{
  private: enum { temp  = indexof<Tail, T>::value };
  public:  enum { value = temp == -1 ? -1 : temp + 1 };
};

template <class T> struct indexof<null_type, T>
{
  enum { value = -1 };
};

template <class Tail, class T> struct indexof<type_node<T, Tail>, T>
{
  enum { value = 0 };
};

/*
    Добавление элемента или списка типов
*/

template <> struct append<null_type, null_type>
{
  typedef null_type type;
};

template <class T> struct append<null_type, T>
{
  typedef type_node<T, null_type> type;
};

template <class Head, class Tail> struct append<null_type, type_node<Head, Tail> >
{
  typedef type_node<Head, Tail> type;
};

template <class Head, class Tail, class T> struct append<type_node<Head, Tail>, T>
{
  typedef type_node<Head, typename append<Tail, T>::type> type;
};

/*
    Удаление первого вхождения элемента в список
*/

template <class T> struct erase_first<null_type, T>
{
  typedef null_type type;
};

template <class T, class Tail> struct erase_first<type_node<T, Tail>, T>
{
  typedef Tail type;
};

template <class Head, class Tail, class T> struct erase_first<type_node<Head, Tail>, T>
{
  typedef type_node<Head, typename erase_first<Tail, T>::type> type;
};

/*
    Удаление всех вхождений элементов в список
*/

template <class T> struct erase<null_type, T>
{
  typedef null_type type;
};

template <class T, class Tail> struct erase<type_node<T, Tail>, T>
{
  typedef typename erase<Tail, T>::type type;
};

template <class Head, class Tail, class T> struct erase<type_node<Head, Tail>, T>
{
  typedef type_node<Head, typename erase<Tail, T>::type> type;
};

/*
    Удаление дубликатов
*/

template <> struct unique<null_type>
{
  typedef null_type type;
};

template <class Head, class Tail> struct unique<type_node<Head, Tail> >
{
  typedef type_node<Head, typename erase_first<typename unique<Tail>::type, Head>::type> type;
};

/*
    Замена первого вхождения элемента T в списке на R
*/

template <class T, class R> struct replace_first<null_type, T, R>
{
  typedef null_type type;
};

template <class T, class Tail, class R> struct replace_first<type_node<T, Tail>, T, R>
{
  typedef type_node<R,Tail> type;
};

template <class Head, class Tail, class T, class R> struct replace_first<type_node<Head, Tail>, T, R>
{
  typedef type_node<Head, typename replace_first<Tail, T, R>::type> type;
};

/*
    Замена всех вхождений элемента T в списке на R
*/

template <class T, class R> struct replace<null_type, T, R>
{
  typedef null_type type;
};

template <class T, class Tail, class R> struct replace<type_node<T, Tail>, T, R>
{
  typedef type_node<R, typename replace<Tail, T, R>::type> type;
};

template <class Head, class Tail, class T, class R>
struct replace<type_node<Head, Tail>, T, R>
{
  typedef type_node<Head, typename replace<Tail, T, R>::type> type;
};

/*
    Обращение порядка следования элементов в списке
*/

template <class T> struct reverse<type_node<T, null_type> >
{
  typedef type_node<T, null_type> type;
};

template <class Head, class Tail> struct reverse<type_node<Head, Tail> >
{
  typedef typename append<typename reverse<Tail>::type, Head>::type type;
};

/*
    Отображение списка с применением к каждому из его элементов операции Transform
*/

template <template <class T> class Transform> struct transform<null_type, Transform>
{
  typedef null_type type;
};

template <class Head, class Tail, template <class T> class Transform>
struct transform<type_node<Head, Tail>, Transform>
{
  typedef type_node<typename Transform<Head>::type, typename transform<Tail, Transform>::type> type;
};

/*
    Аккумулирование преобразований над списком
*/

template <class TypeNode, template <class CurrentResult, class T> class Transform, class StartResult>
struct reverse_accumulate;

template <template <class CurrentResult, class T> class Transform, class StartResult>
struct reverse_accumulate<null_type, Transform, StartResult>
{
  typedef StartResult type;
};

template <class Head, class Tail, template <class CurrentResult, class T> class Transform, class StartResult>
struct reverse_accumulate<type_node<Head, Tail>, Transform, StartResult>
{
  typedef typename Transform<typename reverse_accumulate<Tail, Transform, StartResult>::type, Head>::type type;
};

template <class TypeNode, template <class CurrentResult, class T> class Transform, class StartResult>
struct accumulate: public reverse_accumulate<typename reverse<TypeNode>::type, Transform, StartResult> {};

/*
    Генерация иерархии с множественным наследованием
*/

template <class TypeNode, template <class T> class Unit, class Root>
struct generate_hierarchy_helper
{ 
  template <class Base, class T>
  struct generic_class: public Base,  public Unit<T> {};
  
  template <class Base>
  struct generic_class<Base, null_type>: public Base {};  

  template <class Base, class T>
  struct generator
  {
    typedef generic_class<Base, T> type;
  };
  
  struct generate_hierarchy: public accumulate<TypeNode, generator, Root> {};
};

template <class TypeNode, template <class T> class Unit, class Root>
struct generate_hierarchy: public generate_hierarchy_helper<TypeNode, Unit, Root>::generate_hierarchy {};

/*
    Генерация иерархии с одиночным наследованием
*/

template <class TypeNode, template <class Base, class T> class Unit, class Root>
struct generate_linear_hierarchy_helper
{ 
  template <class Base, class T> struct generator
  {
    typedef Unit<Base, T> type;
  };

  template <class Base> struct generator<Base, null_type>
  {
    typedef Base type;
  };
  
  struct generate_linear_hierarchy: public accumulate<TypeNode, generator, Root> {};
};

template <class TypeNode, template <class Base, class T> class Unit, class Root>
struct generate_linear_hierarchy: 
  public generate_linear_hierarchy_helper<TypeNode, Unit, Root>::generate_linear_hierarchy {};

/*
    Управляющие шаблоны
*/

//выбор из двух типов condition == true -> TrueType, false -> FalseType
template <bool condition, class TrueType, class FalseType> struct select
{
  typedef TrueType type;
};

template <class TrueType, class FalseType> struct select<false, TrueType, FalseType>
{
  typedef FalseType type;
};
