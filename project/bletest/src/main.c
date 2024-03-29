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
#include <assert.h>
#include <string.h>
#include "os/os.h"
#include "bsp/bsp.h"
#include "hal/hal_gpio.h"
#include "hal/hal_cputime.h"
#include "console/console.h"

/* BLE */
#include "nimble/ble.h"
#include "host/host_hci.h"
#include "controller/ll.h"

/* Init all tasks */
volatile int tasks_initialized;
int init_tasks(void);

/* Task 1 */
#define HOST_TASK_PRIO      (1)
#define HOST_STACK_SIZE     OS_STACK_ALIGN(256)
struct os_task host_task;
os_stack_t host_stack[HOST_STACK_SIZE];

/* For LED toggling */
int g_led_pin;

/* Our global device address (public) */
uint8_t g_dev_addr[BLE_DEV_ADDR_LEN];

/* Our random address (in case we need it) */
uint8_t g_random_addr[BLE_DEV_ADDR_LEN];

/* A buffer for host advertising data */
uint8_t g_host_adv_data[BLE_HCI_MAX_ADV_DATA_LEN];

/* Create a mbuf pool of BLE mbufs */
#define MBUF_NUM_MBUFS      (16)
#define MBUF_BUF_SIZE       (256)
#define MBUF_MEMBLOCK_SIZE  \
    (MBUF_BUF_SIZE + sizeof(struct os_mbuf) + sizeof(struct os_mbuf_pkthdr) + \
     sizeof(struct ble_mbuf_hdr))

#define MBUF_MEMPOOL_SIZE   OS_MEMPOOL_SIZE(MBUF_NUM_MBUFS, MBUF_MEMBLOCK_SIZE)

struct os_mbuf_pool g_mbuf_pool; 
struct os_mempool g_mbuf_mempool;
os_membuf_t g_mbuf_buffer[MBUF_MEMPOOL_SIZE];

/* Some application configurations */
#define BLETEST_ROLE_ADVERTISER     (0)
#define BLETEST_ROLE_SCANNER        (1)
#define BLETEST_CFG_ADV_ITVL        (500000 / BLE_HCI_ADV_ITVL)
#define BLETEST_CFG_ADV_TYPE        BLE_HCI_ADV_TYPE_ADV_SCAN_IND
#define BLETEST_CFG_SCAN_ITVL       (500000 / BLE_HCI_SCAN_ITVL)
#define BLETEST_CFG_SCAN_WINDOW     (400000 / BLE_HCI_SCAN_ITVL)
#define BLETEST_CFG_ROLE            (BLETEST_ROLE_SCANNER)
uint32_t g_next_os_time;
int bletest_state;

uint8_t
bletest_create_adv_pdu(uint8_t *dptr)
{
    uint8_t len;

    /* Place flags in first */
    dptr[0] = 2;
    dptr[1] = 0x01;     /* Flags identifier */
    dptr[2] = 0x06;
    dptr += 3;
    len = 3;

    /* Add local name */
    dptr[0] = 15;   /* Length of this data, not including the length */
    dptr[1] = 0x09;
    dptr[2] = 'r';
    dptr[3] = 'u';
    dptr[4] = 'n';
    dptr[5] = 't';
    dptr[6] = 'i';
    dptr[7] = 'm';
    dptr[8] = 'e';
    dptr[9] = '-';
    dptr[10] = 'm';
    dptr[11] = 'y';
    dptr[12] = 'n';
    dptr[13] = 'e';
    dptr[14] = 'w';
    dptr[15] = 't';
    dptr += 16;
    len += 16;

    /* Add local device address */
    dptr[0] = 0x08;
    dptr[1] = 0x1B;
    dptr[2] = 0x00;
    memcpy(dptr + 3, g_dev_addr, BLE_DEV_ADDR_LEN);
    len += 9;

    return len;
}

void
bletest_init_advertising(void)
{
    int rc;
    uint8_t adv_len;
    uint16_t adv_itvl;
    struct hci_adv_params adv;

    /* Create the advertising PDU */
    adv_len = bletest_create_adv_pdu(&g_host_adv_data[0]);

    /* Set advertising parameters */
    adv_itvl = BLETEST_CFG_ADV_ITVL; /* Advertising interval */
    adv.adv_type = BLETEST_CFG_ADV_TYPE;
    adv.adv_channel_map = 0x07;
    adv.adv_filter_policy = BLE_HCI_ADV_FILT_NONE;
    adv.own_addr_type = BLE_HCI_ADV_OWN_ADDR_PUBLIC;
    adv.peer_addr_type = BLE_HCI_ADV_PEER_ADDR_PUBLIC;
    adv.adv_itvl_min = BLE_HCI_ADV_ITVL_NONCONN_MIN;
    adv.adv_itvl_max = adv_itvl;
    rc = host_hci_cmd_le_set_adv_params(&adv);
    assert(rc == 0);

    /* Set advertising data */
    rc = host_hci_cmd_le_set_adv_data(&g_host_adv_data[0], adv_len);
    assert(rc == 0);
}

