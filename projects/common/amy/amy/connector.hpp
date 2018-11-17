#ifndef __AMY_CONNECTOR_HPP__
#define __AMY_CONNECTOR_HPP__

#include <amy/basic_connector.hpp>
#include <amy/basic_results_iterator.hpp>
#include <amy/basic_scoped_transaction.hpp>
#include <amy/mysql_service.hpp>

namespace amy {

typedef
    basic_connector<mysql_service>
    connector;

typedef
    basic_results_iterator<mysql_service>
    results_iterator;

typedef
    basic_scoped_transaction<mysql_service>
    scoped_transaction;

} // namespace amy

#endif // __AMY_CONNECTOR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
