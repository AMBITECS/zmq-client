//-----------------------------------------------------------------------------
// Copyright 2025 Ambitecs
//-----------------------------------------------------------------------------
// Internal buffers for I/O and memory.
// These buffers are defined in the auto-generated glueVars.cpp file
//-----------------------------------------------------------------------------
#ifndef PRO_GATEWAY_H
#define PRO_GATEWAY_H

#include "iec_types.h"
#include "binder.h"
#include "reg_server.h"

// Special functions count
constexpr unsigned short SPEC_FUNC_COUNT = 16;

// lock for the buffers
extern std::mutex bufferLock;

#endif //PRO_GATEWAY_H
