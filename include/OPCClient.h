/*
 * Cinder Friendly Client for Open Pixel Control
 *
 * James Hurlbut @ Stimulant 2014
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "TcpClient.h"

class OPCClient;

typedef boost::shared_ptr< OPCClient > OPCClientRef;

class OPCClient {
public:
    static OPCClientRef create()
    {
        return ( OPCClientRef )( new OPCClient() );
    }

    OPCClient();
    ~OPCClient();
	
	bool connect(std::string pHost, int pPort = 7890);
	void	update();
    bool	isConnected();
	bool	tryConnect();
	void						write(std::string strBuffer);
	void						write(std::vector<char> &data);
	
	template< typename T, typename Y >
	inline void		connectConnectEventHandler( T eventHandler, Y* eventHandlerObject )
	{
		connectConnectEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1 ) );
	}

	void			connectConnectEventHandler( const std::function<void( TcpSessionRef )>& eventHandler );

	template< typename T, typename Y >
	inline void		connectErrorEventHandler( T eventHandler, Y* eventHandlerObject )
	{
		connectErrorEventHandler( std::bind( eventHandler, eventHandlerObject, std::placeholders::_1, std::placeholders::_2 ) );
	}
	void			connectErrorEventHandler( const std::function<void( std::string, size_t )>& eventHandler );

	struct Header {
        uint8_t channel;
        uint8_t command;
        uint8_t length[2];

        void init(uint8_t channel, uint8_t command, uint16_t length) {
            this->channel = channel;
            this->command = command;
            this->length[0] = length >> 8;
            this->length[1] = (uint8_t)length;
        }

        uint8_t *data() {
            return (uint8_t*) &this[1];
        }
        const uint8_t *data() const {
            return (uint8_t*) &this[1];
        }

        // Use a Header() to manipulate packet data in a std::vector
        static Header& view(std::vector<char> &data) {
            return *(Header*) &data[0];
        }
        static const Header& view(const std::vector<char> &data) {
            return *(Header*) &data[0];
        }
    };

	// Commands
    static const uint8_t SET_PIXEL_COLORS = 0;
	
	void						onConnect( TcpSessionRef session );
	void						onError( std::string err, size_t bytesTransferred );
private:
    TcpClientRef				mClient;
	TcpSessionRef				mSession;
	std::string					mHost;
	int32_t						mPort;
	std::string					mRequest;
	bool						mConnecting;
	
	void						onRead( ci::Buffer buffer );
	void						onWrite( size_t bytesTransferred );

	std::shared_ptr<boost::asio::io_service>	mIo;
};