void
bletest_init_scanner(void)
{
    int rc;

    /* Set scanning parameters */
    rc = host_hci_cmd_le_set_scan_params(BLE_HCI_SCAN_TYPE_ACTIVE,
                                         BLETEST_CFG_SCAN_ITVL,
                                         BLETEST_CFG_SCAN_WINDOW,
                                         BLE_HCI_ADV_OWN_ADDR_PUBLIC,
                                         BLE_HCI_SCAN_FILT_NO_WL);
    assert(rc == 0);
}

void 
host_task_handler(void *arg)
{
    int rc;

    /* Init the console */
    rc = console_init(NULL);
    assert(rc == 0);

    /* Initialize host HCI */
    host_hci_init();

    /* Initialize the BLE LL */
    ll_init();

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
    /* Initialize the advertiser */
    bletest_init_advertising();
#endif

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_SCANNER)
    /* Initialize the scanner */
    bletest_init_scanner();
#endif

    /* Init bletest varibles */
    bletest_state = 0;
    g_next_os_time = os_time_get();

    /* We are initialized */
    console_printf("Nimble stack initialized");

    /* Call the host hci task */
    host_hci_task(arg);
}

void
bletest_execute(void)
{
    int rc;

#if (BLETEST_CFG_ROLE == BLETEST_ROLE_ADVERTISER)
    /* Enable advertising */
    if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
        if (bletest_state) {
            rc = host_hci_cmd_le_set_adv_enable(0);
            assert(rc == 0);
            bletest_state = 0;
        } else {
            rc = host_hci_cmd_le_set_adv_enable(1);
            assert(rc == 0);
            bletest_state = 1;
        }
        g_next_os_time += (OS_TICKS_PER_SEC * 60);
    }
#endif
#if (BLETEST_CFG_ROLE == BLETEST_ROLE_SCANNER)
    /* Enable scanning */
    if ((int32_t)(os_time_get() - g_next_os_time) >= 0) {
        if (bletest_state) {
            rc = host_hci_cmd_le_set_scan_enable(0, 1);
            assert(rc == 0);
            bletest_state = 0;
        } else {
            rc = host_hci_cmd_le_set_scan_enable(1, 1);
            assert(rc == 0);
            bletest_state = 1;
        }
        g_next_os_time += (OS_TICKS_PER_SEC * 60);
    }
#endif
}

/**
 * init_tasks
 *  
 * Called by main.c after os_init(). This function performs initializations 
 * that are required before tasks are running. 
 *  
 * @return int 0 success; error otherwise.
 */
int
init_tasks(void)
{
    os_task_init(&host_task, "host", host_task_handler, NULL, HOST_TASK_PRIO, 
                 OS_WAIT_FOREVER, host_stack, HOST_STACK_SIZE);

    tasks_initialized = 1;

    return 0;
}

/**
 * main
 *  
 * The main function for the project. This function initializes the os, calls 
 * init_tasks to initialize tasks (and possibly other objects), then starts the 
 * OS. We should not return from os start. 
 *  
 * @return int NOTE: this function should never return!
 */
int
main(void)
{
    int i;
    int rc;
    uint32_t seed;

    /* Initialize OS */
    os_init();

    /* Set cputime to count at 1 usec increments */
    rc = cputime_init(1000000);
    assert(rc == 0);

    rc = os_mempool_init(&g_mbuf_mempool, MBUF_NUM_MBUFS, 
            MBUF_MEMBLOCK_SIZE, &g_mbuf_buffer[0], "mbuf_pool");

    rc = os_mbuf_pool_init(&g_mbuf_pool, &g_mbuf_mempool, 
                           sizeof(struct ble_mbuf_hdr), MBUF_MEMBLOCK_SIZE, 
                           MBUF_NUM_MBUFS);
    assert(rc == 0);

    /* Dummy device address */
    g_dev_addr[0] = 0x00;
    g_dev_addr[1] = 0x00;
    g_dev_addr[2] = 0x00;
    g_dev_addr[3] = 0x88;
    g_dev_addr[4] = 0x88;
    g_dev_addr[5] = 0x08;

    /* 
     * Seed random number generator with least significant bytes of device
     * address.
     */ 
    seed = 0;
    for (i = 0; i < 4; ++i) {
        seed |= g_dev_addr[i];
        seed <<= 8;
    }
    srand(seed);

    /* Set the led pin as an output */
    g_led_pin = LED_BLINK_PIN;
    gpio_init_out(g_led_pin, 1);

    /* Init tasks */
    init_tasks();

    /* Start the OS */
    os_start();

    /* os start should never return. If it does, this should be an error */
    assert(0);

    return rc;
}

