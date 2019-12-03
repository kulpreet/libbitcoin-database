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

#include <bitcoin/database/transaction_management/transaction_context.hpp>
#include <bitcoin/database/transaction_management/transaction_manager.hpp>

namespace libbitcoin {
namespace database {

transaction_manager::transaction_manager()
 : time_{ timestamp_t(0) }
{
}

transaction_context transaction_manager::begin_transaction()
{
    // TODO add spin latch, RAII on a block
    auto start_time = time_++;
    // TODO end spin latch here

    transaction_context context(start_time, state::active);

    // TODO add spin latch on current transactions, RAII on a block
    current_transactions_.emplace(start_time);
    // TODO end spin latch

    return context;
}

void transaction_manager::commit_transaction(transaction_context& context) const
{
    context.set_state(state::committed);
}

bool transaction_manager::is_active(const transaction_context& context) const
{
    // Can check this without a lock
    if (context.get_state() != state::active)
    {
        return false;
    }

    // TODO add spin latch on current transactions, RAII on a block
    transaction_set::const_iterator existing =
        current_transactions_.find(context.get_timestamp());
    return existing != current_transactions_.end();
    // TODO end spin latch
}

void transaction_manager::remove_transaction(const transaction_context& context)
{
    BITCOIN_ASSERT(context.get_state() == state::committed);

    // TODO add spin latch on current transactions, RAII on a block
    current_transactions_.erase(context.get_timestamp());
    // TODO end spin latch
}

} // namespace database
} // namespace libbitcoin
