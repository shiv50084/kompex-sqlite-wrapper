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

// Test that a sqlite file is created.
TEST(TestKompexSQLiteWrapper, test_create_sqlitedb_memory)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    // Move database to memory, all the queries will be
    // executed in memory.
    pDatabase->MoveDatabaseToMemory();

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    // Create table
    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");
    // Insert data
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (1, 'Lehmann', 'Jamie', 20, 65.5)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (2, 'Burgdorf', 'Peter', 55, NULL)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (3, 'Lehmann', 'Fernando', 18, 70.2)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");

    // A select statement, in database in memory
    pStmt->Sql("SELECT firstName, userID, age, weight FROM user WHERE lastName = 'Lehmann';");

    // Expect 3 rows
    ASSERT_EQ(pStmt->GetNumberOfRows(), 3);

    int rowNum = 0;
    while(pStmt->FetchRow())
    {
        // Expect 4 columns for each row
        ASSERT_EQ(pStmt->GetDataCount(), 4);

        switch(rowNum)
        {
            // Get the column value using the column number.
            case 0:
                ASSERT_EQ(pStmt->GetColumnString(0),"Jamie");
                ASSERT_EQ(pStmt->GetColumnInt(1),1);
                ASSERT_EQ(pStmt->GetColumnInt(2),20);
                ASSERT_EQ(pStmt->GetColumnDouble(3),65.5);
                break;
            case 1:
                ASSERT_EQ(pStmt->GetColumnString(0),"Fernando");
                ASSERT_EQ(pStmt->GetColumnInt(1),3);
                ASSERT_EQ(pStmt->GetColumnInt(2),18);
                ASSERT_EQ(pStmt->GetColumnDouble(3),70.2);
                break;
            case 2:
                ASSERT_EQ(pStmt->GetColumnString(0),"Carlene");
                ASSERT_EQ(pStmt->GetColumnInt(1),4);
                ASSERT_EQ(pStmt->GetColumnInt(2),17);
                ASSERT_EQ(pStmt->GetColumnDouble(3),50.8);
                break;
        }
        rowNum++;
    }

    pStmt->FreeQuery();

    // Save database to a file
    std::string filenameTestDbFromMemory("testMemory.db");
    pDatabase->SaveDatabaseFromMemoryToFile(filenameTestDbFromMemory);

    // Check if the file was created.
    std::ifstream infile(filenameTestDbFromMemory.c_str());
    ASSERT_TRUE(infile.good());

    // A select statement, in database in file
    pStmt->Sql("SELECT firstName, userID, age, weight FROM user WHERE lastName = 'Lehmann';");

    // Expect 3 rows
    ASSERT_EQ(pStmt->GetNumberOfRows(), 3);

    rowNum = 0;
    while(pStmt->FetchRow())
    {
        // Expect 4 columns for each row
        ASSERT_EQ(pStmt->GetDataCount(), 4);

        switch(rowNum)
        {
            // Get the column value using the column number.
            case 0:
                ASSERT_EQ(pStmt->GetColumnString(0),"Jamie");
                ASSERT_EQ(pStmt->GetColumnInt(1),1);
                ASSERT_EQ(pStmt->GetColumnInt(2),20);
                ASSERT_EQ(pStmt->GetColumnDouble(3),65.5);
                break;
            case 1:
                ASSERT_EQ(pStmt->GetColumnString(0),"Fernando");
                ASSERT_EQ(pStmt->GetColumnInt(1),3);
                ASSERT_EQ(pStmt->GetColumnInt(2),18);
                ASSERT_EQ(pStmt->GetColumnDouble(3),70.2);
                break;
            case 2:
                ASSERT_EQ(pStmt->GetColumnString(0),"Carlene");
                ASSERT_EQ(pStmt->GetColumnInt(1),4);
                ASSERT_EQ(pStmt->GetColumnInt(2),17);
                ASSERT_EQ(pStmt->GetColumnDouble(3),50.8);
                break;
        }
        rowNum++;
    }

    pStmt->FreeQuery();

    // Remove files
    std::remove(filenameTestDb.c_str());
    std::remove(filenameTestDbFromMemory.c_str());
    delete pStmt;
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_create_sqlitedb_file_populate_db_with_sqlstatement)
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
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");

    // A select statement
    pStmt->Sql("SELECT firstName, userID, age, weight FROM user WHERE lastName = 'Lehmann';");

    // Expect 3 rows
    ASSERT_EQ(pStmt->GetNumberOfRows(), 3);

    int rowNum = 0;
    while(pStmt->FetchRow())
    {
        // Expect 4 columns for each row
        ASSERT_EQ(pStmt->GetDataCount(), 4);

        switch(rowNum)
        {
            // Get the column value using the column number.
            case 0:
                ASSERT_EQ(pStmt->GetColumnString(0),"Jamie");
                ASSERT_EQ(pStmt->GetColumnInt(1),1);
                ASSERT_EQ(pStmt->GetColumnInt(2),20);
                ASSERT_EQ(pStmt->GetColumnDouble(3),65.5);
                break;
            case 1:
                ASSERT_EQ(pStmt->GetColumnString(0),"Fernando");
                ASSERT_EQ(pStmt->GetColumnInt(1),3);
                ASSERT_EQ(pStmt->GetColumnInt(2),18);
                ASSERT_EQ(pStmt->GetColumnDouble(3),70.2);
                break;
            case 2:
                ASSERT_EQ(pStmt->GetColumnString(0),"Carlene");
                ASSERT_EQ(pStmt->GetColumnInt(1),4);
                ASSERT_EQ(pStmt->GetColumnInt(2),17);
                ASSERT_EQ(pStmt->GetColumnDouble(3),50.8);
                break;
        }

        rowNum++;
    }

    pStmt->FreeQuery();

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_create_sqlitedb_file_populate_db_with_prepared_statement)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    // Create table
    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");
    // Insert data with the following prepared statement
    pStmt->Sql("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (?, ?, ?, ?, ?)");
    pStmt->BindInt(1, 1);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Jamie");
    pStmt->BindInt(4, 20);
    pStmt->BindDouble(5, 65.5);
    // Execute and reset bindings.
    pStmt->Execute();
    pStmt->Reset();

    pStmt->BindInt(1, 2);
    pStmt->BindString(2, "Burgdorf");
    pStmt->BindString(3, "Peter");
    pStmt->BindInt(4, 55);
    pStmt->BindNull(5);
    pStmt->Execute();
    pStmt->Reset();

    pStmt->BindInt(1, 3);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Fernando");
    pStmt->BindInt(4, 18);
    pStmt->BindDouble(5, 70.2);
    pStmt->Execute();
    pStmt->Reset();

    pStmt->BindInt(1, 4);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Carlene");
    pStmt->BindInt(4, 17);
    pStmt->BindDouble(5, 50.8);
    pStmt->Execute();
    pStmt->Reset();

    pStmt->BindInt(1, 5);
    pStmt->BindString(2, "Murahama");
    pStmt->BindString(3, "Yura");
    pStmt->BindInt(4, 28);
    pStmt->BindDouble(5, 60.2);
    pStmt->Execute();
    // Clean up the insert query
    pStmt->FreeQuery();


    // A select statement
    pStmt->Sql("SELECT firstName, userID, age, weight FROM user WHERE lastName = 'Lehmann';");

    // Expect 3 rows
    ASSERT_EQ(pStmt->GetNumberOfRows(), 3);

    int rowNum = 0;
    while(pStmt->FetchRow())
    {
        // Expect 4 columns for each row
        ASSERT_EQ(pStmt->GetDataCount(), 4);

        switch(rowNum)
        {
            // Get the column value using the column name.
            case 0:
                ASSERT_EQ(pStmt->GetColumnString("firstName"),"Jamie");
                ASSERT_EQ(pStmt->GetColumnInt("userID"),1);
                ASSERT_EQ(pStmt->GetColumnInt("age"),20);
                ASSERT_EQ(pStmt->GetColumnDouble("weight"),65.5);
                break;
            case 1:
                ASSERT_EQ(pStmt->GetColumnString("firstName"),"Fernando");
                ASSERT_EQ(pStmt->GetColumnInt("userID"),3);
                ASSERT_EQ(pStmt->GetColumnInt("age"),18);
                ASSERT_EQ(pStmt->GetColumnDouble("weight"),70.2);
                break;
            case 2:
                ASSERT_EQ(pStmt->GetColumnString("firstName"),"Carlene");
                ASSERT_EQ(pStmt->GetColumnInt("userID"),4);
                ASSERT_EQ(pStmt->GetColumnInt("age"),17);
                ASSERT_EQ(pStmt->GetColumnDouble("weight"),50.8);
                break;
        }

        rowNum++;
    }

    pStmt->FreeQuery();

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}


