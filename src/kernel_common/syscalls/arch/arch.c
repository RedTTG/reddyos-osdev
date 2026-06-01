#include "common.h"
#include "abi-bits/errno.h"
#include "abi-bits/prctl.h"

u64 do_sys_arch_prctl(u64 cmd, const void* pointer) {
    switch (cmd) {
        case ARCH_SET_FS:
            if (!pointer)
                return -EINVAL;
            current_thread->fs_base = (u64)pointer;
            switch_thread_fs(current_thread);
            return 0;
        default:
            return -EINVAL;
    }
}