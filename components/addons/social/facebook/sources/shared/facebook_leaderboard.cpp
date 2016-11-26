#include "shared.h"

using namespace social;
using namespace social::facebook;

/*
   Таблицы рекордов
*/

void FacebookSessionImpl::LoadLeaderboards (const LoadLeaderboardsCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadLeaderboardsIds", "Leaderboards not supported for facebook session");
}

void FacebookSessionImpl::LoadLeaderboard (const char* leaderboard_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadLeaderboard (const char*, const LoadLeaderboardCallback&, const common::PropertyMap&)", "Leaderboards not supported for facebook session");
}

void FacebookSessionImpl::LoadLeaderboard (const char* leaderboard_id, const char* user_id, const LoadLeaderboardCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::LoadLeaderboard (const char*, const char*, const LoadLeaderboardCallback&, const common::PropertyMap&)", "Leaderboards not supported for facebook session");
}

/*
   Публикация
*/

void FacebookSessionImpl::SendScore (const Score& score, const SendScoreCallback& callback, const common::PropertyMap& properties)
{
  throw xtl::format_not_supported_exception ("social::facebook::FacebookSessionImpl::SendScore", "Leaderboards not supported for facebook session");
}
