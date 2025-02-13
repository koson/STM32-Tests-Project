/* ----------------------------------------------------------------------------
 * Copyright (C) 2010-2014 ARM Limited. All rights reserved.
 *
 * $Date:        19. March 2015
 * $Revision: 	V.1.4.5
 *
 * Project: 	    CMSIS DSP Library
 * Title:		arm_float_to_q15.c
 *
 * Description:	Converts the elements of the floating-point vector to Q15 vector.
 *
 * Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   - Neither the name of ARM LIMITED nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------- */

#include "arm_math.h"

/**
 * @ingroup groupSupport
 */

/**
 * @addtogroup float_to_x
 * @{
 */

/**
 * @brief Converts the elements of the floating-point vector to Q15 vector.
 * @param[in]       *pSrc points to the floating-point input vector
 * @param[out]      *pDst points to the Q15 output vector
 * @param[in]       blockSize length of the input vector
 * @return none.
 *
 * \par Description:
 * \par
 * The equation used for the conversion process is:
 * <pre>
 * 	pDst[n] = (q15_t)(pSrc[n] * 32768);   0 <= n < blockSize.
 * </pre>
 * \par Scaling and Overflow Behavior:
 * \par
 * The function uses saturating arithmetic.
 * Results outside of the allowable Q15 range [0x8000 0x7FFF] will be saturated.
 * \note
 * In order to apply rounding, the library should be rebuilt with the ROUNDING macro
 * defined in the preprocessor section of project options.
 *
 */

void arm_float_to_q15(float32_t* pSrc, q15_t* pDst, uint32_t blockSize)
{
   float32_t* pIn = pSrc; /* Src pointer */
   uint32_t blkCnt; /* loop counter */

#ifdef ARM_MATH_ROUNDING

   float32_t in;

#endif /*      #ifdef ARM_MATH_ROUNDING        */

#ifndef ARM_MATH_CM0_FAMILY

   /* Run the below code for Cortex-M4 and Cortex-M3 */

   /*loop Unrolling */
   blkCnt = blockSize >> 2u;

   /* First part of the processing with loop unrolling.  Compute 4 outputs at a time.
    ** a second loop below computes the remaining 1 to 3 samples. */
   while (blkCnt > 0u)
   {
#   ifdef ARM_MATH_ROUNDING
      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0.0f ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0.0f ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0.0f ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0.0f ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

#   else

      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);

#   endif /*      #ifdef ARM_MATH_ROUNDING        */

      /* Decrement the loop counter */
      blkCnt--;
   }

   /* If the blockSize is not a multiple of 4, compute any remaining output samples here.
    ** No loop unrolling is used. */
   blkCnt = blockSize % 0x4u;

   while (blkCnt > 0u)
   {
#   ifdef ARM_MATH_ROUNDING
      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0.0f ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

#   else

      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);

#   endif /*      #ifdef ARM_MATH_ROUNDING        */

      /* Decrement the loop counter */
      blkCnt--;
   }

#else

   /* Run the below code for Cortex-M0 */

   /* Loop over blockSize number of values */
   blkCnt = blockSize;

   while (blkCnt > 0u)
   {

#   ifdef ARM_MATH_ROUNDING
      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      in = *pIn++;
      in = (in * 32768.0f);
      in += in > 0 ? 0.5f : -0.5f;
      *pDst++ = (q15_t)(__SSAT((q31_t)(in), 16));

#   else

      /* C = A * 32768 */
      /* convert from float to q15 and then store the results in the destination buffer */
      *pDst++ = (q15_t)__SSAT((q31_t)(*pIn++ * 32768.0f), 16);

#   endif /*      #ifdef ARM_MATH_ROUNDING        */

      /* Decrement the loop counter */
      blkCnt--;
   }

#endif /* #ifndef ARM_MATH_CM0_FAMILY */
}

/**
 * @} end of float_to_x group
 */
