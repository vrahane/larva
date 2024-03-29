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
#include <assert.h>

/**
 * os mutex create
 *  
 * Create a mutex and initialize it. 
 * 
 * @param mu Pointer to mutex
 * 
 * @return os_error_t 
 *      OS_INVALID_PARM     Mutex passed in was NULL.
 *      OS_OK               no error.
 */
os_error_t
os_mutex_init(struct os_mutex *mu)
{
    if (!mu) {
        return OS_INVALID_PARM;
    }

    /* Initialize to 0 */
    mu->mu_prio = 0;
    mu->mu_level = 0;
    mu->mu_owner = NULL;
    SLIST_FIRST(&mu->mu_head) = NULL;

    return OS_OK;
}

/**
 * os mutex release
 *  
 * Release a mutex. 
 * 
 * @param mu Pointer to the mutex to be released
 * 
 * @return os_error_t 
 *      OS_INVALID_PARM Mutex passed in was NULL.
 *      OS_BAD_MUTEX    Mutex was not granted to current task (not owner).
 *      OS_OK           No error
 */
os_error_t
os_mutex_release(struct os_mutex *mu)
{
    int resched;
    os_sr_t sr;
    struct os_task *current;
    struct os_task *rdy;

    /* Check if OS is started */
    if (!g_os_started) {
        return (OS_NOT_STARTED);
    }

    /* Check for valid mutex */
    if (!mu) {
        return OS_INVALID_PARM;
    }

    /* We better own this mutex! */
    current = os_sched_get_current_task();
    if ((mu->mu_level == 0) || (mu->mu_owner != current)) {
        return (OS_BAD_MUTEX);
    }

    /* Decrement nesting level by 1. If not zero, nested (so dont release!) */
    --mu->mu_level;
    if (mu->mu_level != 0) {
        return (OS_OK);
    }

    OS_ENTER_CRITICAL(sr);

    /* Restore owner task's priority; resort list if different  */
    if (current->t_prio != mu->mu_prio) {
        current->t_prio = mu->mu_prio;
        os_sched_resort(current);
    }

    /* Check if tasks are waiting for the mutex */
    rdy = SLIST_FIRST(&mu->mu_head);
    if (rdy) {
        /* There is one waiting. Wake it up */
        assert(rdy->t_mutex);
        rdy->t_mutex = NULL;

        SLIST_REMOVE_HEAD(&mu->mu_head, t_obj_list);
        SLIST_NEXT(rdy, t_obj_list) = NULL;
        os_sched_wakeup(rdy);

        /* Set mutex internals */
        mu->mu_level = 1;
        mu->mu_prio = rdy->t_prio;
    }

    /* Set new owner of mutex (or NULL if not owned) */
    mu->mu_owner = rdy;

    /* Do we need to re-schedule? */
    resched = 0;
    rdy = os_sched_next_task();
    if (rdy != current) {
        resched = 1;
    }
    OS_EXIT_CRITICAL(sr);

    /* Re-schedule if needed */
    if (resched) {
        os_sched(rdy, 0);
    }

    return OS_OK;
}

/**
 * os mutex pend 
 *  
 * Pend (wait) for a mutex. 
 * 
 * @param mu Pointer to mutex.
 * @param timeout Timeout, in os ticks. A timeout of 0 means do 
 *                not wait if not available. A timeout of
 *                0xFFFFFFFF means wait forever.
 *              
 * 
 * @return os_error_t 
 *      OS_INVALID_PARM     Mutex passed in was NULL.
 *      OS_TIMEOUT          Mutex was owned by another task and timeout=0
 *      OS_OK               no error.
 */ 
