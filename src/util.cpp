#include "util.h"

namespace Lua {
    namespace Caller {
        thread_local lua_State* running_state = nullptr;
    }
}
