#pragma once

#include "ir_variant.h"

// ExpressionIR
#include "call/call.h"
#include "const/const.h"
#include "eseq/eseq.h"
#include "mem/mem.h"
#include "name/name.h"
#include "temp/temp.h"
#include "bin-op/bin-op.h"      // Needs to be last because of std::visit on ExpressionIR

// StatementIR
#include "cjump/cjump.h"
#include "comp-unit/comp-unit.h"
#include "func-decl/func-decl.h"
#include "jump/jump.h"
#include "label/label.h"
#include "move/move.h"
#include "return/return.h"
#include "seq/seq.h"
