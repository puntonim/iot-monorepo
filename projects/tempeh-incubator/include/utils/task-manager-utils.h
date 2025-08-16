#ifndef _TASK_MANAGER_UTILS_H
#define _TASK_MANAGER_UTILS_H

namespace incub {
namespace task_manager_utils {

void printDebugTasks(bool isVerbose = true);
void _printExtraInfo();
void cancelTask(taskid_t &taskId);

}  // namespace task_manager_utils
}  // namespace incub
#endif
