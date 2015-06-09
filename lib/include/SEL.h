/**
 *  @file    SEL.h
 *  @brief   SEL library interface header
 *  @date    Date Created:: 11/05/13 Pavel Voylov
 *           $Date:: 2013-11-05 02:13#$
 *  @author  Copyright (c) 2013 Infopulse Ukraine. All rights reserved.
 */

#pragma once

#include <windows.h>

#ifdef bayeslib_EXPORTS
#  define SEL_API __declspec(dllexport)
#else
#  define SEL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
  extern "C" {
#endif
      
SEL_API double  __cdecl  SEL_RegisterNotificationCB( int );

#ifdef __cplusplus
  };  
#endif
