/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/database/record/record_allocator.hpp>

#include <cstddef>
#include <boost/thread.hpp>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/disk/mmfile.hpp>

namespace libbitcoin {
namespace database {

record_allocator::record_allocator(mmfile& file, file_offset sector_start,
    size_t record_size)
  : file_(file), start_(sector_start), count_(0), record_size_(record_size)
{
}

void record_allocator::create()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    //count_ = 0;
    write_count();
    ///////////////////////////////////////////////////////////////////////////
}

void record_allocator::start()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::shared_lock<boost::shared_mutex> unique_lock(mutex_);

    read_count();
    ///////////////////////////////////////////////////////////////////////////
}

void record_allocator::sync()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::unique_lock<boost::shared_mutex> unique_lock(mutex_);

    write_count();
    ///////////////////////////////////////////////////////////////////////////
}

// new record allocation
array_index record_allocator::new_record(/* size_t records=1 */)
{
    static constexpr size_t records = 1;

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::unique_lock<boost::shared_mutex> unique_lock(mutex_);

    const auto record_index = count_.load();
    reserve(records);
    return record_index;
    ///////////////////////////////////////////////////////////////////////////
}

// logical record access
const record_byte_pointer record_allocator::get_record(array_index record) const
{
    const auto reader = file_.reader();
    const auto offset = start_ + record_to_position(record);
    const auto read_position = reader.buffer() + offset;
    return const_cast<uint8_t*>(read_position);
}

array_index record_allocator::count() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::shared_lock<boost::shared_mutex> shared_lock(mutex_);

    return count_.load();
    ///////////////////////////////////////////////////////////////////////////
}

void record_allocator::count(const array_index records)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    boost::unique_lock<boost::shared_mutex> unique_lock(mutex_);

    BITCOIN_ASSERT(records <= count_.load());

    count_.store(records);
    ///////////////////////////////////////////////////////////////////////////
}

// privates

void record_allocator::reserve(size_t count)
{
    const size_t required_size = start_ + record_to_position(count_ + count);
    file_.resize(required_size);
    count_ += count;
}

// Read the count value from the first chunk of the file.
// Since the write is not atomic this must be read before write is enabled.
void record_allocator::read_count()
{
    BITCOIN_ASSERT(sizeof(array_index) <= file_.size());

    const auto reader = file_.reader();
    const auto read_position = reader.buffer() + start_;

    count_.store(from_little_endian_unsafe<array_index>(read_position));
}

// Write the count value to the first chunk of the file.
// This write is not atomic and therefore assumes there are no readers.
void record_allocator::write_count()
{
    BITCOIN_ASSERT(sizeof(array_index) <= file_.size());

    auto writer = file_.writer(file_.size());
    auto write_position = writer.buffer() + start_;

    auto serial = make_serializer(write_position);
    serial.write_little_endian(count_.load());
}

// A record allocator is a slab allocator where the record size is fixed.
// These methods convert from logical record position to byte-wise position
// relative to start.
//array_index record_allocator::position_to_record(file_offset position) const
//{
//    return (position - sizeof(array_index)) / record_size_;
//}

file_offset record_allocator::record_to_position(array_index record) const
{
    // The position is relative to start, so we must add the size prefix.
    return sizeof(array_index) + record * record_size_;
}
} // namespace database
} // namespace libbitcoin
