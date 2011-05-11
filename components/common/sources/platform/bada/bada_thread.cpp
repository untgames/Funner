#include <FBaseRtThreadThread.h>
#include <platform/platform.h>
using namespace common;


BadaPlatform::threadid_t BadaPlatform::GetCurrentThreadId() {
    return reinterpret_cast<BadaPlatform::threadid_t>(Osp::Base::Runtime::Thread::GetCurrentThread());
}
