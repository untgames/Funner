#include "shared.h"

using namespace social;
using namespace social::game_kit;

/*
   Конструктор / Деструктор
*/

UtilityImpl::UtilityImpl ()
{
  NSString* system_version = [[UIDevice currentDevice] systemVersion];

  system_version_5_0_available = [system_version compare:@"5.0" options:NSNumericSearch] != NSOrderedAscending;

  date_formatter = [[NSDateFormatter alloc] init];

  [date_formatter setDateFormat:@"dd-MM-YYYY HH:mm:ss"];
}

UtilityImpl::~UtilityImpl ()
{
  [date_formatter release];
}

/*
   Утилиты для конвертирования типов
*/

void UtilityImpl::FillUser (GKPlayer* player, User& user)
{
  user.SetId       ([player.playerID UTF8String]);
  user.SetNickname ([player.alias UTF8String]);
  user.SetHandle   ([player retain], &release_ns_object);
  user.SetFriend   (player.isFriend);
}

void UtilityImpl::FillScore (GKScore* ns_score, Score& score)
{
  if (system_version_5_0_available)
  {
    char user_data_buffer [22];

    memset (user_data_buffer, 0, sizeof (user_data_buffer));

    if (snprintf (user_data_buffer, sizeof (user_data_buffer), "%llu", ns_score.context) == sizeof (user_data_buffer))
      throw xtl::format_operation_exception ("social::game_kit::UtilityImpl::FillScore", "Can't format user data from score context %llu", ns_score.context);

    score.SetUserData (user_data_buffer);
  }

  score.SetUserId         ([ns_score.playerID UTF8String]);
  score.SetLeaderboardId  ([ns_score.category UTF8String]);
  score.SetValue          (ns_score.value);
  score.SetFormattedValue ([ns_score.formattedValue UTF8String]);
  score.SetRank           (ns_score.rank);
  score.SetHandle         ([ns_score retain], &release_ns_object);

  score.Properties ().SetProperty ("Date", [[date_formatter stringFromDate:ns_score.date] UTF8String]);
}

void UtilityImpl::FillLeaderboard (GKLeaderboard* ns_leaderboard, Leaderboard& leaderboard)
{
  static const char* METHOD_NAME = "social::game_kit::UtilityImpl::FillLeaderboard";

  const char* player_scope = "";

  switch (ns_leaderboard.playerScope)
  {
    case GKLeaderboardPlayerScopeGlobal:
      player_scope = "Global";
      break;
    case GKLeaderboardPlayerScopeFriendsOnly:
      player_scope = "FriendsOnly";
      break;
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Unknown player scope %d", ns_leaderboard.playerScope);
  }

  const char* time_scope = "";

  switch (ns_leaderboard.timeScope)
  {
    case GKLeaderboardTimeScopeToday:
      time_scope = "Today";
      break;
    case GKLeaderboardTimeScopeWeek:
      time_scope = "Week";
      break;
    case GKLeaderboardTimeScopeAllTime:
      time_scope = "AllTime";
      break;
    default:
      throw xtl::format_operation_exception (METHOD_NAME, "Unknown time scope %d", ns_leaderboard.timeScope);
  }

  leaderboard.SetId     ([ns_leaderboard.category UTF8String]);
  leaderboard.SetTitle  ([ns_leaderboard.title UTF8String]);

  if (ns_leaderboard.localPlayerScore)
  {
    Score user_score;

    FillScore (ns_leaderboard.localPlayerScore, user_score);

    leaderboard.SetUserScore (user_score);
  }

  NSArray*   ns_scores = ns_leaderboard.scores;
  ScoreList& scores    = leaderboard.Scores ();

  scores.Reserve ([ns_scores count]);

  for (GKScore* ns_score in ns_scores)
  {
    Score score;

    FillScore (ns_score, score);

    scores.Add (score);
  }

  leaderboard.SetHandle ([ns_leaderboard retain], &release_ns_object);

  common::PropertyMap& properties = leaderboard.Properties ();

  properties.SetProperty ("RangeBegin", (int)ns_leaderboard.range.location);
  properties.SetProperty ("RangeLength", (int)ns_leaderboard.range.length);

  if (xtl::xstrlen (player_scope))
    properties.SetProperty ("PlayerScope", player_scope);

  if (xtl::xstrlen (time_scope))
    properties.SetProperty ("TimeScope", time_scope);
}

media::Image UtilityImpl::ConvertImage (UIImage* image)
{
  static const char* METHOD_NAME = "social::game_kit::UtilityImpl::ConvertImage";

  CGImageRef   cg_image   = image.CGImage;
  CGBitmapInfo image_info = CGImageGetBitmapInfo (cg_image);

  size_t bpp                = CGImageGetBitsPerPixel (cg_image),
         bits_per_component = CGImageGetBitsPerComponent (cg_image);

  if (bits_per_component != 8)
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported bits per component %u", bits_per_component);

  if (bpp != 24 && bpp != 32)
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported bits per pixel %u", bpp);

  if ((CGColorSpaceGetModel (CGImageGetColorSpace (cg_image)) != kCGColorSpaceModelRGB) ||
      (bpp == 24 && ((image_info & kCGBitmapAlphaInfoMask) != kCGImageAlphaNone)) ||
      (bpp == 32 && ((image_info & kCGBitmapAlphaInfoMask) != kCGImageAlphaLast)))
    throw xtl::format_operation_exception (METHOD_NAME, "Unsupported color space");

  media::PixelFormat pixel_format = bpp == 24 ? media::PixelFormat_RGB8 : media::PixelFormat_RGBA8;

  size_t row_bytes    = CGImageGetBytesPerRow (cg_image),
         image_width  = CGImageGetWidth (cg_image),
         image_height = CGImageGetHeight (cg_image);

  if (row_bytes != image_width * bpp / 8)
    throw xtl::format_operation_exception (METHOD_NAME, "Returned image has padding");

  CGDataProviderRef image_data_provider = CGImageGetDataProvider (cg_image);
  CFDataRef         image_data          = CGDataProviderCopyData (image_data_provider);

  try
  {
    media::Image image (image_width, image_height, 1, pixel_format, CFDataGetBytePtr (image_data));

    CFRelease (image_data);

    return image;
  }
  catch (...)
  {
    if (image_data)
      CFRelease (image_data);

    throw;
  }
}
