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
#include <boost/test/unit_test.hpp>
#include <bitcoin/database/transaction_management/transaction_manager.hpp>

using namespace bc;
using namespace bc::database;

BOOST_AUTO_TEST_SUITE(transaction_management_tests)

BOOST_AUTO_TEST_CASE(transaction_manager__begin_transaction__none__active_and_first_timestamp)
{
    transaction_manager instance{};
    auto context = instance.begin_transaction();

    BOOST_REQUIRE_EQUAL(context.get_timestamp(), timestamp_t(0));
    BOOST_REQUIRE(context.get_state() == state::active);
    BOOST_REQUIRE(instance.is_active(context));
}

BOOST_AUTO_TEST_CASE(transaction_manager__begin_transaction__second__active_and_second_timestamp)
{
    transaction_manager instance{};
    auto context = instance.begin_transaction();
    auto second_context = instance.begin_transaction();

    BOOST_REQUIRE_EQUAL(context.get_timestamp(), timestamp_t(0));
    BOOST_REQUIRE(context.get_state() == state::active);
    BOOST_REQUIRE(instance.is_active(context));

    BOOST_REQUIRE_EQUAL(second_context.get_timestamp(), timestamp_t(1));
    BOOST_REQUIRE(second_context.get_state() == state::active);
    BOOST_REQUIRE(instance.is_active(second_context));
}

BOOST_AUTO_TEST_CASE(transaction_manager__commit_transaction__commit_first__second_still_active)
{
    transaction_manager instance{};
    auto context = instance.begin_transaction();
    auto second_context = instance.begin_transaction();

    instance.commit_transaction(context);

    BOOST_REQUIRE_EQUAL(context.get_timestamp(), timestamp_t(0));
    BOOST_REQUIRE(context.get_state() == state::committed);
    BOOST_REQUIRE(!instance.is_active(context));

    BOOST_REQUIRE_EQUAL(second_context.get_timestamp(), timestamp_t(1));
    BOOST_REQUIRE(second_context.get_state() == state::active);
    BOOST_REQUIRE(instance.is_active(second_context));
}

BOOST_AUTO_TEST_CASE(transaction_manager__remove_transaction__commit_and_remove_first__second_still_active)
{
    transaction_manager instance{};
    auto context = instance.begin_transaction();
    auto second_context = instance.begin_transaction();

    instance.commit_transaction(context);
    instance.remove_transaction(context);

    BOOST_REQUIRE_EQUAL(context.get_timestamp(), timestamp_t(0));
    BOOST_REQUIRE(context.get_state() == state::committed);
    BOOST_REQUIRE(!instance.is_active(context));

    BOOST_REQUIRE_EQUAL(second_context.get_timestamp(), timestamp_t(1));
    BOOST_REQUIRE(second_context.get_state() == state::active);
    BOOST_REQUIRE(instance.is_active(second_context));
}

BOOST_AUTO_TEST_SUITE_END()
