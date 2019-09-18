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

#include <bitcoin/database/transaction_management/transaction_manager.hpp>


namespace libbitcoin {
namespace database {

transaction_manager::transaction_manager(std::function<void()> callback)
  : snapshot_callback_(callback),
    quiescent(false)
{
    active_count_.store(0);
}

void transaction_manager::begin_transaction()
{
    active_count_.fetch_add(1, std::memory_order_relaxed);
}

void transaction_manager::end_transaction()
{
    if (active_count_.fetch_sub(1, std::memory_order_relaxed) == 1) {
        if (quiescent)
            snapshot_callback_();
    }
}

void transaction_manager::enter_quiescent()
{
    quiescent = true;
}

void transaction_manager::leave_quiescent()
{
    quiescent = false;
}

}
}
