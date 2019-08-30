#ifndef __AMY_MARIADB_CONNECTOR_HPP__
#define __AMY_MARIADB_CONNECTOR_HPP__

#include <amy/basic_connector.hpp>
#include <amy/basic_results_iterator.hpp>
#include <amy/basic_scoped_transaction.hpp>
#include <amy/mariadb_service.hpp>

namespace amy {

using mariadb_connector = basic_connector<mariadb_service>;

using mariadb_results_iterator = basic_results_iterator<mariadb_service>;

using mariadb_scoped_transaction = basic_scoped_transaction<mariadb_service>;

} // namespace amy

#endif // __AMY_MARIADB_CONNECTOR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
