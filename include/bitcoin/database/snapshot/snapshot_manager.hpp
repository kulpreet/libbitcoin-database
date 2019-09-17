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
#include <bitcoin/system/utility/asio.hpp>
#include <bitcoin/database/define.hpp>

namespace libbitcoin {
namespace database {

class BCD_API snapshot_manager
{
public:

    // Construct a snapshot_manager. Starts a timer to take snapshots
    // every period seconds.
    snapshot_manager(size_t period);

    /// Callback invoked from transaction_manager all active
    /// transactions have finished.
    /// transaction_manager::snapshot_callback invokes this member
    /// function.
    void take_snapshot();

private:

    /// Period in seconds to start the snapshot process.
    size_t period_;

    /// Callback on timer expiry. Starting point of snapshot process.
    void handle_snapshot_timer(const system::code& ec);

    /// asio based timer triggered every period_ seconds.
    system::deadline::ptr timer;
};

} // namespace database
} // namespace libbitcoin


#endif
