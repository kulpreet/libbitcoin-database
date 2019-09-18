/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DATABASE_TRANSACTION_MANAGER_HPP
#define LIBBITCOIN_DATABASE_TRANSACTION_MANAGER_HPP

#include <bitcoin/system.hpp>
#include <bitcoin/database/define.hpp>

namespace libbitcoin {
namespace database {

/// transaction_manager provides a means to start and commit
/// transactions.
/// If the database is in the quiescent mode to take a snapshot, no
/// new tranasactions are allowed to start. All begin_transactions
/// requests wait on a spin lock.
class BCD_API transaction_manager
{
public:

    /// Construct the transaction manager
    transaction_manager(std::function<void()> callback);

    /// Begin a transaction. Caller synchronously waits for a
    /// transaction_context.
    void begin_transaction();

    /// End a transaction. Caller synchronously waits for the
    /// active_list to be decremented. If active_count reduces to
    /// zero and transaction_manager is in quiescent phase, then call
    /// snapshot_callback.
    void end_transaction();

    // Start quiescent phase. No new transactions are allowed to
    // start.
    void enter_quiescent();

    // Leave quiescent phase. Transactions waiting will now get the
    // transaction_contexts and can start processing.
    void leave_quiescent();

private:

    /// Snapshot manager callback to invoke when last transaction
    /// leaves
    std::function<void()> snapshot_callback_;

    std::atomic<size_t> active_count_;

    // Set and read by a single thread. Doesn't need to be atomic.
    bool quiescent;
};

} // namespace database
} // namespace libbitcoin


#endif
