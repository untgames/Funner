#ifndef SOCIAL_CORE_TESTS_SHARED_HEADER
#define SOCIAL_CORE_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/string>

#include <xtl/function.h>

#include <common/property_map.h>

#include <social/session.h>

using namespace common;
using namespace social;

void dump (const PropertyMap& properties)
{
  printf ("    map has %u properties (hash=%08x, layout_hash=%08x):\n",
    properties.Size (), properties.Hash (), properties.LayoutHash ());

  for (size_t i=0, count=properties.Size (); i<count; i++)
  {
    stl::string value;

    properties.GetProperty (i, value);

    printf ("     #%u: name='%s', type=%s, value='%s'\n", i, properties.PropertyName (i), get_name (properties.PropertyType (i)), value.c_str ());
  }
}

void dump (const Achievement& achievement)
{
  printf ("Achievement '%s':\n", achievement.Id ());
  printf ("  title    '%s'\n",      achievement.Title ());
  printf ("  hidden   %c\n",       achievement.IsHidden () ? 'y' : 'n');
  printf ("  progress %.2g\n",     achievement.Progress ());
  printf ("  handle   %p\n",       achievement.Handle ());
  printf ("  properties:\n");
  dump (achievement.Properties ());
}

void dump (const Score& score)
{
  printf ("Score:\n");
  printf ("  user id         '%s'\n", score.UserId ());
  printf ("  leaderboard id  '%s'\n", score.LeaderboardId ());
  printf ("  user data       '%s'\n", score.UserData ());
  printf ("  value           %.2g\n", score.Value ());
  printf ("  formatted value '%s'\n", score.FormattedValue ());
  printf ("  rank            %u\n",   score.Rank ());
  printf ("  handle          %p\n",   score.Handle ());
  printf ("  properties:\n");
  dump (score.Properties ());
}

void dump (const Leaderboard& leaderboard)
{
  printf ("Leaderboard '%s':\n", leaderboard.Id ());
  printf ("  title '%s'\n",      leaderboard.Title ());
  printf ("  handle %p\n",       leaderboard.Handle ());
  printf ("  properties:\n");
  dump (leaderboard.Properties ());
  printf ("  user score [\n");
  dump (leaderboard.UserScore ());
  printf ("]\n");

  const ScoreList& scores = leaderboard.Scores ();

  printf ("Scores count %d: [\n", scores.Size ());

  for (size_t i = 0, count = scores.Size (); i < count; i++)
    dump (scores [i]);

  printf ("]\n");
}

void dump (const User& user)
{
  printf ("User:\n");
  printf ("  id        '%s'\n", user.Id ());
  printf ("  nickname  '%s'\n", user.Nickname ());
  printf ("  is friend %c\n", user.IsFriend () ? 'y' : 'n');
  printf ("  handle    %p\n",   user.Handle ());
  printf ("  properties:\n");
  dump (user.Properties ());
}


#endif
