/**
 * @file    Backup.h
 * @ingroup SQLiteCpp
 * @brief   Backup is used to backup a database file in a safe and online way.
 *
 * Copyright (c) 2015-2015 Shibao HONG (shibaohong@outlook.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <sqlite3.h>

#include <SQLiteCpp/Database.h>

#include <string>

namespace SQLite
{

/**
 * @brief RAII encapsulation of a SQLite Database Backup process.
 *
 * A Backup object is used to backup a source database file to a destination database file
 * in a safe and online way.
 *
 * Resource Acquisition Is Initialization (RAII) means that the Backup Resource
 * is allocated in the constructor and released in the destructor, so that there is
 * no need to worry about memory management or the validity of the underlying SQLite Backup.
 *
 * Thread-safety: a Backup object shall not be shared by multiple threads, because :
 * 1) in the SQLite "Thread Safe" mode, "SQLite can be safely used by multiple threads
 *    provided that no single database connection is used simultaneously in two or more threads."
 * 2) the SQLite "Serialized" mode is not supported by SQLiteC++,
 *    because of the way it shares the underling SQLite precompiled statement
 *    in a custom shared pointer (See the inner class "Statement::Ptr").
 */
class Backup
{
public:
    /**
     * @brief Initialize a SQLite Backup object.
     *
     * Initialize a SQLite Backup object for the source database and destination database.
     * The database name is "main" for the main database, "temp" for the temporary database,
     * or the name specified after the AS keyword in an ATTACH statement for an attached database.
     *
     * Exception is thrown in case of error, then the Backup object is NOT constructed.
     *
     * @param[in] aDestDatabase        Destination database connection
     * @param[in] apDestDatabaseName   Destination database name
     * @param[in] aSrcDatabase         Source database connection
     * @param[in] apSrcDatabaseName    Source database name
     *
     * @throw SQLite::Exception in case of error
     */
    Backup(Database&   aDestDatabase,
           const char* apDestDatabaseName,
           Database&   aSrcDatabase,
           const char* apSrcDatabaseName);

    /**
     * @brief Initialize a SQLite Backup object.
     *
     * Initialize a SQLite Backup object for source database and destination database.
     * The database name is "main" for the main database, "temp" for the temporary database,
     * or the name specified after the AS keyword in an ATTACH statement for an attached database.
     *
     * Exception is thrown in case of error, then the Backup object is NOT constructed.
     *
     * @param[in] aDestDatabase        Destination database connection
     * @param[in] aDestDatabaseName    Destination database name
     * @param[in] aSrcDatabase         Source database connection
     * @param[in] aSrcDatabaseName     Source database name
     *
     * @throw SQLite::Exception in case of error
     */
    Backup(Database&          aDestDatabase,
           const std::string& aDestDatabaseName,
           Database&          aSrcDatabase,
           const std::string& aSrcDatabaseName);

    /**
     * @brief Initialize a SQLite Backup object for main databases.
     *
     * Initialize a SQLite Backup object for source database and destination database.
     * Backup the main databases between the source and the destination.
     *
     * Exception is thrown in case of error, then the Backup object is NOT constructed.
     *
     * @param[in] aDestDatabase        Destination database connection
     * @param[in] aSrcDatabase         Source database connection
     *
     * @throw SQLite::Exception in case of error
     */
    Backup(Database& aDestDatabase,
           Database& aSrcDatabase);

    /**
     * @brief Release the SQLite Backup resource.
     */
    virtual ~Backup() noexcept;

    /**
     * @brief Execute a step of backup with a given number of source pages to be copied
     *
     * Exception is thrown when SQLITE_IOERR_XXX, SQLITE_NOMEM, or SQLITE_READONLY is returned
     * in sqlite3_backup_step(). These errors are considered fatal, so there is no point
     * in retrying the call to executeStep().
     *
     * @param[in] aNumPage    The number of source pages to be copied, with a negative value meaning all remaining source pages
     *
     * @return SQLITE_OK/SQLITE_DONE/SQLITE_BUSY/SQLITE_LOCKED
     *
     * @throw SQLite::Exception in case of error
     */
    int executeStep(const int aNumPage = -1);

    /**
     * @brief Get the remaining number of source pages to be copied.
     *
     * @return the remaining number of source pages to be copied
     */
    int remainingPageCount();

    /**
     * @brief Get the total number of source pages.
     *
     * @return the total number of source pages
     */
    int totalPageCount();

    /**
     * @brief Return raw pointer to SQLite Database Backup Handle.
     *
     * This is often needed to mix this wrapper with other libraries or for advance usage not supported by SQLiteCpp.
     *
     * @return Raw pointer to SQLite Backup Handle
    */
    inline sqlite3_backup* getHandle() const noexcept // nothrow
    {
        return mpSQLiteBackup;
    }

private:
    /// @{ Backup must be non-copyable
    Backup(const Backup&);
    Backup& operator=(const Backup&);
    /// @}

private:
    sqlite3_backup* mpSQLiteBackup;   //!< Pointer to SQLite Database Backup Handle
};

}  // namespace SQLite
