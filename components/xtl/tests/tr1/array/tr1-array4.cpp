#include "shared.h"

int main ()
{
    printf ("Results of tr1_array4_test:\n");

    // array of arrays of seasons
    array<array<stl::string,4>,2> seasons_i18n = {
        { { { "spring", "summer", "autumn", "winter", } },
          { { "Fruehling", "Sommer", "Herbst", "Winter" } }
        }
    };

    // for any array of seasons print seasons
    for (unsigned i=0; i<seasons_i18n.size(); ++i) {
        array<stl::string,4> seasons = seasons_i18n[i];
        for (unsigned j=0; j<seasons.size(); ++j) {
            printf ("%s ", seasons[j].c_str ());
        }
        printf ("\n");
    }

    // print first element of first array
    printf ("first element of first array: %s\n", seasons_i18n[0][0].c_str ());

    // print last element of last array
    printf ("last element of last array: %s\n", seasons_i18n[seasons_i18n.size()-1][seasons_i18n[0].size()-1].c_str ());

    return 0;
}

