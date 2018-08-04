// Copyright (c) 2012-2013 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "checkpoints.h"
#include "misc_log_ex.h"

#define ADD_CHECKPOINT(h, hash)  CHECK_AND_ASSERT(checkpoints.add_checkpoint(h,  hash), false);

namespace currency {
  inline bool create_checkpoints(currency::checkpoints& checkpoints)
  {
#ifndef TESTNET
    ADD_CHECKPOINT(1,       "3851c2a1717754c1ca52f2238d1c920d5aee6971a92f297bf67b608dea18ddb3");
    ADD_CHECKPOINT(100,     "acb68b26f4dd8664597dc23505d644fb643f5bc40192c1a05af482b343b79911");
    ADD_CHECKPOINT(1000,    "28849ae2cdef618fd1012658c247de00f53ceb77fb586eb1e9988a9dd36eb537");
    ADD_CHECKPOINT(2000,    "b6e55197e706fadc199803c61b6428c67fe6a31b417b03957f54a650b0ceddd4");
    ADD_CHECKPOINT(3000,    "f186237be9683e334f2f4876516d76d245b063b91124e9fa44e61e26c639eb22");
    ADD_CHECKPOINT(5000,    "0ea251832a689b4b58a8b06fb59644c9ae08d4cab9263cd273e4f21c0891efed");
    ADD_CHECKPOINT(10000,  "0c175e7313dc3c2a3d6a3f4585b5423046faca6b6a01595494e3948717400cf3");
    ADD_CHECKPOINT(20000,  "ed7a07bcb2c8e13329488c199462f316164da3b163b220c802a63dcd29caa1ac");
    ADD_CHECKPOINT(30000,  "b0ea39010952188195dcf87541b078baf1c67574701ee12a74c8df52af0765fd");
    ADD_CHECKPOINT(400000,  "2c9c968b65436f4c84178c230481a83abaf5262827d768d603c0809e93839bed");
    ADD_CHECKPOINT(450000,  "4ca1791f3a3aaa33f28b401cd82905bb261560c29a93ff84e01ed771cea3e8b3");
    ADD_CHECKPOINT(1000000,  "362ce2a2ee63c7aea9df453c1062caeb008b3b903b8e0df8d0bb14c965482e63");
#endif
    return true;
  }
}
