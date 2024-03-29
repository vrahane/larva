/**
 * Copyright (c) 2015 Runtime Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "os/os.h"
#include "os/queue.h"

#include <assert.h>

TAILQ_HEAD(, os_task) g_os_run_list = TAILQ_HEAD_INITIALIZER(g_os_run_list); 

TAILQ_HEAD(, os_task) g_os_sleep_list = TAILQ_HEAD_INITIALIZER(g_os_sleep_list); 

struct os_task *g_current_task; 

/**
 * os sched insert
 *  
 * Insert a task into the scheduler list. This causes the task to be evaluated
 * for running when os_sched is called. 
 * 
 * @param t     Pointer to task to insert in run list
 * 
 * @return int  OS_OK: task was inserted into run list 
 *              OS_EINVAL: Task was not in ready state. 
 */
os_error_t
os_sched_insert(struct os_task *t) 
{
    struct os_task *entry; 
    os_sr_t sr; 
    os_error_t rc;

    if (t->t_state != OS_TASK_READY) {
        rc = OS_EINVAL;
        goto err;
    }

    entry = NULL;
    OS_ENTER_CRITICAL(sr); 
    TAILQ_FOREACH(entry, &g_os_run_list, t_os_list) {
        if (t->t_prio < entry->t_prio) { 
            break;
        }
    }
    if (entry) {
        TAILQ_INSERT_BEFORE(entry, (struct os_task *) t, t_os_list);
    } else {
        TAILQ_INSERT_TAIL(&g_os_run_list, (struct os_task *) t, t_os_list);
    }
    OS_EXIT_CRITICAL(sr);

    return (0);
err:
    return (rc);
}

/**
 * os sched get current task 
 *  
 * Returns the currently running task. Note that this task may or may not be 
 * the highest priority task ready to run. 
 * 
 * 
 * @return struct os_task* 
 */
struct os_task * 
os_sched_get_current_task(void)
{
    return (g_current_task);
}

/**
 * os sched set current task 
 *  
 * Sets the currently running task to 't'. Note that this function simply sets 
 * the global variable holding the currently running task. It does not perform 
 * a context switch or change the os run or sleep list. 
 * 
 * @param t Pointer to currently running task.
 */
void 
os_sched_set_current_task(struct os_task *t) 
{
    g_current_task = t;
}

/**
 * os sched 
 *  
 * Performs a context switch. When called, it will either find the highest 
 * priority task ready to run if next_t is NULL (i.e. the head of the os run 
 * list) or will schedule next_t as the task to run.
 * 
 * @param next_t Task to run
 * @param isr    Flag denoting whether we are inside an isr (0:no, 1:yes).
 */
void
os_sched(struct os_task *next_t, int isr) 
{
    os_sr_t sr;

    OS_ENTER_CRITICAL(sr);

    if (!next_t) {
        next_t = os_sched_next_task();
    }

    if (next_t != os_sched_get_current_task()) {
        OS_EXIT_CRITICAL(sr);
        if (isr) {
            os_arch_ctx_sw_isr(next_t);
        } else {
            os_arch_ctx_sw(next_t);
        }

    } else {
        OS_EXIT_CRITICAL(sr);
    }
}

/**
 * os sched sleep 
 *  
 * Removes the task from the run list and puts it on the sleep list. 
 * 
 * @param t Task to put to sleep
 * @param nticks Number of ticks to put task to sleep
 * 
 * @return int
 *  
 * NOTE: must be called with interrupts disabled! This function does not call 
 * the scheduler 
 */
