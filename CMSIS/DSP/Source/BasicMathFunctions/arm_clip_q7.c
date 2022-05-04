/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        arm_clip_q7.c
 * Description:  Floating-point vector addition
 *
 * $Date:        23 April 2021
 * $Revision:    V1.9.0
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2021 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dsp/basic_math_functions.h"

/**
  @ingroup groupMath
 */


/**
  @addtogroup BasicClip
  @{
 */

/**
  @brief         Elementwise fixed-point clipping
  @param[in]     pSrc          points to input values
  @param[out]    pDst          points to output clipped values
  @param[in]     low           lower bound
  @param[in]     high          higher bound
  @param[in]     numSamples    number of samples to clip
  @return        none
 */
#if defined(ARM_MATH_MVEI) && !defined(ARM_MATH_AUTOVECTORIZE)

#include "arm_helium_utils.h"
void arm_clip_q7(const q7_t * pSrc,
  q7_t * pDst,
  q7_t low,
  q7_t high,
  uint32_t numSamples)
{
    uint32_t  blkCnt;
    q7x16_t curVec0, curVec1;
    q7x16_t vecLow, vecHigh;

    vecLow = vdupq_n_s8(low);
    vecHigh = vdupq_n_s8(high);

    curVec0 = vld1q(pSrc);
    pSrc += 16;
    /*
     * unrolled x 2 to allow
     * vldr/vstr/vmin/vmax
     * stall free interleaving
     */
    blkCnt = numSamples >> 5;
    while (blkCnt--)
    {
        curVec0 = vmaxq(curVec0, vecLow);
        curVec1 = vld1q(pSrc);
        pSrc += 16;
        curVec0 = vminq(curVec0, vecHigh);
        vst1q(pDst, curVec0);
        pDst += 16;
        curVec1 = vmaxq(curVec1, vecLow);
        curVec0 = vld1q(pSrc);
        pSrc += 16;
        curVec1 = vminq(curVec1, vecHigh);
        vst1q(pDst, curVec1);
        pDst += 16;
    }
    /*
     * Tail handling
     */
    blkCnt = numSamples - ((numSamples >> 5) << 5);
    if (blkCnt >= 16)
    {
        curVec0 = vmaxq(curVec0, vecLow);
        curVec0 = vminq(curVec0, vecHigh);
        vst1q(pDst, curVec0);
        pDst += 16;
        curVec0 = vld1q(pSrc);
        pSrc += 16;
    }

    if (blkCnt > 0)
    {
        mve_pred16_t p0 = vctp8q(blkCnt & 0xf);
        curVec0 = vmaxq(curVec0, vecLow);
        curVec0 = vminq(curVec0, vecHigh);
        vstrbq_p(pDst, curVec0, p0);
    }
}

#else
void arm_clip_q7(const q7_t * pSrc,
  q7_t * pDst,
  q7_t low,
  q7_t high,
  uint32_t numSamples)
{
    uint32_t i;
    for (i = 0; i < numSamples; i++)
    {
        if (pSrc[i] > high)
            pDst[i] = high;
        else if (pSrc[i] < low)
            pDst[i] = low;
        else
            pDst[i] = pSrc[i];
    }
}
#endif /* defined(ARM_MATH_MVEI) */

/**
  @} end of BasicClip group
 */
