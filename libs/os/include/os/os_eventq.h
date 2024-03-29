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

#ifndef _OS_EVENTQ_H
#define _OS_EVENTQ_H

struct os_event {
    uint8_t ev_queued;
    uint8_t ev_type;
    void *ev_arg;
    STAILQ_ENTRY(os_event) ev_next;
};

#define OS_EVENT_QUEUED(__ev) ((__ev)->ev_queued)

#define OS_EVENT_T_TIMER (1)
#define OS_EVENT_T_PERUSER (16)

struct os_eventq {
    struct os_task *evq_task;
    STAILQ_HEAD(, os_event) evq_list;
};

void os_eventq_init(struct os_eventq *);
void os_eventq_put2(struct os_eventq *, struct os_event *, int);
void os_eventq_put(struct os_eventq *, struct os_event *);
struct os_event *os_eventq_get(struct os_eventq *);
void os_eventq_remove(struct os_eventq *, struct os_event *);

#endif /* _OS_EVENTQ_H */

