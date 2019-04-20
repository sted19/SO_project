#pragma once
#include "disastrOS.h"
#include <stdio.h>
#include "disastrOS_constants.h"

int test_semOpen(int id);
void test_semClose(int fd);
void test_semWait(int fd);
void test_semPost(int fd);