#include "shared.h"

int main ()
{
  printf ("Results of score_test:\n");
  
  try
  {
    Score score;

    printf ("default score\n");

    dump (score);

    score.SetUserId ("Id1");
    score.SetLeaderboardId ("Id2");
    score.SetUserData ("UserData");
    score.SetValue (1.5);
    score.SetFormattedValue ("100 points");
    score.SetRank (10);
    score.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    score.SetProperties (properties);

    printf ("filled score\n");

    dump (score);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
