/*
    Invoker
*/

//конструктор
inline Invoker::Invoker (const Function& in_handler, size_t in_arguments_count, size_t in_results_count)
  : handler (in_handler), arguments_count (in_arguments_count), results_count (in_results_count)
  {}

//получение количества аргументов
inline size_t Invoker::ArgumentsCount () const
{
  return arguments_count;
}

//получение количества результатов вызова шлюза
inline size_t Invoker::ResultsCount () const
{
  return results_count;
}

//вызов шлюза
inline void Invoker::operator () (IStack& stack) const
{
  handler (stack);
}
