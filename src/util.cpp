#include "util.h"

namespace Lua {
    nil_t const nil;
    namespace Caller {
        thread_local lua_State* running_state = nullptr;
    }
}