TEST(TestKompexSQLiteWrapper, test_prepared_statement_with_repetition)
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
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");

    // A prepare statement
    pStmt->Sql("SELECT * FROM user WHERE userID=@id");

    for(int i = 1; i <= 3; ++i)
    {
        pStmt->BindInt(1, i);

        // Expect one row
        ASSERT_EQ(pStmt->GetNumberOfRows(), 1);

        while(pStmt->FetchRow())
        {
            switch(pStmt->GetColumnInt(0))
            {
                case 1:
                    ASSERT_EQ(pStmt->GetColumnInt(0),1);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Lehmann");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Jamie");
                    ASSERT_EQ(pStmt->GetColumnInt(3),20);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),65.5);
                    break;
                case 2:
                    ASSERT_EQ(pStmt->GetColumnInt(0),2);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Burgdorf");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Peter");
                    ASSERT_EQ(pStmt->GetColumnInt(3),55);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),0);
                    break;
                case 3:
                    ASSERT_EQ(pStmt->GetColumnInt(0),3);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Lehmann");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Fernando");
                    ASSERT_EQ(pStmt->GetColumnInt(3),18);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),70.2);
                    break;
            }
        }

        // Reset bindings.
        pStmt->Reset();
    }

    pStmt->FreeQuery();

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_insert_update_delete)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    // Create table
    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");
    // Insert data with the following prepared statement
    pStmt->Sql("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (@userID, @lastName, @firstName, @age, @weight)");
    pStmt->BindInt(1, 1);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Jamie");
    pStmt->BindInt(4, 20);
    pStmt->BindDouble(5, 65.5);
    // Execute and reset bindings.
    pStmt->Execute();
    pStmt->Reset();

    // Insert 1 row
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);

    pStmt->BindInt(1, 2);
    pStmt->BindString(2, "Burgdorf");
    pStmt->BindString(3, "Peter");
    pStmt->BindInt(4, 55);
    pStmt->BindNull(5);
    pStmt->Execute();
    pStmt->Reset();

    // Insert 1 row
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);

    pStmt->BindInt(1, 3);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Fernando");
    pStmt->BindInt(4, 18);
    pStmt->BindDouble(5, 70.2);
    pStmt->Execute();
    pStmt->Reset();

    // Insert 1 row
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);

    pStmt->BindInt(1, 4);
    pStmt->BindString(2, "Lehmann");
    pStmt->BindString(3, "Carlene");
    pStmt->BindInt(4, 17);
    pStmt->BindDouble(5, 50.8);
    pStmt->Execute();
    pStmt->Reset();

    // Insert 1 row
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);

    pStmt->BindInt(1, 5);
    pStmt->BindString(2, "Murahama");
    pStmt->BindString(3, "Yura");
    pStmt->BindInt(4, 28);
    pStmt->BindDouble(5, 60.2);
    pStmt->Execute();
    // Clean up the insert query
    pStmt->FreeQuery();

    // Insert 1 row
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);
    // Total changes 5
    ASSERT_EQ(pDatabase->GetTotalDatabaseChanges(), 5);

    // Check that the data inserted with no issue.
    // A prepare statement
    pStmt->Sql("SELECT * FROM user WHERE userID=@id");

    for(int i = 1; i <= 5; ++i)
    {
        pStmt->BindInt(1, i);

        // Expect one row
        ASSERT_EQ(pStmt->GetNumberOfRows(), 1);

        while(pStmt->FetchRow())
        {
            switch(pStmt->GetColumnInt(0))
            {
                case 1:
                    ASSERT_EQ(pStmt->GetColumnInt(0),1);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Lehmann");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Jamie");
                    ASSERT_EQ(pStmt->GetColumnInt(3),20);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),65.5);
                    break;
                case 2:
                    ASSERT_EQ(pStmt->GetColumnInt(0),2);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Burgdorf");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Peter");
                    ASSERT_EQ(pStmt->GetColumnInt(3),55);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),0);
                    break;
                case 3:
                    ASSERT_EQ(pStmt->GetColumnInt(0),3);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Lehmann");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Fernando");
                    ASSERT_EQ(pStmt->GetColumnInt(3),18);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),70.2);
                    break;
                case 4:
                    ASSERT_EQ(pStmt->GetColumnInt(0),4);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Lehmann");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Carlene");
                    ASSERT_EQ(pStmt->GetColumnInt(3),17);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),50.8);
                    break;
                case 5:
                    ASSERT_EQ(pStmt->GetColumnInt(0),5);
                    ASSERT_EQ(pStmt->GetColumnString(1),"Murahama");
                    ASSERT_EQ(pStmt->GetColumnString(2),"Yura");
                    ASSERT_EQ(pStmt->GetColumnInt(3),28);
                    ASSERT_EQ(pStmt->GetColumnDouble(4),60.2);
                    break;

            }
        }

        // Reset bindings.
        pStmt->Reset();
    }

    pStmt->FreeQuery();


    // Update data
    pStmt->Sql("UPDATE user SET lastName=@lastName, age=@age WHERE userID=@userID");

    pStmt->BindString(1, "Urushihara");
    pStmt->BindInt(2, 56);
    pStmt->BindInt(3, 2);
    pStmt->ExecuteAndFree();

    // Updated 1 row by the most recent sql query.
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 1);
    // Total changes 6
    ASSERT_EQ(pDatabase->GetTotalDatabaseChanges(), 6);

    // Check that the update was executed with success
    pStmt->Sql("SELECT * FROM user WHERE userID=@id");
    pStmt->BindInt(1, 2);

    ASSERT_EQ(pStmt->GetNumberOfRows(), 1);

    while(pStmt->FetchRow())
    {
        ASSERT_EQ(pStmt->GetColumnInt(0),2);
        ASSERT_EQ(pStmt->GetColumnString(1),"Urushihara");
        ASSERT_EQ(pStmt->GetColumnString(2),"Peter");
        ASSERT_EQ(pStmt->GetColumnInt(3),56);
        ASSERT_EQ(pStmt->GetColumnDouble(4),0);
    }

    pStmt->FreeQuery();


    // Delete data
    pStmt->SqlStatement("DELETE FROM user WHERE lastName = 'Lehmann'");

    // Deleted 3 rows by the most recent sql query.
    ASSERT_EQ(pDatabase->GetDatabaseChanges(), 3);
    // Total changes 9
    ASSERT_EQ(pDatabase->GetTotalDatabaseChanges(), 9);

    pStmt->Sql("SELECT * FROM user WHERE lastName=@lastName");
    pStmt->BindString(1,"Lehmann");

    ASSERT_EQ(pStmt->GetNumberOfRows(), 0);

    // If there are no results will be false.
    ASSERT_FALSE(pStmt->FetchRow());

    pStmt->FreeQuery();


    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_get_instant_results)
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
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");

    // Instant results
    ASSERT_EQ(pStmt->GetSqlResultString("SELECT lastName FROM user WHERE userID = 3"), "Lehmann");
    ASSERT_EQ(pStmt->GetSqlResultInt("SELECT userID FROM user WHERE firstName = 'Yura'"), 5);
    ASSERT_EQ(pStmt->GetSqlResultDouble("SELECT weight FROM user WHERE age = 20"), 65.5);

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

