/* Basic type aliases and forward declarations.
 *
 * Copyright (c) 2000-2021, Jeroen T. Vermeulen
 *
 * See COPYING for copyright license.  If you did not receive a file called
 * COPYING with this source code, please notify the distributor of this
 * mistake, or contact the author.
 */
#ifndef PQXX_H_TYPES
#define PQXX_H_TYPES

#include <cstddef>
#include <cstdint>

namespace pqxx
{
/// Number of rows in a result set.
using result_size_type = int;

/// Difference between result sizes.
using result_difference_type = int;

/// Number of fields in a row of database data.
using row_size_type = int;

/// Difference between row sizes.
using row_difference_type = int;

/// Number of bytes in a field of database data.
using field_size_type = std::size_t;

/// Number of bytes in a large object.
using large_object_size_type = int64_t;


// Forward declarations, to help break compilation dependencies.
// These won't necessarily include all classes in libpqxx.
class binarystring;
class connection;
class const_result_iterator;
class const_reverse_result_iterator;
class const_reverse_row_iterator;
class const_row_iterator;
class dbtransaction;
class field;
class largeobjectaccess;
class notification_receiver;
struct range_error;
class result;
class row;
class stream_from;
class transaction_base;

/// Marker for @c stream_from constructors: "stream from table."
/** @deprecated Use stream_from::table() instead.
 */
struct from_table_t
{};

/// Marker for @c stream_from constructors: "stream from query."
/** @deprecated Use stream_from::query() instead.
 */
struct from_query_t
{};


/// Format code: is data text or binary?
/** Binary-compatible with libpq's format codes.
 */
enum class format : int
{
  text = 0,
  binary = 1,
};
} // namespace pqxx

#endif
