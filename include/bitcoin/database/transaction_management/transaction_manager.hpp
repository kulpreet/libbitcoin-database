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
#ifndef LIBBITCOIN_DATABASE_DATA_BASE_HPP
#define LIBBITCOIN_DATABASE_DATA_BASE_HPP

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
    transaction_manager();

    /// Begin a transaction. Caller synchronously waits for a
    /// transaction_context.
    const transaction_context& begin_transaction();

    /// End a transaction. Caller synchronously waits for the
    /// transaction context to be removed from the active list. If
    /// this is the last transaction in active and transaction_manager
    /// is in quiescent phase, then call snapshot_callback.
    bool end_transaction(const transaction_context&);

    /// Number of active transactions at the moment.
    int count_active() const;

    // Start quiescent phase. No new transactions are allowed to
    // start.
    bool enter_quiescent();

    // Leave quiescent phase. Transactions waiting will now get the
    // transaction_contexts and can start processing.
    bool leave_quiescent();

private:

    /// Active transactions in the system.
    std::vector<const transaction_context&> active;

    /// Snapshot manager callback to invoke when last transaction
    /// leaves
    system::handle0 snapshot_callback;
};

} // namespace database
} // namespace libbitcoin


#endif