TEST(TestKompexSQLiteWrapper, test_transactions)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    // Create table
    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");

    try
    {
        pStmt->BeginTransaction();
        // Insert data
        pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (1, 'Lehmann', 'Jamie', 20, 65.5)");
        pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (2, 'Burgdorf', 'Peter', 55, NULL)");
        pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (3, 'Lehmann', 'Fernando', 18, 70.2)");
        pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
        pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");
        pStmt->CommitTransaction();
    }
    catch(Kompex::SQLiteException &exception)
    {
        exception.GetString();
        pStmt->RollbackTransaction();
    }

    // Instant results
    ASSERT_EQ(pStmt->GetSqlResultString("SELECT lastName FROM user WHERE userID = 3"), "Lehmann");
    ASSERT_EQ(pStmt->GetSqlResultInt("SELECT userID FROM user WHERE firstName = 'Yura'"), 5);
    ASSERT_EQ(pStmt->GetSqlResultDouble("SELECT weight FROM user WHERE age = 20"), 65.5);

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

void populate(Kompex::SQLiteDatabase *pDatabase, Kompex::SQLiteStatement *pStmt)
{
    // Insert data
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (1, 'Lehmann', 'Jamie', 20, 65.5)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (2, 'Burgdorf', 'Peter', 55, NULL)");
    usleep(10);
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (3, 'Lehmann', 'Fernando', 18, 70.2)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (4, 'Lehmann', 'Carlene', 17, 50.8)");
    pStmt->SqlStatement("INSERT INTO user (userID, lastName, firstName, age, weight) VALUES (5, 'Murahama', 'Yura', 28, 60.2)");
}

