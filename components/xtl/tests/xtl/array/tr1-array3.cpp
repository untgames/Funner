#include "shared.h"

template <class T>
void print_elements (const T& x);

int main()
{
    printf ("Results of tr1_array3_test:\n");

    // create array of four seasons
    array<stl::string,4> seasons = {
        { "spring", "summer", "autumn", "winter" }
    };

    // copy and change order
    array<stl::string,4> seasons_orig = seasons;
    for (unsigned i=seasons.size()-1; i>0; --i) {
        stl::swap (seasons.at (i), seasons.at ((i+1) % seasons.size ()));
    }

    printf ("one way:   ");
    print_elements(seasons);

    // try swap()
    printf ("other way: ");
    stl::swap (seasons,seasons_orig);
    print_elements(seasons);

    // try reverse iterators
    printf ("reverse:   ");
    for (array<stl::string,4>::reverse_iterator pos = seasons.rbegin(); pos < seasons.rend(); ++pos)
      printf (" %s", pos->c_str ());
    printf ("\n");

    return 0;
}

template <class T>
void print_elements (const T& x)
{
   for (unsigned i=0; i<x.size(); ++i)
      printf (" %s", x[i].c_str ());

   printf ("\n");
}

