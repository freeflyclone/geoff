#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <boost/beast/core.hpp>

namespace beast = boost::beast;

beast::string_view
mime_type(beast::string_view path);

std::string
path_cat(beast::string_view base, beast::string_view path);

void
fail(beast::error_code ec, char const* what);

#endif