TEST(TestKompexSQLiteWrapper, test_populate_sqlitedb_separate_thread)
{
    Kompex::SQLiteDatabase *pDatabase = new Kompex::SQLiteDatabase(filenameTestDb.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

    // Check if the file was created.
    std::ifstream ifile(filenameTestDb.c_str());
    ASSERT_TRUE(ifile.good());

    Kompex::SQLiteStatement *pStmt = new Kompex::SQLiteStatement(pDatabase);

    pStmt->SqlStatement("CREATE TABLE user (userID INTEGER NOT NULL PRIMARY KEY, lastName VARCHAR(50) NOT NULL, firstName VARCHAR(50), age INTEGER, weight DOUBLE)");

    std::thread t(populate, pDatabase, pStmt);


    // No data in the table yet.
    ASSERT_EQ(pStmt->GetSqlResultString("SELECT lastName FROM user WHERE userID = 5"), "");
    ASSERT_EQ(pStmt->GetSqlResultString("SELECT firstName FROM user WHERE weight = 50.8"), "");

    sleep(1);

    // Now there are some data in the table.
    ASSERT_EQ(pStmt->GetSqlResultInt("SELECT userID FROM user WHERE firstName = 'Jamie'"), 1);
    ASSERT_EQ(pStmt->GetSqlResultDouble("SELECT weight FROM user WHERE age = 20"), 65.5);

    t.join();

    // Remove file
    std::remove(filenameTestDb.c_str());

    delete pStmt;
    delete pDatabase;
}

int main(int argc, char **argv)
{
    ::testing::GTEST_FLAG(color) = "yes";
    ::testing::GTEST_FLAG(print_time) = false;

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

