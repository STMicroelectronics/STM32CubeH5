"""
    The module [ comare.py ] gathers all oldest functions from HASH V1 and 
    the newest one from hash V2 driver
"""
from array import *

Functions_CMP_Array = [
     #[ old function name                 , new function name                  ,      algorithm,                 ,      OutputBuffer,           ,      Timeout,
     #SHA224
     ["HAL_HASHEx_SHA224_Accmlt_End_IT"   , "HAL_HASH_AccumulateLast_IT"       , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA224_Accmlt_End"      , "HAL_HASH_AccumulateLast"          , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ], 
     ["HAL_HASHEx_SHA224_Accmlt_IT"       , "HAL_HASH_Accumulate_IT"           , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],     
     ["HAL_HASHEx_SHA224_Accmlt"          , "HAL_HASH_Accumulate"              , "HASH_ALGOSELECTION_SHA224"            , "" ,                            "YES"],
     ["HAL_HASHEx_SHA224_Start_DMA"       , "HAL_HASH_Start_DMA"               , "HASH_ALGOSELECTION_SHA224"            , "YES" ,                         ""   ],
     ["HAL_HASHEx_SHA224_Start_IT"        , "HAL_HASH_Start_IT"                , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA224_Start"           , "HAL_HASH_Start"                   , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],
     ["HAL_HMACEx_SHA224_Step2_3_DMA"     , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA224_Step1_2_DMA"     , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA224_Start_DMA"       , "HAL_HASH_HMAC_Start_DMA"          , "HASH_ALGOSELECTION_SHA224"            , "YES" ,                         ""   ],
     ["HAL_HMACEx_SHA224_Step2_DMA"       , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA224_Start_IT"        , "HAL_HASH_HMAC_Start_IT"           , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],
     ["HAL_HMACEx_SHA224_Start"           , "HAL_HASH_HMAC_Start"              , "HASH_ALGOSELECTION_SHA224"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA224_Finish"          , "NONE"                             , "NONE"                                 , "" ,                            ""   ],

     #SHA256
     ["HAL_HASHEx_SHA256_Start_DMA"       , "HAL_HASH_Start_DMA"               , "HASH_ALGOSELECTION_SHA256"            , "YES" ,                         ""   ],     
     ["HAL_HASHEx_SHA256_Start"           , "HAL_HASH_Start"                   , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA256_Accmlt_End_IT"   , "HAL_HASH_AccumulateLast_IT"       , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA256_Accmlt_IT"       , "HAL_HASH_Accumulate_IT"           , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA256_Accmlt_End"      , "HAL_HASH_AccumulateLast"          , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA256_Accmlt"          , "HAL_HASH_Accumulate"              , "HASH_ALGOSELECTION_SHA256"            , "" ,                            "YES"   ],
     ["HAL_HASHEx_SHA256_Start_IT"        , "HAL_HASH_Start_IT"                , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HASHEx_SHA256_Finish"          , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA256_Start_IT"        , "HAL_HASH_HMAC_Start_IT"           , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HMACEx_SHA256_Start_DMA"       , "HAL_HASH_HMAC_Start_DMA"          , "HASH_ALGOSELECTION_SHA256"            , "YES" ,                         ""   ],
     ["HAL_HMACEx_SHA256_Start"           , "HAL_HASH_HMAC_Start"              , "HASH_ALGOSELECTION_SHA256"            , "" ,                            ""   ],
     ["HAL_HMACEx_SHA256_Step1_2_DMA"     , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA256_Step2_DMA"       , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA256_Step2_3_DMA"     , "NONE"                             , "NONE"                                 , "" ,                            ""   ],

     #MD5   
     ["HAL_HASH_MD5_Start_DMA"            , "NONE"                             , "NONE"                                 , "" ,                            ""   ],     
     ["HAL_HASH_MD5_Start"                , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Accmlt_IT"            , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Accmlt_End_IT"        , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Accmlt"               , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Accmlt_End"           , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Start_IT"             , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMAC_MD5_Start_IT"             , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMAC_MD5_Start_DMA"            , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_MD5_Step1_2_DMA"        , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_MD5_Step2_DMA"          , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_MD5_Step2_3_DMA"        , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_MD5_Finish"               , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMAC_MD5_Start"                , "NONE"                             , "NONE"                                 , "" ,                            ""   ],

     #SHA1
     ["HAL_HASH_SHA1_Accmlt_End_IT"       , "HAL_HASH_AccumulateLast_IT"       , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ], 
     ["HAL_HASH_SHA1_Accmlt_End"          , "HAL_HASH_AccumulateLast"          , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ],  
     ["HAL_HASH_SHA1_Accmlt_IT"           , "HAL_HASH_Accumulate_IT"           , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ], 
     ["HAL_HASH_SHA1_Accmlt"              , "HAL_HASH_Accumulate"              , "HASH_ALGOSELECTION_SHA1"              , "" ,                            "YES"   ],
     ["HAL_HASH_SHA1_Start_DMA"           , "HAL_HASH_Start_DMA"               , "HASH_ALGOSELECTION_SHA1"              , "YES" ,                         ""   ],
     ["HAL_HASH_SHA1_Start_IT"            , "HAL_HASH_Start_IT"                , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ],     
     ["HAL_HASH_SHA1_Start"               , "HAL_HASH_Start"                   , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ],


     ["HAL_HASH_SHA1_Finish"              , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMAC_SHA1_Start_IT"            , "HAL_HASH_HMAC_Start_IT"           , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ],
     ["HAL_HMAC_SHA1_Start_DMA"           , "HAL_HASH_HMAC_Start_DMA"          , "HASH_ALGOSELECTION_SHA1"              , "YES" ,                            ""   ],
     ["HAL_HMAC_SHA1_Start"               , "HAL_HASH_HMAC_Start"              , "HASH_ALGOSELECTION_SHA1"              , "" ,                            ""   ],
     ["HAL_HMACEx_SHA1_Step1_2_DMA"       , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA1_Step2_DMA"         , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HMACEx_SHA1_Step2_3_DMA"       , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
                                                                                                                                                         
     #General functions                                                                                                                                  
     ["HAL_HASH_GetStatus"                , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_ContextSaving"            , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_ContextRestoring"         , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_SwFeed_ProcessSuspend"    , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["HAL_HASH_DMAFeed_ProcessSuspend"   , "NONE"                             , "NONE"                                 , "" ,                            ""   ],
     ["NONE"                              , "HAL_HASH_ProcessSuspend"          , "NONE"                                 , "" ,                            ""   ],
     ["NONE"                              , "HAL_HASH_Suspend"                 , "NONE"                                 , "" ,                            ""   ],
     ["NONE"                              , "HAL_HASH_Resume"                  , "NONE"                                 , "" ,                            ""   ],
     ["NONE"                              , "HAL_HASH_SetConfig"               , "NONE"                                 , "" ,                            ""   ],
     ["NONE"                              , "HAL_HASH_GetConfig"               , "NONE"                                 , "" ,                            ""   ]]


Structures_CMP_Array = [
     ["HASH_InitTypeDef"                  , "HASH_ConfigTypeDef"        ],
     ["HashBuffSize"                      , "Size"                      ],
     ["HashITCounter"                     , "HashInCount_saved"         ],
     ["Phase"                             , "Phase_saved"               ],
     ["HashKeyCount"                      , "NONE"                      ],
     ["NbWordsAlreadyPushed"              , "NONE"                      ],
     ["HashKeyCount"                      , "NONE"                      ],
     ["HashKeyCount"                      , "NONE"                      ],
     ["HashKeyCount"                      , "NONE"                      ],
     ["HashKeyCount"                      , "NONE"                      ],
     ["pHashMsgBuffPtr"                   , "NONE"                      ]]

Defines_CMP_Array = [
     ["HASH_DATATYPE_32B"                 , "HASH_NO_SWAP"         ],
     ["HASH_DATATYPE_16B"                 , "HASH_HALFWORD_SWAP"   ],
     ["HASH_DATATYPE_8B"                  , "HASH_BYTE_SWAP"       ],
     ["HASH_DATATYPE_1B"                  , "HASH_BIT_SWAP"        ]]


     