os_error_t
os_mutex_pend(struct os_mutex *mu, uint32_t timeout)
{
    os_sr_t sr;
    os_error_t rc;
    struct os_task *current;
    struct os_task *entry;
    struct os_task *last;

    /* OS must be started when calling this function */
    if (!g_os_started) {
        return (OS_NOT_STARTED);
    }

    /* Check for valid mutex */
    if (!mu) {
        return OS_INVALID_PARM;
    }

    OS_ENTER_CRITICAL(sr);

    /* Is this owned? */
    current = os_sched_get_current_task();
    if (mu->mu_level == 0) {
        mu->mu_owner = current;
        mu->mu_prio  = current->t_prio;
        mu->mu_level = 1;
        OS_EXIT_CRITICAL(sr);
        return OS_OK;
    }

    /* Are we owner? */
    if (mu->mu_owner == current) {
        ++mu->mu_level;
        OS_EXIT_CRITICAL(sr);
        return OS_OK;
    }

    /* Mutex is not owned by us. If timeout is 0, return immediately */
    if (timeout == 0) {
        OS_EXIT_CRITICAL(sr);
        return OS_TIMEOUT;
    }

    /* Change priority of owner if needed */
    if (mu->mu_owner->t_prio > current->t_prio) {
        mu->mu_owner->t_prio = current->t_prio;
        os_sched_resort(mu->mu_owner);
    }

    /* Link current task to tasks waiting for mutex */
    last = NULL;
    if (!SLIST_EMPTY(&mu->mu_head)) {
        /* Insert in priority order */
        SLIST_FOREACH(entry, &mu->mu_head, t_obj_list) {
            if (current->t_prio < entry->t_prio) { 
                break;
            }
            last = entry;
        }
    }

    if (last) {
        SLIST_INSERT_AFTER(last, current, t_obj_list);
    } else {
        SLIST_INSERT_HEAD(&mu->mu_head, current, t_obj_list);
    }

    /* Set mutex pointer in task */
    current->t_mutex = mu;
    os_sched_sleep(current, timeout);

    OS_EXIT_CRITICAL(sr);

    os_sched(NULL, 0);

    /* If we are owner we did not time out. */
    if (mu->mu_owner == current) {
        rc = OS_OK; 
    } else {
        rc = OS_TIMEOUT;
    }

    return rc;
}

/**
 * os mutex delete
 * 
 * Delete a mutex. 
 *  
 * @param mu Pointer to mutex to delete
 * 
 * @return os_error_t 
 *      OS_INVALID_PARM     Mutex passed in was NULL.
 *      OS_OK               no error.
 */
os_error_t
os_mutex_delete(struct os_mutex *mu)
{
    os_sr_t sr;
    struct os_task *current;
    struct os_task *rdy;

    /* OS must be started to call this function */
    if (!g_os_started) {
        return (OS_NOT_STARTED);
    }

    /* Check for valid mutex */
    if (!mu) {
        return OS_INVALID_PARM;
    }

    /* Get currently running task */
    current = os_sched_get_current_task();

    OS_ENTER_CRITICAL(sr);

    /* Restore owner task's priority. */
    if (mu->mu_level != 0) {
        if (mu->mu_owner->t_prio != mu->mu_prio) {
            /* We have to resort the ready list if this task is ready */
            mu->mu_owner->t_prio = mu->mu_prio;
            os_sched_resort(mu->mu_owner);
        }
    }

    /* Now, go through all the tasks waiting on the mutex */
    while (!SLIST_EMPTY(&mu->mu_head)) {
        rdy = SLIST_FIRST(&mu->mu_head);
        assert(rdy->t_mutex);
        rdy->t_mutex = NULL;
        SLIST_REMOVE_HEAD(&mu->mu_head, t_obj_list);
        SLIST_NEXT(rdy, t_obj_list) = NULL;
        os_sched_wakeup(rdy);
    }

    /* Is there a task that is ready that is higher priority than us? */
    rdy = os_sched_next_task();
    if (rdy != current) {
        /* Re-schedule */
        OS_EXIT_CRITICAL(sr);
        os_sched(rdy, 0);
    } else {
        OS_EXIT_CRITICAL(sr);
    }

    return OS_OK;
}

