#ifndef MYTR1_FUNCALL_HEADER
#define MYTR1_FUNCALL_HEADER

#include <tr1/functional_traits.h>
#include <tr1/utility>

namespace tr1
{

//implementation forwards
template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
class tuple;

/*
    Диспетчеризация вызовов функциональных объектов
*/

//перегрузка для функционального объекта без аргументов
template <class Ret, class Fn>
typename Ret funcall (Fn&);

//перегрузка для функционального объекта с 1-м аргументом
template <class Ret, class Fn, class T1>
typename Ret funcall (Fn&, T1&);

//перегрузка для функционального объекта с 2-мя аргументами
template <class Ret, class Fn, class T1, class T2>
typename Ret funcall (Fn&, T1&, T2&);

//перегрузка для функционального объекта с 3-мя аргументами
template <class Ret, class Fn, class T1, class T2, class T3>
typename Ret funcall (Fn&, T1&, T2&, T3&);

//перегрузка для функционального объекта с 4-мя аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&);

//перегрузка для функционального объекта с 5-ю аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&);

//перегрузка для функционального объекта с 6-ю аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&);

//перегрузка для функционального объекта с 7-ю аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&);

//перегрузка для функционального объекта с 8-ю аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&);

//перегрузка для функционального объекта с 9-ю аргументами
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
typename Ret funcall (Fn&, T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&);

/*
    Вызов функтора с аргументами заданными кортежем
*/

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
Ret apply (Fn&, const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>&);

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
Ret apply (Fn&, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>&);

#include <tr1/detail/unwrap.inl>
#include <tr1/detail/funcall.inl>
#include <tr1/detail/apply.inl>

}

#endif
