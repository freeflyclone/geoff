#ifndef SSL_CONTEXT_H
#define SSL_CONTEXT_H

#include "geoff.h"

class SSLContext : public ssl::context
{
public:
    SSLContext(const std::string & certs_folder, boost::system::error_code & ec);

private:
    const std::string m_lets_encrypt {"/etc/letsencrypt/"};
};

#endif