#include "KompexSQLitePrerequisites.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "KompexSQLiteStreamRedirection.h"
#include "KompexSQLiteBlob.h"

#include <exception>
#include <iostream>
#include <thread>
#include <algorithm>
#include <vector>

#include "gtest/gtest.h"



int main(int argc, char **argv)
{
    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::GTEST_FLAG(print_time) = false;

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

