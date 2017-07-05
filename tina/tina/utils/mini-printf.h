/*
 * The Minimal snprintf() implementation
 *
 * Copyright (c) 2013 Michal Ludvig <michal@logix.cz>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the auhor nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __MINI_PRINTF__
#define __MINI_PRINTF__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
    
/**
 * @defgroup mini-printf Mini-Printf
 * @ingroup Utils
 * 
 * Dieses Modul stellt minimale Funktionen zur grundlegenden 
 * formatierten Ausgabe in Puffer angegebener Länge bereit.

 * Die folgenden Format-Spezifizierer werden unterstützt:
 *  - u, d
 *  - x, X
 *  - c
 *  - s
 */
    
/**
 * @addtogroup mini-printf
 * @{
 */
 

/**
 * @brief Minimale printf-Implementierung.
 * @param[out] buffer Puffer, in den der formatierte String geschrieben wird.
 * @param[in] buffer_len Länge von buffer.
 * @param[in] fmt Format-String. 
 * @param[in] va Argument-Liste.
 * 
 * Diese Form kann zur Implementierung eigener printf-Funktionen genutzt werden.
 */
int mini_vsnprintf(char* buffer, unsigned int buffer_len, const char *fmt, va_list va);

/**
 * @brief Minimale printf-Implementierung.
 * @param[out] buffer Puffer, in den der formatierte String geschrieben wird.
 * @param[in] buffer_len Länge von buffer.
 * @param[in] fmt Format-String. 
 * 
 * Diese Form ist zur direkten Verwendung vorgesehen.
 */
int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#define vsnprintf mini_vsnprintf
#define snprintf mini_snprintf

#endif
