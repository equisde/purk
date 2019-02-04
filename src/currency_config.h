// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#define CURRENCY_MAX_BLOCK_NUMBER                     500000000
#define CURRENCY_MAX_BLOCK_SIZE                       500000000  // block header blob limit, never used!
#define CURRENCY_MAX_TX_SIZE                          1000000000
#define CURRENCY_PUBLIC_ADDRESS_TEXTBLOB_VER          0
#define CURRENCY_PUBLIC_ADDRESS_BASE58_PREFIX         0x3185 // wallet addresses start with "PK"

#define BLOCK_MAJOR_VERSION_1                         1
#define BLOCK_MAJOR_VERSION_2                         2
#define BLOCK_MAJOR_VERSION_3                         3
#define BLOCK_MAJOR_VERSION_4                         4

#define CURRENCY_MINED_MONEY_UNLOCK_WINDOW            10
#define CURRENT_TRANSACTION_VERSION                   1
#define CURRENT_BLOCK_MAJOR_VERSION                   1
#define CURRENT_BLOCK_MINOR_VERSION                   0
#define CURRENCY_BLOCK_FUTURE_TIME_LIMIT              60*60*2

#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW             60

#define TOTAL_MONEY_SUPPLY                            ((uint64_t)179424673000000) //  total number coins to be generated
#define DONATIONS_SUPPLY                              (TOTAL_MONEY_SUPPLY/100) 
#define EMISSION_SUPPLY                               (TOTAL_MONEY_SUPPLY - DONATIONS_SUPPLY) 

#define EMISSION_CURVE_CHARACTER                      21

#define CURRENCY_TO_KEY_OUT_RELAXED                   0
#define CURRENCY_TO_KEY_OUT_FORCED_NO_MIX             1

#define CURRENCY_REWARD_BLOCKS_WINDOW                 400
#define CURRENCY_BLOCK_GRANTED_FULL_REWARD_ZONE       30000 // size of block (bytes) after which reward for block calculated using block size
#define CURRENCY_COINBASE_BLOB_RESERVED_SIZE          600
#define CURRENCY_MAX_TRANSACTION_BLOB_SIZE            (CURRENCY_BLOCK_GRANTED_FULL_REWARD_ZONE - CURRENCY_COINBASE_BLOB_RESERVED_SIZE*2) 
#define CURRENCY_DISPLAY_DECIMAL_POINT                6

// NEW VARIABLES
#define TRANSACTION_VERSION_2                         2

#define COIN                                            ((uint64_t)100000000) // pow(10, 8) // number of smallest units in one coin
#define DEFAULT_DUST_THRESHOLD                          ((uint64_t)100) // pow(10, 2) // 

#define DEFAULT_FEE                                     ((uint64_t)1000) // pow(10,3) 
#define TX_POOL_MINIMUM_FEE                             ((uint64_t)1000) // pow(10,3)

#define ORPHANED_BLOCKS_MAX_COUNT                       100

#define DIFFICULTY_TARGET                               90 // seconds
#define DIFFICULTY_WINDOW                               720 // blocks
#define DIFFICULTY_LAG                                  15  // !!!
#define DIFFICULTY_CUT                                  60  // timestamps to cut after sorting
#define DIFFICULTY_BLOCKS_COUNT                         (DIFFICULTY_WINDOW + DIFFICULTY_LAG)

#define CURRENCY_BLOCK_PER_DAY                          ((60*60*24)/(DIFFICULTY_TARGET))

#define CURRENCY_LOCKED_TX_ALLOWED_DELTA_SECONDS        (DIFFICULTY_TARGET * CURRENCY_LOCKED_TX_ALLOWED_DELTA_BLOCKS)
#define CURRENCY_LOCKED_TX_ALLOWED_DELTA_BLOCKS         1

#define DIFFICULTY_BLOCKS_ESTIMATE_TIMESPAN             DIFFICULTY_TARGET //just alias

#define BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT          10000  //by default, blocks ids count in synchronizing
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT              200    //by default, blocks count in blocks downloading
#define CURRENCY_PROTOCOL_HOP_RELAX_COUNT               3      //value of hop, after which we use only announce of new block

#define CURRENCY_ALT_BLOCK_LIVETIME_COUNT               (720*7)//one week
#define CURRENCY_MEMPOOL_TX_LIVETIME                    86400 //seconds, one day
#define CURRENCY_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME     (CURRENCY_ALT_BLOCK_LIVETIME_COUNT*DIFFICULTY_TARGET) //seconds, one week

