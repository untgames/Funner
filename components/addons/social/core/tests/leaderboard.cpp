#include "shared.h"

int main ()
{
  printf ("Results of leaderboard_test:\n");
  
  try
  {
    Leaderboard leaderboard;

    printf ("default leaderboard\n");

    dump (leaderboard);

    leaderboard.SetId ("Id1");
    leaderboard.SetTitle ("Title");
    leaderboard.SetHandle ((const void*)1);

    PropertyMap properties;

    properties.SetProperty ("String", "StringValue");
    properties.SetProperty ("IntValue", 10);

    leaderboard.SetProperties (properties);

    Score user_score, second_score;

    user_score.SetUserId   ("Id1");
    second_score.SetUserId ("Id2");

    leaderboard.SetUserScore (user_score);
    leaderboard.Scores ().Add (user_score);
    leaderboard.Scores ().Add (second_score);

    printf ("filled leaderboard\n");

    dump (leaderboard);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
