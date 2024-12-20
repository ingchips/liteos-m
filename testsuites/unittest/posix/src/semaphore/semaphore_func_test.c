/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <time.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include "ohos_types.h"
#include "posix_test.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"

#define NANO_S 1000000000

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixSemaphoreFuncTestSuite
 */
LITE_TEST_SUIT(Posix, PosixSemaphore, PosixSemaphoreFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixSemaphoreFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixSemaphoreFuncTestSuiteTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

// get cur-time plus ms
struct timespec GetDelayedTime(unsigned int ms)
{
    LOG("GetDelayedTime ms = %u", ms);
    struct timespec ts = { 0 };
    const unsigned int nsecPerSec = 1000000000;
    const unsigned int nsecPerMs = 1000000;
    unsigned long long setTimeNs = ms * nsecPerMs;
    struct timespec tsNow = { 0 };
    clock_gettime(CLOCK_REALTIME, &tsNow);
    LOG("Cur time tv_sec: %lld, tv_nsec: %ld", tsNow.tv_sec, tsNow.tv_nsec);
    ts.tv_sec = tsNow.tv_sec + (tsNow.tv_nsec + setTimeNs) / nsecPerSec;
    ts.tv_nsec = (tsNow.tv_nsec + setTimeNs) % nsecPerSec;
    return ts;
}

// calculate time difference, in ms
int GetTimeDiff(struct timespec ts1, struct timespec ts2)
{
    const int nsecPerSec = 1000000000;
    const int nsecPerMs = 1000000;
    int ms = (ts1.tv_sec - ts2.tv_sec) * nsecPerSec + (ts1.tv_nsec - ts2.tv_nsec);
    ms = ms / nsecPerMs;
    LOG("different result: %d (ms)", ms);
    return ms;
}

/* *
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_001
 * @tc.name     sem_timedwait get semaphore timeout
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixSemaphoreFuncTestSuite, testIpcSem_Timedwait001, Function | MediumTest | Level1)
{
    struct timespec ts = { 0 };
    struct timespec tsNow = { 0 };
    sem_t sem;

    ICUNIT_ASSERT_EQUAL(sem_init((sem_t *)&sem, 0, 0), 0, 0);

    ts = GetDelayedTime(100);
    LOG("predicted time:%lld, %ld", ts.tv_sec, ts.tv_nsec);
    if (sem_timedwait((sem_t *)&sem, &ts) == -1) {
        ICUNIT_ASSERT_EQUAL(errno, ETIMEDOUT, errno);
    } else {
        LOG("\n> sem_timedwait return unexpected");
    }

    clock_gettime(CLOCK_REALTIME, &tsNow);
    LOG("tsNow %lld, %ld", tsNow.tv_sec, tsNow.tv_nsec);
    int timeDiff = GetTimeDiff(tsNow, ts); // calculate time different
    LOG("timeDiff %d", timeDiff);
    ICUNIT_ASSERT_EQUAL(abs(timeDiff) < 20, TRUE, 0);

    ICUNIT_ASSERT_EQUAL(sem_destroy((sem_t *)&sem), 0, 0);
    return 0;
}

/* *
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_001
 * @tc.name     sem_timedwait get semaphore success
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixSemaphoreFuncTestSuite, testIpcSem_Timedwait002, Function | MediumTest | Level1)
{
    struct timespec ts = { 0 };
    struct timespec tsNow = { 0 };
    struct timespec tsBegin = { 0 };
    sem_t sem;

    ICUNIT_ASSERT_EQUAL(sem_init((sem_t *)&sem, 0, 1), 0, 0);

    ts = GetDelayedTime(100);
    LOG("\n ts %lld, %ld", ts.tv_sec, ts.tv_nsec);
    clock_gettime(CLOCK_REALTIME, &tsBegin);
    int ret = sem_timedwait((sem_t *)&sem, &ts);
    clock_gettime(CLOCK_REALTIME, &tsNow);

    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    LOG("\n tsBegin %lld, %ld, tsNow %lld, %ld", tsBegin.tv_sec, tsBegin.tv_nsec, tsNow.tv_sec, tsNow.tv_nsec);
    int timeDiff = GetTimeDiff(tsNow, tsBegin); // calculate time different
    LOG("\n timeDiff %d", timeDiff);
    ICUNIT_ASSERT_WITHIN_EQUAL(timeDiff, timeDiff, 19, 0);

    // try get semaphore again
    ts = GetDelayedTime(100);
    LOG("\n ts %lld, %ld", ts.tv_sec, ts.tv_nsec);
    ret = sem_timedwait((sem_t *)&sem, &ts);
    clock_gettime(CLOCK_REALTIME, &tsNow);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, ETIMEDOUT, errno);
    LOG("\n tsNow %lld, %ld", tsNow.tv_sec, tsNow.tv_nsec);
    timeDiff = GetTimeDiff(tsNow, tsBegin); // calculate time different
    LOG("\n wait timeDiff %d", timeDiff);

    ICUNIT_ASSERT_EQUAL(sem_destroy((sem_t *)&sem), 0, 0);
    return 0;
}

/* *
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_003
 * @tc.name     {sem_timedwait} get semaphore, wait time abnormal, tv_nsec less than 0
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixSemaphoreFuncTestSuite, testIpcSem_Timedwait003, Function | MediumTest | Level3)
{
    int ret;
    struct timespec ts;
    sem_t sem;

    ts.tv_sec = 0;
    ts.tv_nsec = 200000;
    ret = sem_timedwait((sem_t *)&sem, &ts);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, errno);

    ret = sem_init((sem_t *)&sem, 0, 0);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ts.tv_sec = time(NULL);
    ts.tv_nsec = -2;
    ret = sem_timedwait((sem_t *)&sem, &ts);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, errno);

    ts.tv_sec = time(NULL);
    ts.tv_nsec = NANO_S;
    ret = sem_timedwait((sem_t *)&sem, &ts);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, errno);

    ret = sem_timedwait((sem_t *)&sem, NULL);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, errno);

    ret = sem_destroy((sem_t *)&sem);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ts.tv_sec = time(NULL);
    ts.tv_nsec = 2000000;
    ret = sem_timedwait(NULL, &ts);
    ICUNIT_ASSERT_EQUAL(ret, -1, ret);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, errno);
    return 0;
}

/* *
 * @tc.number   SUB_KERNEL_IPC_SEM_TIMEDWAIT_004
 * @tc.name     sem_trywait get semaphore success
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixSemaphoreFuncTestSuite, testIpcSem_Trywait004, Function | MediumTest | Level3)
{
    sem_t sem;
    int val;
    int ret;

    ret = sem_init(&sem, 0, 1);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = sem_trywait(&sem);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = sem_getvalue(&sem, &val);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    if (val <= 0) {
        sem_destroy(&sem);
        return LOS_OK;
    } else {
        ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    }
    return 0;
}

RUN_TEST_SUITE(PosixSemaphoreFuncTestSuite);

void PosixSemaphoreFuncTest()
{
    PRINT_EMG("begin PosixSemaphoreFuncTest....");
    RUN_ONE_TESTCASE(testIpcSem_Timedwait001);
    RUN_ONE_TESTCASE(testIpcSem_Timedwait002);
    RUN_ONE_TESTCASE(testIpcSem_Timedwait003);
    RUN_ONE_TESTCASE(testIpcSem_Trywait004);

    return;
}