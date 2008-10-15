/*
    Утилиты
*/

namespace detail
{

struct NextParseNodeFn
{
  void operator () (ParseNode& node) const { node = node.Next (); }
};

struct NextNamesakeParseNodeFn
{
  void operator () (ParseNode& node) const { node = node.NextNamesake (); }
};

}

/*
    BasicParseIterator
*/

template <class NextFn>
inline BasicParseIterator<NextFn>::BasicParseIterator ()
  {}

template <class NextFn>
inline BasicParseIterator<NextFn>::BasicParseIterator (const ParseNode& in_node)
  : node (in_node)
  {}

template <class NextFn> template <class NextFn1>
inline BasicParseIterator<NextFn>::BasicParseIterator (const BasicParseIterator<NextFn1>& i)
  : node (*i)
  { }

template <class NextFn>
inline BasicParseIterator<NextFn>& BasicParseIterator<NextFn>::operator = (const ParseNode& in_node)
{
  node = in_node;

  return *this;
}

template <class NextFn> template <class NextFn1>
inline BasicParseIterator<NextFn>& BasicParseIterator<NextFn>::operator = (const BasicParseIterator<NextFn1>& i)
{
  node = *i;

  return *this;
}

template <class NextFn>
inline BasicParseIterator<NextFn>& BasicParseIterator<NextFn>::operator ++ ()
{
   NextFn ()(node);

  return *this;
}

template <class NextFn>
inline BasicParseIterator<NextFn> BasicParseIterator<NextFn>::operator ++ (int)
{
  BasicParseIterator tmp = *this;

  NextFn ()(node);

  return tmp;
}

template <class NextFn>
inline ParseNode* BasicParseIterator<NextFn>::operator -> ()
{
  return &node;
}

template <class NextFn>
inline const ParseNode* BasicParseIterator<NextFn>::operator -> () const
{
  return &node;
}

template <class NextFn>
inline ParseNode& BasicParseIterator<NextFn>::operator * ()
{
  return node;
}

template <class NextFn>
inline const ParseNode& BasicParseIterator<NextFn>::operator * () const
{
  return node;
}

template <class NextFn>
inline bool BasicParseIterator<NextFn>::operator ! () const
{
  return node.IsEmpty ();
}
