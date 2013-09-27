#include "KompexSQLitePrerequisites.h"
#include "KompexSQLiteDatabase.h"
#include "KompexSQLiteStatement.h"
#include "KompexSQLiteException.h"
#include "KompexSQLiteStreamRedirection.h"
#include "KompexSQLiteBlob.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <thread>
#include <algorithm>
#include <vector>
#include <cstdio>

#include "gtest/gtest.h"

std::string filenameTestDb("test.db");

// Test that a sqlite file is created.
TEST(TestKompexSQLiteWrapper, test_create_sqlitedb_file)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    // Remove file
    std::remove(filenameTestDb.c_str());
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_create_sqlitedb_file_populate_db)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    // Create table
    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");
    // Insert data
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (1, 'Lehmann', 'Jamie', 20, 65.5)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (2, 'Burgdorf', 'Peter', 55, NULL)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (3, 'Lehmann', 'Fernando', 18, 70.2)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene ', 17, 50.8)");

    // Create query
    pStmt->GetTable("SELECT firstName, userID, age, weight FROM user WHERE lastName = 'Lehmann';", 13);


    // Remove file
    std::remove(filenameTestDb.c_str());
    delete pDatabase;

}

int main(int argc, char **argv)
{
    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::GTEST_FLAG(print_time) = false;

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

