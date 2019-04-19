#pragma once
#include "disastrOS.h"
#include <stdio.h>
#include "disastrOS_constants.h"

void test_semOpen(int id);
void test_semClose(int id);
void test_semWait(int id);
void test_semPost(int id);