#ifndef TESTNET
#define P2P_DEFAULT_PORT                                10110
#define RPC_DEFAULT_PORT                                10120
#else 
#define P2P_DEFAULT_PORT                                30110
#define RPC_DEFAULT_PORT                                30120
#endif

#define COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT           1000

#define P2P_LOCAL_WHITE_PEERLIST_LIMIT                  1000
#define P2P_LOCAL_GRAY_PEERLIST_LIMIT                   5000

#define P2P_DEFAULT_CONNECTIONS_COUNT                   8
#define P2P_DEFAULT_HANDSHAKE_INTERVAL                  60           //seconds
#define P2P_DEFAULT_PACKET_MAX_SIZE                     50000000     //50000000 bytes maximum packet size
#define P2P_DEFAULT_PEERS_IN_HANDSHAKE                  250
#define P2P_DEFAULT_CONNECTION_TIMEOUT                  5000       //5 seconds
#define P2P_DEFAULT_PING_CONNECTION_TIMEOUT             2000       //2 seconds
#define P2P_DEFAULT_INVOKE_TIMEOUT                      60*2*1000  //2 minutes
#define P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT            5000       //5 seconds
#define P2P_MAINTAINERS_PUB_KEY                         "596e179fc2a4d30e8f801391e8867ad46f6da404b65de8f5294006d2d7a1fb6f"
#define P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT       70
#define P2P_FAILED_ADDR_FORGET_SECONDS                  (60*60)     //1 hour

#define P2P_IP_BLOCKTIME                                (60*60*24)  //24 hour
#define P2P_IP_FAILS_BEFOR_BLOCK                        10
#define P2P_IDLE_CONNECTION_KILL_INTERVAL               (5*60) //5 minutes

/* 1% Purk will be donated to the development of the project */

  #define CURRENCY_DONATIONS_ADDRESS                     "PK39caXJuVeQPKe4LVWnjA7PpTSyu4z7iTL4HStXThvvDgv6zFo2qZzNuC4YmzDdB7gkLwNJNi8UZdxrQP3XkCXH367py37VJ"
  #define CURRENCY_DONATIONS_ADDRESS_TRACKING_KEY        "c02666daec755a008978c21b802c7dab2e5687ee0371931efd2206031527cf0e"
  
/* 10% of development fund will be donated to the PR fund of the project */

  #define CURRENCY_ROYALTY_ADDRESS                       "PK4FtG7uV97DnqqGcSUZMDG1irpRB3P2YXsLjsHDFr4Zc18LkC6xQPz8tFCYtSZWr9JKBsd7h8gW4YNzmdS9kLn31h6BWnHmk"
  #define CURRENCY_ROYALTY_ADDRESS_TRACKING_KEY          "d199e5be4c37ebf63c85839c7a484a99ac76ff9aaf91ffa4f0bba6a374eb2500"

#ifdef TESTNET
  #define CURRENCY_DONATIONS_INTERVAL                     10
#else
  #define CURRENCY_DONATIONS_INTERVAL                     720
#endif

#define ALLOW_DEBUG_COMMANDS

#define CURRENCY_NAME_BASE                              "Purk"
#define CURRENCY_NAME_SHORT_BASE                        "purk"
#ifndef TESTNET
#define CURRENCY_NAME                                   CURRENCY_NAME_BASE
#define CURRENCY_NAME_SHORT                             CURRENCY_NAME_SHORT_BASE
#else
#define CURRENCY_NAME                                   CURRENCY_NAME_BASE"_testnet"
#define CURRENCY_NAME_SHORT                             CURRENCY_NAME_SHORT_BASE"_testnet"
#endif

#define CURRENCY_POOLDATA_FILENAME                      "poolstate.bin"
#define CURRENCY_BLOCKCHAINDATA_FILENAME                "blockchain.bin"
#define CURRENCY_BLOCKCHAINDATA_TEMP_FILENAME           "blockchain.bin.tmp"
#define P2P_NET_DATA_FILENAME                           "p2pstate.bin"
#define MINER_CONFIG_FILENAME                           "miner_conf.json"
#define GUI_CONFIG_FILENAME                             "gui_conf.json"
