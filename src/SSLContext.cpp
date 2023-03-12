#include "SSLContext.h"

SSLContext::SSLContext(const std::string & certs_folder, boost::system::error_code & ec)
    : ssl::context(ssl::context::tlsv12)
{
    std::string chain_file = m_lets_encrypt + "live/" + certs_folder + "/fullchain.pem";
    std::string privkey_file = m_lets_encrypt + "live/" + certs_folder + "/privkey.pem";
    std::string dhparams_file = m_lets_encrypt + "/ssl-dhparams.pem";

    set_password_callback(
        [](std::size_t, 
        ssl::context_base::password_purpose)
        {
            std::cout << "password_callback invoked" << std::endl;
            return "test";
        }
    );

    set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::single_dh_use);

    use_certificate_chain_file(chain_file, ec);
    if (ec)
    {
        fail(ec, "use_certificate_chain_file");
        return;
    }

    use_private_key_file(privkey_file, ssl::context::file_format::pem, ec);
    if (ec)
    {
        fail(ec,"use_private_key_file");
        return;
    }

    use_tmp_dh_file(dhparams_file, ec);
    if (ec)
    {
        fail(ec, "use_tmp_dh_file");
        return;
    }
}