int 
os_sched_sleep(struct os_task *t, os_time_t nticks) 
{
    struct os_task *entry;

    entry = NULL; 

    TAILQ_REMOVE(&g_os_run_list, t, t_os_list);
    t->t_state = OS_TASK_SLEEP;
    t->t_next_wakeup = os_time_get() + nticks;
    if (nticks == OS_TIMEOUT_NEVER) {
        t->t_flags |= OS_TASK_FLAG_NO_TIMEOUT;
        TAILQ_INSERT_TAIL(&g_os_sleep_list, t, t_os_list); 
    } else {
        TAILQ_FOREACH(entry, &g_os_sleep_list, t_os_list) {
            if ((entry->t_flags & OS_TASK_FLAG_NO_TIMEOUT) ||
                    OS_TIME_TICK_GT(entry->t_next_wakeup, t->t_next_wakeup)) {
                break;
            }
        }
        if (entry) {
            TAILQ_INSERT_BEFORE(entry, t, t_os_list); 
        } else {
            TAILQ_INSERT_TAIL(&g_os_sleep_list, t, t_os_list); 
        }
    }

    return (0);
}

/**
 * os sched wakeup 
 *  
 * Called to wake up a task. Waking up a task consists of setting the task state
 * to READY and moving it from the sleep list to the run list. 
 * 
 * @param t     Pointer to task to wake up. 
 * 
 * @return int 
 *  
 * NOTE: This function must be called with interrupts disabled. 
 */
int 
os_sched_wakeup(struct os_task *t) 
{
    /* Remove self from mutex list if waiting on one */
    if (t->t_mutex) {
        assert(!SLIST_EMPTY(&t->t_mutex->mu_head));
        SLIST_REMOVE(&t->t_mutex->mu_head, t, os_task, t_obj_list);
        SLIST_NEXT(t, t_obj_list) = NULL;
        t->t_mutex = NULL; 
    }

    /* Remove task from sleep list */
    t->t_state = OS_TASK_READY;
    t->t_next_wakeup = 0;
    t->t_flags &= ~OS_TASK_FLAG_NO_TIMEOUT;
    TAILQ_REMOVE(&g_os_sleep_list, t, t_os_list);
    os_sched_insert(t);

    return (0);
}

/**
 * os sched os timer exp 
 *  
 * Called when the OS tick timer expires. Search the sleep list for any tasks 
 * that need waking up. This occurs when the current OS time exceeds the next 
 * wakeup time stored in the task. Any tasks that need waking up will be 
 * removed from the sleep list and added to the run list. 
 * 
 */
void
os_sched_os_timer_exp(void)
{
    struct os_task *t;
    struct os_task *next;
    os_time_t now; 
    os_sr_t sr;

    now = os_time_get();

    OS_ENTER_CRITICAL(sr);

    /*
     * Wakeup any tasks that have their sleep timer expired
     */
    t = TAILQ_FIRST(&g_os_sleep_list);
    while (t) {
        /* If task waiting forever, do not check next wakeup time */
        if (t->t_flags & OS_TASK_FLAG_NO_TIMEOUT) {
            break;
        }
        next = TAILQ_NEXT(t, t_os_list);
        if (OS_TIME_TICK_GEQ(now, t->t_next_wakeup)) {
            os_sched_wakeup(t);
        } else {
            break;
        }
        t = next;
    }

    OS_EXIT_CRITICAL(sr); 
}

/**
 * os sched next task 
 *  
 * Returns the task that we should be running. This is the task at the head 
 * of the run list. 
 *  
 * NOTE: if you want to guarantee that the os run list does not change after 
 * calling this function you have to call it with interrupts disabled. 
 * 
 * @return struct os_task* 
 */
struct os_task *  
os_sched_next_task(void) 
{
    return (TAILQ_FIRST(&g_os_run_list));
}

/**
 * os sched resort 
 *  
 * Resort a task that is in the ready list as its priority has 
 * changed. If the task is not in the ready state, there is 
 * nothing to do. 
 * 
 * @param t Pointer to task to insert back into ready to run 
 *          list.
 *  
 * NOTE: this function expects interrupts to be disabled so they 
 * are not disabled here. 
 */
void 
os_sched_resort(struct os_task *t) 
{
    if (t->t_state == OS_TASK_READY) {
        TAILQ_REMOVE(&g_os_run_list, t, t_os_list);
        os_sched_insert(t);
    }
}

