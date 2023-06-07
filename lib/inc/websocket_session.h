#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include "geoff.h"

// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template<class Derived>
class websocket_session
{
    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.
    Derived&
        derived()
    {
        return static_cast<Derived&>(*this);
    }

    beast::flat_buffer buffer_;

    // Set by WebsockServer::OnAccept() callback, so that when we detect
    // a websocket_session close, we know which WebsockServer client to clean up.
    uint32_t m_sessionID;
    bool m_write_active{ false };

    // Start the asynchronous operation
    template<class Body, class Allocator>
    void do_accept(http::request<Body, http::basic_fields<Allocator>> req)
    {
        // Set suggested timeout settings for the websocket
        derived().ws().set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        derived().ws().set_option(
            websocket::stream_base::decorator(
                [](websocket::response_type& res)
        {
            res.set(http::field::server,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " GEOFF version " + GEOFF_Version);
        }));

        // Accept the websocket handshake
        derived().ws().async_accept(
            req,
            beast::bind_front_handler(
                &websocket_session::on_accept,
                derived().shared_from_this()));
    }

    void on_accept(beast::error_code ec)
    {
        if (ec)
            return fail(ec, "accept");

        // -------------------------------------------------------------------------------
        // when this call returns, a new WebsockSession for "m_sessionID" has been created.
        // -------------------------------------------------------------------------------
        WebsockServer::GetInstance().OnAccept([&](uint32_t sessionID) {
            m_sessionID = sessionID;
        });

        // Set the OnTxReadyCallback_t function member of the WebsockSession to this lambda
        // We get one for each session, thus each session now has its own OnTxReady interrupt
        // for processing TimerTick() messages.
        WebsockServer::GetInstance().FindSessionByID(m_sessionID)->SetOnTxReadyCallback([this](WebsockSession & session) {
            post(derived().ws().get_executor(), [&]() {
                //TRACE("Posted");

                if (session.TxQueueEmpty())
                    return;

                std::unique_ptr<AppBuffer> txBuffer;
                if (session.GetNextTxBuffer(txBuffer))
                {
                    do_write(txBuffer->data(), txBuffer->bytesWritten());
                }
            });
        });

        do_read();

    }

    void do_read()
    {
        // Read a message into our buffer
        derived().ws().async_read(
            buffer_,
            beast::bind_front_handler(
                &websocket_session::on_read,
                derived().shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This indicates that the websocket_session was closed
        if (ec == websocket::error::closed)
        {
            WebsockServer::GetInstance().OnClose(m_sessionID);
            return;
        }

        if (ec)
            return fail(ec, "read");

        // Invoke the WebsockServer comms handler...
        WebsockServer::GetInstance().CommsHandler(m_sessionID, buffer_, bytes_transferred);

        // ... and return results (if any) to client
        auto session = WebsockServer::GetInstance().FindSessionByID(m_sessionID);
        if (session)
        {
            if (!session->TxQueueEmpty())
            {
                std::unique_ptr<AppBuffer> txBuff;

                if (session->GetNextTxBuffer(txBuff))
                {
                    do_write(txBuff->data(), txBuff->bytesWritten());
                }
            }
        }

        // start another async_read() no matter what.
        buffer_.consume(buffer_.size());
        do_read();
    }

    void do_write(uint8_t* buffer, size_t length)
    {
        if (m_write_active)
            return;

        m_write_active = true;

        derived().ws().async_write(
            boost::asio::buffer(buffer, length),
            beast::bind_front_handler(
                &websocket_session::on_write,
                derived().shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if (ec)
            return fail(ec, "write");

        m_write_active = false;

        // Server side is now allowed to send multiple messages back-to-back,
        // independently of input from the client.
        // 
        // Thus the async_write() / on_write() chain is managed based on
        // whether WebsockServer has tx buffers queued or not.
        auto session = WebsockServer::GetInstance().FindSessionByID(m_sessionID);
        if (session)
        {
            if (!session->TxQueueEmpty())
            {
                std::unique_ptr<AppBuffer> txBuff;

                if (session->GetNextTxBuffer(txBuff))
                {
                    do_write(txBuff->data(), txBuff->bytesWritten());
                }
            }
        }
    }

public:
    // Start the asynchronous operation
    template<class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> req)
    {
        // Accept the WebSocket upgrade request
        do_accept(std::move(req));
    }
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

// Handles an SSL WebSocket connection
class ssl_websocket_session : 
    public websocket_session<ssl_websocket_session> ,
    public std::enable_shared_from_this<ssl_websocket_session>
{
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

public:
    // Create the ssl_websocket_session
    explicit ssl_websocket_session(beast::ssl_stream<beast::tcp_stream>&& stream) : 
        ws_(std::move(stream))
    {
        ws_.binary(true);

        const boost::asio::ip::tcp::no_delay option(true);
        ws_.next_layer().next_layer().socket().set_option(option);
    }

    // Called by the base class
    websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws()
    {
        return ws_;
    }
};

//------------------------------------------------------------------------------

// Handles a plain WebSocket connection
class plain_websocket_session :
    public websocket_session<plain_websocket_session>,
    public std::enable_shared_from_this<plain_websocket_session>
{
    websocket::stream<beast::tcp_stream> ws_;

public:
    // Create the session
    explicit plain_websocket_session(beast::tcp_stream&& stream)
        : ws_(std::move(stream))
    {
        // This line is a requirement for binary data!
        // I was missing it in looking at Tim's code, but Wireshark showed me the
        // packet was in text mode. (Note to Self: Wireshark is your friend!)
        ws_.binary(true);

        // Disable Nagle algorithm
        const boost::asio::ip::tcp::no_delay option(true);
        ws_.next_layer().socket().set_option(option);
    }

    // Called by the base class
    websocket::stream<beast::tcp_stream>& ws()
    {
        return ws_;
    }
};

#endif
