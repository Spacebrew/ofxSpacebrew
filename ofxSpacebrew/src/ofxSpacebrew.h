//
//  SpaceBrew.h
//  ofxSpacebrew
//
//  Created by Brett Renfer on 8/17/12.
//
//

#pragma once

#if !defined(OFX_WSPP) && !defined(OFX_LWS)
	#define OFX_LWS     // ofxLibwebsockets
	//#define OFX_WSPP    // ofxWebsocketpp
#endif

#ifdef OFX_LWS
    #include "ofxLibwebsockets.h"
#else
    #include "ofxWebsocketppClient.h"
    using namespace ofxWebsocketpp::wsClient;
#endif


#include "ofUtils.h"
#include "json.h"

namespace Spacebrew {
    
    static const int SPACEBREW_PORT = 9000;
    
    
    class Message {
        public:
            
            Message( string _name="", string _type="", string _val=""){
                name = _name;
                type = _type;
                _default = value = _val;
            }
            
            virtual string getJSON( string configName ){
                return "{\"message\":{\"clientName\":\"" + configName +"\",\"name\":\"" + name + "\",\"type\":\"" + type + "\",\"value\":\"" + value +"\"}}";
            }
            
            string name, type, _default, value;
    };
    
    class Config {
        public:
            
            void addSubscribe( string name, string type );
            void addSubscribe( Message m );
        
            void addPublish( string name, string type, string def);
            void addPublish( Message m );
        
            string getJSON();
            string name, description;
            
        private:
            
            vector<Message> publish;
            vector<Message> subscribe;
    };
    
    class Connection {
        public:
            Connection();
			~Connection();
        
			void update( ofEventArgs & e );

            void connect( string host, string name, string description);
            void connect( string host, Config _config );
        
            void send( string name, string type, string value );
            void send( Message m );
            void send( Message * m );
        
            // edit config
            void addSubscribe( string name, string type );
            void addSubscribe( Message m );
            
            void addPublish( string name, string type, string def);
            void addPublish( Message m );
        
            // websocket methods
#ifdef OFX_LWS
            void onConnect( ofxLibwebsockets::Event& args );
            void onOpen( ofxLibwebsockets::Event& args );
            void onClose( ofxLibwebsockets::Event& args );
            void onIdle( ofxLibwebsockets::Event& args );
            void onMessage( ofxLibwebsockets::Event& args );
            void onBroadcast( ofxLibwebsockets::Event& args );
#else
            
            void sendBinary(ofxWebsocketpp::wsClient::client::connection_ptr con, const string& binary);
            void sendString(ofxWebsocketpp::wsClient::client::connection_ptr con, const string& str);
            
            void onClientSocketMessage(ofxWebsocketpp::wsClient::websocketMessageEvent &event);
            
            void onClientSocketPing(ofxWebsocketpp::wsClient::websocketPingEvent &event);
            void onClientSocketPong(ofxWebsocketpp::wsClient::websocketPingEvent &event);
            void onClientSocketPongFail(ofxWebsocketpp::wsClient::websocketPingEvent &event);
            
            void onClientSocketHandshake(ofxWebsocketpp::wsClient::websocketConnectionEvent &event);
            void onClientSocketOpen(ofxWebsocketpp::wsClient::websocketConnectionEvent &event);
            void onClientSocketClose(ofxWebsocketpp::wsClient::websocketConnectionEvent &event);
            void onClientSocketFail(ofxWebsocketpp::wsClient::websocketConnectionEvent &event);
        
#endif
        
            Config * getConfig();
        
            bool isConnected();
			void setAutoReconnect( bool bAutoReconnect=true );
			void setReconnectRate( int reconnectMillis );
			bool doesAutoReconnect();
        
            ofEvent<Message> onMessageEvent;
        
        protected:
        
			string host;
            bool bConnected;
            void updatePubSub();
        
            Config config;

			// reconnect
			bool bAutoReconnect;
			int	 lastTimeTriedConnect;
			int	 reconnectInterval;
        
    #ifdef OFX_LWS
        
            ofxLibwebsockets::Client            _client;
        
    #else
            ofxWebsocketpp::wsClient::client    _client;
            Json::Reader                        _jsonParser;
        
    #endif
    };
}
