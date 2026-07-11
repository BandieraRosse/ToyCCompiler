/* EXPECT_ERR: cannot find 'nonexistent.h' */
#include "nonexistent.h"
#error "must fail to test the error message above"
